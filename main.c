#include "buffer.h"
#include "font/tiny.h"
#include "version.h"

#define SDL_MAIN_USE_CALLBACKS 1

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#define die(s)                      \
    {                               \
        fprintf(stderr, "%s\n", s); \
        exit(1);                    \
    }

static Buffer buffer = {0};

static SDL_Window   *window   = NULL;
static SDL_Renderer *renderer = NULL;

static TTF_Font    *font = NULL;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
    (void)appstate;
    (void)argc;
    (void)argv;

    SDL_SetAppMetadata("GitCommitsBulletHell",
                       GitCommitsBulletHell_VERSION,
                       "io.github.p1k0chu.gitcommitsbullethell");

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't init sdl video: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("Commits Bullet Hell",
                                     1280,
                                     720,
                                     SDL_WINDOW_FULLSCREEN | SDL_WINDOW_RESIZABLE,
                                     &window,
                                     &renderer)) {
        SDL_Log("Couldn't create window/renderer: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!TTF_Init()) {
        SDL_Log("Couldn't initialize SDL3_ttf: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!(font = TTF_OpenFontIO(SDL_IOFromConstMem(tiny_ttf, tiny_ttf_len), true, 18.0f))) {
        SDL_Log("Couldn't open font: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

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

    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    (void)appstate;

    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xff);
    SDL_RenderClear(renderer);
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

