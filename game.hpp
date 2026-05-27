#pragma once

#include "pattern.hpp"
#include "pattern_factory.hpp"

#include <SDL3/SDL_render.h>
#include <optional>

class Renderable {
  public:
    virtual void render(SDL_Renderer* renderer) const = 0;
};

class GamePhase : public Renderable {
  public:
    virtual void run(unsigned long dt);
    virtual void handle_inputs(char* inputs, unsigned long dt);
    virtual bool still_active() const;
    virtual void render(SDL_Renderer* renderer) const override;
    virtual GamePhase* get_next_phase() = 0;
};

class PreGame : public GamePhase {
  public:
    void render(SDL_Renderer* renderer) const override;
    void handle_inputs(char* inputs, unsigned long dt) override;
    bool still_active() const override;

    GamePhase* get_next_phase();

  private:
    bool started = false;
};

class Game : public GamePhase {
  public:
    Game();
    ~Game();

    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

    Game(Game&&) = delete;
    Game& operator=(Game&&) = delete;

    void run(unsigned long dt) override;
    void render(SDL_Renderer* renderer) const override;
    GamePhase* get_next_phase() override;

    bool still_active() const override;

  private:
    Patterns::BasePattern* pattern = nullptr;
    PatternFactory pattern_factory;
};

class AfterGameEntity {
  public:
    AfterGameEntity(Wrappers::Commit& commit, Vec2d where, Vec2d spawn_src);
    ~AfterGameEntity();

    AfterGameEntity(const AfterGameEntity&) = delete;
    AfterGameEntity& operator=(const AfterGameEntity&) = delete;

    AfterGameEntity(AfterGameEntity&&);
    AfterGameEntity& operator=(AfterGameEntity&&);

    void move(Vec2d delta);

    void render(SDL_Renderer* renderer) const;
    double get_y() const;

  private:
    SDL_FRect rect;
    SDL_Texture* texture;
};

class AfterGame : public GamePhase {
  public:
    AfterGame();

    void run(unsigned long dt) override;
    void render(SDL_Renderer* renderer) const override;
    GamePhase* get_next_phase() override;

    bool still_active() const override;

  private:
    std::vector<AfterGameEntity> enemies{};
    std::vector<Wrappers::Commit>::iterator commit_iter;
    long since_spawn = 0;
};
