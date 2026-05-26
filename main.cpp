// SPDX-License-Identifier: MIT
// Copyright (C) 2026 p1k0chu

#include "main.hpp"

#include "args_parser.hpp"
#include "broken_heart.h"
#include "enemy.hpp"
#include "font.h"
#include "git2/commit.h"
#include "git2/types.h"
#include "heart.h"
#include "my_math.hpp"
#include "pattern.hpp"
#include "pattern_factory.hpp"
#include "player.hpp"
#include "utils.hpp"
#include "version.h"

#define SDL_MAIN_USE_CALLBACKS 1

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <assert.h>
#include <git2.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

using Patterns::BasePattern;

#define load_png_file(array, size, dst)                                 \
    {                                                                   \
        SDL_Surface *surface;                                           \
        SDL_IOStream *sdl_stream;                                       \
                                                                        \
        if (!(sdl_stream = SDL_IOFromConstMem(array, size)))            \
            sdl_die("couldn't make a const mem sdl stream");            \
                                                                        \
        if (!(surface = IMG_LoadPNG_IO(sdl_stream)))                    \
            sdl_die("couldn't load png from stream");                   \
        SDL_CloseIO(sdl_stream);                                        \
                                                                        \
        if (!((dst) = SDL_CreateTextureFromSurface(renderer, surface))) \
            sdl_die("couldn't create texture from surface");            \
        SDL_DestroySurface(surface);                                    \
    }

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

TTF_Font *font = NULL;

SDL_Texture *player_texture = NULL;
SDL_Texture *dead_player_texture = NULL;

Player player{};

git_repository *repo = NULL;
git_revwalk *walker = NULL;

std::vector<Wrappers::Commit> hit_commits{};

char inputs[INPUTS_SIZE];
char started = 0;

static SDL_Texture *start_hint = NULL;

static BasePattern *pattern = nullptr;
static PatternFactory pattern_factory{};

static git_commit *get_commit_from_string(git_repository *repo, const char *s) {
    int error;
    git_object *obj;
    error = git_revparse_single(&obj, repo, s);
    if (error < 0)
        libgit_panic(error);

    git_object *peeled;
    error = git_object_peel(&peeled, obj, GIT_OBJECT_COMMIT);
    if (error < 0)
        libgit_panic(error);

    git_object_free(obj);

    return (git_commit *)peeled;
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
    (void)appstate;

    app_arguments args = parse_args(argc, argv);

    SDL_SetAppMetadata("GitCommitsBulletHell",
                       GitCommitsBulletHell_VERSION,
                       "io.github.p1k0chu.gitcommitsbullethell");

    if (!SDL_Init(SDL_INIT_VIDEO))
        sdl_die("Couldn't init sdl video");

    if (!SDL_CreateWindowAndRenderer("Commits Bullet Hell",
                                     WINDOW_WIDTH,
                                     WINDOW_HEIGHT,
                                     SDL_WINDOW_FULLSCREEN | SDL_WINDOW_RESIZABLE,
                                     &window,
                                     &renderer))
        sdl_die("Couldn't create window/renderer");

    if (!TTF_Init())
        sdl_die("Couldn't initialize SDL3_ttf");

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
    if (error < 0)
        libgit_panic(error);

    error = git_repository_open_ext(&repo, args.path, 0, NULL);
    if (error < 0)
        libgit_panic(error);

    error = git_revwalk_new(&walker, repo);
    if (error < 0)
        libgit_panic(error);

    if (args.rev_push == NULL) {
        error = git_revwalk_push_head(walker);
    } else {
        git_commit *commit = get_commit_from_string(repo, args.rev_push);
        error = git_revwalk_push(walker, git_commit_id(commit));
        if (error < 0)
            libgit_panic(error);

        git_commit_free(commit);
    }

    if (error < 0)
        libgit_panic(error);

    if (args.rev_hide != NULL) {
        git_commit *commit = get_commit_from_string(repo, args.rev_hide);
        error = git_revwalk_hide(walker, git_commit_id(commit));
        if (error < 0)
            libgit_panic(error);
        git_commit_free(commit);
    }

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
        case SDLK_Q:
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
    last_ms = ms;

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
        // SDL_GetTextureSize(player_texture, &dst.w, &dst.h);
        if (pattern_factory.has_next() &&
            (pattern == nullptr ||
             (pattern->enemies.empty() && pattern->should_start_next_pattern()))) {
            delete pattern;
            if (walker != nullptr) {
                pattern = pattern_factory.create_next();
                assert(pattern != nullptr);
            } else {
                pattern = nullptr;
            }
        }
        if (pattern != nullptr) {
            pattern->tick(dt);
        }
    } else {
        SDL_GetTextureSize(start_hint, &dst.w, &dst.h);
        dst.x = (WINDOW_WIDTH - dst.w) / 2;
        dst.y = 0;
        SDL_RenderTexture(renderer, start_hint, NULL, &dst);
    }

    if (pattern != nullptr) {
        for (auto &enemy : pattern->enemies) {
            enemy.render(renderer);
        }
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

    if (!hit_commits.empty()) {
        std::cout << "Hit " << hit_commits.size() << " commits:" << std::endl;

        auto commit = hit_commits.begin();
        while (commit != hit_commits.end()) {
            std::cout << commit->sha_str() << ": " << commit->summary() << std::endl;
            ++commit;
        }
    }

    if (font) {
        TTF_CloseFont(font);
    }
    TTF_Quit();
}

