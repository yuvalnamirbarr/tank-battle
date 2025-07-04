#pragma once
#include "Player.h"
#include <cstddef>
#include <iostream>

/**
 * @brief Abstract base class for custom player logic.
 * 
 * Inherits from Player and provides a common interface for user-defined players.
 * Stores player index, board size, and other configuration parameters.
 */
class MyPlayer : public Player {
protected:
    int playerIndex; ///< Player index (1 or 2)
    size_t rows; ///< Number of rows on the board
    size_t cols; ///< Number of columns on the board
    size_t maxSteps; ///< Maximum number of steps per turn
    size_t numShells; ///< Initial number of shells per tank

public:

    /**
     * @brief Constructs a MyPlayer instance with game configuration parameters.
     * 
     * @param player_index The player's index (1 or 2)
     * @param x Number of columns on the board
     * @param y Number of rows on the board
     * @param max_steps Maximum number of steps per turn
     * @param num_shells Initial number of shells per tank
     */
    MyPlayer(int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells)
    : Player(player_index, x, y, max_steps, num_shells),
      playerIndex(player_index), rows(y), cols(x), maxSteps(max_steps), numShells(num_shells) 
      {}

    ~MyPlayer() override = default;

    /**
     * @brief Abstract method that updates a tank with new battle information.
     * 
     * Must be implemented by derived classes to inject satellite view data into a specific tank algorithm.
     * 
     * @param tank The tank algorithm to update.
     * @param satellite_view The satellite view object containing information about the board.
     */
    virtual void updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& satellite_view) override = 0;
};