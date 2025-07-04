#pragma once
#include "PlayerFactory.h"
#include "AggressivePlayer.h"
#include "DefensivePlayer.h"
#include <memory>

/**
 * @brief A custom player factory that returns different player types per player index.
 * 
 * Player 1 gets a DefensivePlayer, Player 2 gets an AggressivePlayer.
 */
class MyPlayerFactory : public PlayerFactory {
public:
    /**
     * @brief Creates a player object (Defensive for player 1, Aggressive for player 2).
     * 
     * @param player_index Index of the player (1 or 2).
     * @param x Number of columns (board width).
     * @param y Number of rows (board height).
     * @param max_steps Maximum allowed steps.
     * @param num_shells Initial number of shells per tank.
     * @return A smart pointer to the created player.
     */
    std::unique_ptr<Player> create(int player_index, size_t x, size_t y,
                                   size_t max_steps, size_t num_shells) const override {
        if (player_index == 1) {
            auto player = std::make_unique<DefensivePlayer>(player_index, x, y, max_steps, num_shells);
            return player;
        } else {
            auto player = std::make_unique<AggressivePlayer>(player_index, x, y, max_steps, num_shells);
            return player;
        }
    }

    /**
     * @brief Virtual destructor.
     */
    ~MyPlayerFactory() override = default;
};
