#pragma once
#include "Player.h"
#include <memory>

class PlayerFactory {
public:
    virtual ~PlayerFactory() {}
    virtual std::unique_ptr<Player> create(int player_index, size_t x, size_t y,
                size_t max_steps, size_t num_shells ) const = 0;
};