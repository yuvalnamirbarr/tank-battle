#pragma once
#include "MyPlayer.h"
#include "BasicTankAlgorithm.h"
#include "MyBattleInfo.h"
#include <iostream>

/**
 * @brief A player that uses a basic defensive strategy for its tank(s).
 * 
 * This class inherits from MyPlayer and creates a basic tank algorithm
 * that prioritizes safety using MyBattleInfo.
 */
class DefensivePlayer : public MyPlayer {
public:
    /**
     * @brief Constructs a DefensivePlayer instance with game configuration parameters.
     * 
     * @param player_index The player's index (1 or 2)
     * @param x Number of columns on the board
     * @param y Number of rows on the board
     * @param max_steps Maximum number of steps per turn
     * @param num_shells Initial number of shells per tank
     */
    DefensivePlayer(int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells);

    /**
     * @brief Updates the tank's algorithm with a new satellite view of the board.
     * 
     * Converts the satellite view into a MyBattleInfo object and passes it to the tank.
     * 
     * @param tank The tank algorithm to update.
     * @param satellite_view The satellite view of the board.
     */
    void updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& satellite_view) override;
};