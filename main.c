#include "main.h"

#include "broken_heart.h"
#include "enemy.h"
#include "font.h"
#include "git2/types.h"
#include "heart.h"
#include "my_math.h"
#include "pattern.h"
#include "player.h"
#include "utils.h"
#include "version.h"

#define SDL_MAIN_USE_CALLBACKS 1

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <git2.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#define load_png_file(array, size, dst)                                                        \
    {                                                                                          \
        SDL_Surface  *surface;                                                                 \
        SDL_IOStream *sdl_stream;                                                              \
                                                                                               \
        if (!(sdl_stream = SDL_IOFromConstMem(array, size)))                                   \
            sdl_die("couldn't make a const mem sdl stream");                                   \
                                                                                               \
        if (!(surface = IMG_LoadPNG_IO(sdl_stream))) sdl_die("couldn't load png from stream"); \
        SDL_CloseIO(sdl_stream);                                                               \
                                                                                               \
        if (!((dst) = SDL_CreateTextureFromSurface(renderer, surface)))                        \
            sdl_die("couldn't create texture from surface");                                   \
        SDL_DestroySurface(surface);                                                           \
    }

SDL_Window   *window   = NULL;
SDL_Renderer *renderer = NULL;

TTF_Font *font = NULL;

SDL_Texture *player_texture      = NULL;
SDL_Texture *dead_player_texture = NULL;

Player player        = {.alive = 1};
Enemy *enemies       = NULL;
size_t enemies_len   = 0;
size_t alive_enemies = 0;

git_repository *repo   = NULL;
git_revwalk    *walker = NULL;

char inputs[INPUTS_SIZE];
char started = 0;

static SDL_Texture *start_hint = NULL;

