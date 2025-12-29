#include "broken_heart.h"
#include "buffer.h"
#include "enemy.h"
#include "font.h"
#include "heart.h"
#include "math.h"
#include "player.h"
#include "version.h"

#include <SDL3/SDL_init.h>

#define SDL_MAIN_USE_CALLBACKS 1

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#define die(s)                      \
    {                               \
        fprintf(stderr, "%s\n", s); \
        exit(1);                    \
    }
#define sdl_die(s)                  \
    {                               \
        SDL_Log(s, SDL_GetError()); \
        return SDL_APP_FAILURE;     \
    }

#define STR(s)  #s
#define XSTR(s) STR(s)

#define load_png_file(array, size, dst)                                 \
    {                                                                   \
        SDL_Surface  *surface;                                          \
        SDL_IOStream *sdl_stream;                                       \
                                                                        \
        if (!(sdl_stream = SDL_IOFromConstMem(array, size)))            \
            sdl_die("couldn't make a const mem sdl stream: %s\n");      \
                                                                        \
        if (!(surface = IMG_LoadPNG_IO(sdl_stream)))                    \
            sdl_die("couldn't load png from stream: %s\n");             \
        SDL_CloseIO(sdl_stream);                                        \
                                                                        \
        if (!((dst) = SDL_CreateTextureFromSurface(renderer, surface))) \
            sdl_die("couldn't create texture from surface: %s\n");      \
        SDL_DestroySurface(surface);                                    \
    }

static Buffer buffer = {0};

static SDL_Window   *window   = NULL;
static SDL_Renderer *renderer = NULL;

static TTF_Font *font = NULL;

static SDL_Texture *player_texture      = NULL;
static SDL_Texture *dead_player_texture = NULL;

static Player player        = {.alive = true};
static Enemy  enemies[1]    = {0};
static size_t alive_enemies = 0;

static bool has_more_commits = true;

