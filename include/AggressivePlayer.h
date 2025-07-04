#pragma once
#include "MyPlayer.h"
#include "MyBattleInfo.h"
#include "SatelliteView.h"
#include <set>
#include <utility>
#include <vector>

/**
 * @brief A player that aggressively assigns targets and prioritizes engaging enemies.
 * 
 * This class tracks known enemy tank positions and attempts to assign each tank
 * a unique enemy to pursue using satellite view information.
 */
class AggressivePlayer : public MyPlayer {
private:
    std::set<std::pair<int, int>> assignedEnemies; ///< Positions already assigned to other tanks
    std::set<std::pair<int, int>> knownEnemies;    ///< Enemy tank positions discovered via satellite view

    /**
     * @brief Updates known enemy positions based on the given grid.
     * 
     * @param grid A 2D grid of ObjectTypes representing satellite scan.
     */
    void updateKnownEnemies(const std::vector<std::vector<ObjectType>>& grid);

    /**
     * @brief Finds the calling tank's position on the board.
     * 
     * @param satellite_view Satellite view of the board.
     * @return The position (x, y) of the current tank.
     */
    std::pair<int, int> findMyPosition(SatelliteView& satellite_view) const;

    /**
     * @brief Selects the closest enemy tank not yet assigned to another tank.
     *        If no unassigned target is available, returns the closest one anyway.
     * 
     * @param myPos The tank's current position.
     * @return Target enemy position.
     */
    std::pair<int, int> findClosestEnemyWithFallback(std::pair<int, int> myPos);

public:
/**
     * @brief Constructs an AggressivePlayer.
     * 
     * @param player_index The index of the player (1 or 2).
     * @param x Number of columns on the board.
     * @param y Number of rows on the board.
     * @param max_steps Maximum number of steps per turn.
     * @param num_shells Initial number of shells per tank.
     */
    AggressivePlayer(int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells);

    /**
     * @brief Updates a tank with new battle information and assigns a target enemy.
     * 
     * Converts satellite data to MyBattleInfo, assigns an enemy,
     * and provides the tank with updated tactical info.
     * 
     * @param tank The tank algorithm to update.
     * @param satellite_view The satellite view of the board.
     */
    void updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& satellite_view) override;
};