static BulletPatternId pattern_id       = Dummy;
static unsigned long   pattern_start_ms = 0;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
    (void)appstate;
    (void)argc;
    (void)argv;

    SDL_SetAppMetadata("GitCommitsBulletHell",
                       GitCommitsBulletHell_VERSION,
                       "io.github.p1k0chu.gitcommitsbullethell");

    if (!SDL_Init(SDL_INIT_VIDEO)) sdl_die("Couldn't init sdl video");

    if (!SDL_CreateWindowAndRenderer("Commits Bullet Hell",
                                     WINDOW_WIDTH,
                                     WINDOW_HEIGHT,
                                     SDL_WINDOW_FULLSCREEN | SDL_WINDOW_RESIZABLE,
                                     &window,
                                     &renderer))
        sdl_die("Couldn't create window/renderer");

    if (!TTF_Init()) sdl_die("Couldn't initialize SDL3_ttf");

    if (!(font = TTF_OpenFontIO(SDL_IOFromConstMem(tiny_ttf, tiny_ttf_len), 1, 30.0)))
        sdl_die("Couldn't open font");

    SDL_Surface *surface;
    if (!(surface = TTF_RenderText_Blended(font,
                                           "Ready? Press [Z] to start.",
                                           0,
                                           (SDL_Color){0xff, 0xff, 0xff, 0xff})))
        sdl_die("couldn't render text");
    if (!(start_hint = SDL_CreateTextureFromSurface(renderer, surface)))
        sdl_die("couldn't create text texture");

    load_png_file(heart_png, heart_png_len, player_texture);
    load_png_file(broken_heart_png, broken_heart_png_len, dead_player_texture);

    SDL_GetTextureSize(player_texture, &player.w, &player.h);
    player.x = (double)WINDOW_WIDTH / 2;
    player.y = (double)WINDOW_HEIGHT / 2;
    player.w *= PLAYER_HITBOX_MUL;
    player.h *= PLAYER_HITBOX_MUL;

    SDL_SetRenderVSync(renderer, 1);
    SDL_SetRenderLogicalPresentation(renderer,
                                     WINDOW_WIDTH,
                                     WINDOW_HEIGHT,
                                     SDL_LOGICAL_PRESENTATION_LETTERBOX);

    int error;
    error = git_libgit2_init();
    if (error < 0) libgit_panic(error);

    error = git_repository_open_ext(&repo, getenv("PWD"), 0, NULL);
    if (error < 0) libgit_panic(error);

    error = git_revwalk_new(&walker, repo);
    if (error < 0) libgit_panic(error);

    error = git_revwalk_push_head(walker);
    if (error < 0) libgit_panic(error);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    (void)appstate;

    switch (event->type) {
    case SDL_EVENT_QUIT:
        return SDL_APP_SUCCESS;
    case SDL_EVENT_KEY_DOWN:
        switch (event->key.key) {
        case SDLK_ESCAPE:
            return SDL_APP_SUCCESS;
        case SDLK_LEFT:
            inputs[INPUT_LEFT] = 1;
            break;
        case SDLK_RIGHT:
            inputs[INPUT_RIGHT] = 1;
            break;
        case SDLK_UP:
            inputs[INPUT_UP] = 1;
            break;
        case SDLK_DOWN:
            inputs[INPUT_DOWN] = 1;
            break;
        case SDLK_LSHIFT:
            inputs[INPUT_SHIFT] = 1;
            break;
        case SDLK_X:
            inputs[INPUT_X] = 1;
            break;
        case SDLK_Z:
            started = 1;
            break;
        }
        break;
    case SDL_EVENT_KEY_UP:
        switch (event->key.key) {
        case SDLK_LEFT:
            inputs[INPUT_LEFT] = 0;
            break;
        case SDLK_RIGHT:
            inputs[INPUT_RIGHT] = 0;
            break;
        case SDLK_UP:
            inputs[INPUT_UP] = 0;
            break;
        case SDLK_DOWN:
            inputs[INPUT_DOWN] = 0;
            break;
        case SDLK_LSHIFT:
            inputs[INPUT_SHIFT] = 0;
            break;
        case SDLK_X:
            inputs[INPUT_X] = 0;
            break;
        }
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    (void)appstate;

    size_t i;

    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xff);
    SDL_RenderClear(renderer);

    SDL_FRect dst = {};

    if (!player.alive) {
        SDL_GetTextureSize(dead_player_texture, &dst.w, &dst.h);

        dst.x = player.x - dst.w / 2;
        dst.y = player.y - dst.h / 2;

        SDL_RenderTexture(renderer, dead_player_texture, NULL, &dst);
        SDL_RenderPresent(renderer);

        return SDL_APP_CONTINUE;
    }

    static unsigned long last_ms = 0;

    unsigned long ms = SDL_GetTicks();
    unsigned long dt = ms - last_ms;
    last_ms          = ms;

    double speed_mul = (double)dt / 1000.f;

    if (inputs[INPUT_SHIFT] || inputs[INPUT_X]) {
        speed_mul *= PLAYER_SHIFT_SPEED_MUL;
    }

    if (inputs[INPUT_LEFT]) {
        player.x -= PLAYER_SPEED * speed_mul;
        if (player.x < player.w / PLAYER_HITBOX_MUL / 2)
            player.x = player.w / PLAYER_HITBOX_MUL / 2;
    } else if (inputs[INPUT_RIGHT]) {
        player.x += PLAYER_SPEED * speed_mul;
        if (player.x > WINDOW_WIDTH - player.w / PLAYER_HITBOX_MUL / 2)
            player.x = WINDOW_WIDTH - player.w / PLAYER_HITBOX_MUL / 2;
    }

    if (inputs[INPUT_UP]) {
        player.y -= PLAYER_SPEED * speed_mul;
        if (player.y < player.h / PLAYER_HITBOX_MUL / 2)
            player.y = player.h / PLAYER_HITBOX_MUL / 2;
    } else if (inputs[INPUT_DOWN]) {
        player.y += PLAYER_SPEED * speed_mul;
        if (player.y > WINDOW_HEIGHT - player.h / PLAYER_HITBOX_MUL / 2)
            player.y = WINDOW_HEIGHT - player.h / PLAYER_HITBOX_MUL / 2;
    }

    if (started) {
        speed_mul = (double)dt / 1000.f;

        SDL_GetTextureSize(player_texture, &dst.w, &dst.h);

        for (i = 0; i < alive_enemies; ++i) {
            Enemy *enemy = enemies + i;
            tick_enemy(enemy->pattern_id, enemy);

            enemy->rect.x += enemy->move_direction.x * enemy->speed * speed_mul;
            enemy->rect.y += enemy->move_direction.y * enemy->speed * speed_mul;

            if (collide(&player, enemy)) {
                player.alive = 0;
                return SDL_APP_CONTINUE;
            }

            Vec2d normals[4] = {{1, 0}, {0, 1}};
            Enemy_get_normals(enemy, normals + 2);

            static const Vec2d screen_points[4] = {{0, 0},
                                                   {WINDOW_WIDTH, 0},
                                                   {0, WINDOW_HEIGHT},
                                                   {WINDOW_WIDTH, WINDOW_HEIGHT}};
            Vec2d              enemy_points[4];
            Enemy_get_points(enemy, enemy_points);

            if (!polygons_collide(normals, 4, enemy_points, 4, screen_points, 4)) {
                SDL_DestroyTexture(enemies[i].texture);
                if (i != alive_enemies - 1) {
                    enemies[i] = enemies[alive_enemies - 1];
                }
                --alive_enemies;
            }
        }

        if (should_start_next_pattern(pattern_id, ms - pattern_start_ms)) {
            if (alive_enemies > 0) goto render;

            pattern_id       = (pattern_id + 1) % BULLET_PATTERN_ID_LEN;
            pattern_start_ms = ms;

#ifndef NDEBUG
            fprintf(stderr, "starting next pattern\n");
#endif
        }

        if (should_spawn_enemies(pattern_id, ms)) {
            spawn_enemies(pattern_id);
        }
    } else {
        SDL_GetTextureSize(start_hint, &dst.w, &dst.h);
        dst.x = (WINDOW_WIDTH - dst.w) / 2;
        dst.y = 0;
        SDL_RenderTexture(renderer, start_hint, NULL, &dst);
    }

render:;
    for (i = 0; i < alive_enemies; ++i) {
        Enemy *enemy = enemies + i;

        SDL_RenderTextureRotated(renderer,
                                 enemy->texture,
                                 NULL,
                                 &enemy->rect,
                                 enemy->rotation,
                                 NULL,
                                 SDL_FLIP_NONE);
    }

    SDL_GetTextureSize(player_texture, &dst.w, &dst.h);
    dst.x = player.x - dst.w / 2;
    dst.y = player.y - dst.h / 2;

    SDL_RenderTexture(renderer, player_texture, NULL, &dst);

    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    (void)appstate;
    (void)result;

    if (font) {
        TTF_CloseFont(font);
    }
    TTF_Quit();
}

