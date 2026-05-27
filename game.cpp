#include "game.hpp"

#include "main.hpp"

#include <assert.h>

void Game::run(unsigned long dt) {
    GamePhase::run(dt);

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
}

Game::Game() {
    player.x = (double)WINDOW_WIDTH / 2;
    player.y = (double)WINDOW_HEIGHT / 2;
}

Game::~Game() {
    delete pattern;
}

void Game::render(SDL_Renderer *renderer) const {
    GamePhase::render(renderer);

    if (pattern != nullptr) {
        for (auto &enemy : pattern->enemies) {
            enemy.render(renderer);
        }
    }
}

GamePhase *Game::get_next_phase() {
    return nullptr;
}

bool Game::still_active() const {
    return walker != nullptr || (pattern != nullptr && !pattern->enemies.empty());
}

void GamePhase::run(unsigned long dt) {
    handle_inputs(inputs, dt);
}

void GamePhase::handle_inputs(char *inputs, unsigned long dt) {
    player.handle_inputs(inputs, dt);
}

bool GamePhase::still_active() const {
    return true;
}

void GamePhase::render(SDL_Renderer *renderer) const {
    player.render(renderer);
}

void PreGame::render(SDL_Renderer *renderer) const {
    GamePhase::render(renderer);

    SDL_FRect dst = {};
    SDL_GetTextureSize(start_hint, &dst.w, &dst.h);
    dst.x = (WINDOW_WIDTH - dst.w) / 2;
    dst.y = 0;
    SDL_RenderTexture(renderer, start_hint, NULL, &dst);
}

void PreGame::handle_inputs(char *inputs, unsigned long dt) {
    GamePhase::handle_inputs(inputs, dt);

    if (inputs[INPUT_Z]) {
        started = true;
    }
}

bool PreGame::still_active() const {
    return !started;
}

GamePhase *PreGame::get_next_phase() {
    return new Game();
}
