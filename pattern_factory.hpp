#pragma once

#include "pattern.hpp"

class PatternFactory {
  public:
    Patterns::BasePattern *create_next();
    bool has_next() const;

    static Patterns::BasePattern *create_top_down();
    static Patterns::BasePattern *create_homing();
    static Patterns::BasePattern *create_left_and_right();
    static Patterns::BasePattern *create_scissors();

  private:
    unsigned int counter = 0;

    static constexpr Patterns::BasePattern *(*const factories[])() = {create_homing,
                                                                      create_left_and_right,
                                                                      create_top_down,
                                                                      create_scissors};
};
