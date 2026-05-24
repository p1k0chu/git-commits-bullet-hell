#include "pattern_factory.hpp"

#include "pattern.hpp"

#include <assert.h>

Patterns::BasePattern *PatternFactory::create_next() {
    unsigned int index = counter++ % std::size(PatternFactory::factories);
    return factories[index]();
}

bool PatternFactory::has_next() const {
    return counter < std::size(PatternFactory::factories);
}

Patterns::BasePattern *PatternFactory::create_top_down() {
    return new Patterns::TopDown();
}

Patterns::BasePattern *PatternFactory::create_homing() {
    return new Patterns::Homing(8);
}

Patterns::BasePattern *PatternFactory::create_left_and_right() {
    return new Patterns::LeftAndRight(4);
}

Patterns::BasePattern *PatternFactory::create_scissors() {
    return new Patterns::Scissors(4);
}