enum Inputs { INPUT_LEFT, INPUT_RIGHT, INPUT_UP, INPUT_DOWN, INPUT_SHIFT };
static bool inputs[5];

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
    (void)appstate;
    (void)argc;
    (void)argv;

    SDL_SetAppMetadata("GitCommitsBulletHell",
                       GitCommitsBulletHell_VERSION,
                       "io.github.p1k0chu.gitcommitsbullethell");

    if (!SDL_Init(SDL_INIT_VIDEO)) sdl_die("Couldn't init sdl video: %s\n");

    if (!SDL_CreateWindowAndRenderer("Commits Bullet Hell",
                                     1280,
                                     720,
                                     SDL_WINDOW_FULLSCREEN | SDL_WINDOW_RESIZABLE,
                                     &window,
                                     &renderer))
        sdl_die("Couldn't create window/renderer: %s\n");

    if (!TTF_Init()) sdl_die("Couldn't initialize SDL3_ttf: %s\n");

    if (!(font = TTF_OpenFontIO(SDL_IOFromConstMem(tiny_ttf, tiny_ttf_len), true, 30.0f)))
        sdl_die("Couldn't open font: %s\n");

    load_png_file(heart_png, heart_png_len, player_texture);
    load_png_file(broken_heart_png, broken_heart_png_len, dead_player_texture);

    SDL_GetTextureSize(player_texture, &player.w, &player.h);

    SDL_SetRenderVSync(renderer, 1);

    int   git_pipe[2];
    pid_t pid;

    buffer.buffer_start = calloc(10, sizeof(char));
    buffer.buffer_size  = 10 * sizeof(char);

    if (pipe(git_pipe) == -1) die("pipe");

    buffer.fd = git_pipe[0];

    if ((pid = fork()) == -1) die("fork");

    if (pid == 0) {
        dup2(git_pipe[1], STDOUT_FILENO);
        close(git_pipe[0]);
        close(git_pipe[1]);
        execlp("git", "git", "log", "--pretty=%s", NULL);
        exit(0);
    }

    close(git_pipe[1]);

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
        }
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    (void)appstate;

    const float scale = 1.0f;
    SDL_SetRenderScale(renderer, scale, scale);

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

    float speed_mul = (float)dt / 1000.f;

    if (inputs[INPUT_SHIFT]) {
        speed_mul *= PLAYER_SHIFT_SPEED_MUL;
    }

    if (inputs[INPUT_LEFT]) {
        player.x -= PLAYER_SPEED * speed_mul;
    } else if (inputs[INPUT_RIGHT]) {
        player.x += PLAYER_SPEED * speed_mul;
    }

    if (inputs[INPUT_UP]) {
        player.y -= PLAYER_SPEED * speed_mul;
    } else if (inputs[INPUT_DOWN]) {
        player.y += PLAYER_SPEED * speed_mul;
    }

    speed_mul = (float)dt / 1000.f;

    int w, h;
    SDL_GetRenderOutputSize(renderer, &w, &h);

    SDL_GetTextureSize(player_texture, &dst.w, &dst.h);

    for (size_t i = 0; i < alive_enemies; ++i) {
        Enemy *enemy = enemies + i;
        enemy->speed *= ENEMY_ACCEL;

        const float radian = enemy->rotation * M_PI / 180;
        const float c      = SDL_cosf(radian);
        const float s      = SDL_sinf(radian);

        enemy->rect.x += c * enemy->speed * speed_mul;
        enemy->rect.y += s * enemy->speed * speed_mul;

        if (collide(&player, enemy)) {
            player.alive = false;
            return SDL_APP_CONTINUE;
        }

        const float e_w = SDL_fabsf(c * enemy->rect.w + s * enemy->rect.h);
        const float e_h = SDL_fabsf(s * enemy->rect.w + c * enemy->rect.h);

        const float e_center_x = enemy->rect.x + enemy->rect.w / 2;
        const float e_center_y = enemy->rect.y + enemy->rect.h / 2;

        const float e_l = e_center_x - e_w / 2;
        const float e_r = e_l + e_w;

        const float e_t = e_center_y - e_h / 2;
        const float e_b = e_t + e_h;

        if (e_r < 0 || e_l > w || e_b < 0 || e_t > h) {
            SDL_DestroyTexture(enemies[i].texture);
            if (i != alive_enemies - 1) {
                enemies[i] = enemies[alive_enemies - 1];
            }
            --alive_enemies;
        }
    }

    if (has_more_commits && alive_enemies == 0) {
        for (; alive_enemies < sizeof(enemies) / sizeof(enemies[0]); ++alive_enemies) {
            char *line;
            if (!(line = Buffer_get_line(&buffer))) die("Buffer_get_line");

            if (line[0] == 0) {
                has_more_commits = false;
                break;
            }

            enemies[alive_enemies] = (Enemy){
                .speed = ENEMY_SPEED,
            };

            const SDL_Color white_color = {0xff, 0xff, 0xff, 0xff};

            SDL_Surface *surface = TTF_RenderText_Blended(font, line, 0, white_color);
            if (!surface) sdl_die(__FILE_NAME__ ":" XSTR(__LINE__) ": %s\n");

            enemies[alive_enemies].texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_DestroySurface(surface);

            if (!enemies[alive_enemies].texture) sdl_die(__FILE_NAME__ ":" XSTR(__LINE__) ": %s\n");

            SDL_GetTextureSize(enemies[alive_enemies].texture,
                               &enemies[alive_enemies].rect.w,
                               &enemies[alive_enemies].rect.h);

            enemies[alive_enemies].rect.x = w - enemies[alive_enemies].rect.w;
            enemies[alive_enemies].rect.y = 0;

            const float radian = SDL_atan2f(
                player.y - (enemies[alive_enemies].rect.y + enemies[alive_enemies].rect.h / 2.0f),
                player.x - (enemies[alive_enemies].rect.x + enemies[alive_enemies].rect.w / 2.0f));
            float degrees = radian * 180.0f / (float)M_PI;
            while (degrees < 0.0f) degrees += 360.0f;

            enemies[alive_enemies].rotation = degrees;
        }
    }

    SDL_SetRenderScale(renderer, scale, scale);

    for (size_t i = 0; i < alive_enemies; ++i) {
        Enemy *enemy = enemies + i;

        float render_rotation;
        if (enemy->rotation > 90.0f && enemy->rotation < 270.0f) {
            render_rotation = enemy->rotation - 180.0f;
        } else {
            render_rotation = enemy->rotation;
        }

        SDL_RenderTextureRotated(renderer,
                                 enemy->texture,
                                 NULL,
                                 &enemy->rect,
                                 render_rotation,
                                 NULL,
                                 SDL_FLIP_NONE);
    }

    SDL_SetRenderScale(renderer, scale, scale);

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

    close(buffer.fd);
    free(buffer.buffer_start);
}

