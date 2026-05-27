#include "game.hpp"

#include "main.hpp"
#include "pattern.hpp"

#include <assert.h>
#include <iostream>
#include <sstream>
#include <string>

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
    return new AfterGame();
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

AfterGame::AfterGame() : commit_iter(hit_commits.begin()) {}

void AfterGame::run(unsigned long dt) {
    if (commit_iter != hit_commits.end() && (since_spawn += dt) >= 200L) {
        enemies.emplace_back(*(commit_iter++), Vec2d(), Vec2d(0.0, 1.0));
        since_spawn = 0L;
    }

    auto it = enemies.begin();
    while (it != enemies.end()) {
        it->move(Vec2d(0.0, (double)WINDOW_HEIGHT / 4 * (double)dt / 1000.0));
        if (it->get_y() > WINDOW_HEIGHT) {
            it = enemies.erase(it);
        } else {
            ++it;
        }
    }
}

void AfterGame::render(SDL_Renderer *renderer) const {
    GamePhase::render(renderer);
    for (auto &enemy : enemies) {
        enemy.render(renderer);
    }
}

GamePhase *AfterGame::get_next_phase() {
    return nullptr;
}

bool AfterGame::still_active() const {
    return !enemies.empty() || commit_iter != hit_commits.end();
}

AfterGameEntity::AfterGameEntity(Wrappers::Commit &commit, Vec2d where, Vec2d spawn_src) {
    std::ostringstream os{};
    os << commit.sha_str() << " " << commit.summary();
    std::string text = os.str();

    SDL_Surface *surface =
        TTF_RenderText_Blended(font, text.c_str(), 0, Patterns::SDL_COLOR_YELLOW);
    if (!surface)
        throw SDL_GetError();

    SDL_DestroyTexture(texture);
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);

    if (!texture)
        throw SDL_GetError();

    SDL_GetTextureSize(texture, &rect.w, &rect.h);

    rect.x = where.x - rect.w * spawn_src.x;
    rect.y = where.y - rect.h * spawn_src.y;
}

AfterGameEntity::~AfterGameEntity() {
    SDL_DestroyTexture(texture);
}

AfterGameEntity::AfterGameEntity(AfterGameEntity &&other) :
    rect(other.rect), texture(other.texture) {
    other.texture = nullptr;
}

AfterGameEntity &AfterGameEntity::operator=(AfterGameEntity &&other) {
    if (this != &other) {
        this->rect = other.rect;

        SDL_DestroyTexture(this->texture);
        this->texture = other.texture;
        other.texture = nullptr;
    }
    return *this;
}

void AfterGameEntity::move(Vec2d delta) {
    rect.x += delta.x;
    rect.y += delta.y;
}

void AfterGameEntity::render(SDL_Renderer *renderer) const {
    SDL_RenderTexture(renderer, texture, NULL, &rect);
}

double AfterGameEntity::get_y() const {
    return rect.y;
}
