#include "AggressivePlayer.h"
#include <cstddef>
#include <iostream>
#include <climits>

/**
 * @brief Constructs an AggressivePlayer using MyPlayer constructor.
 */
AggressivePlayer::AggressivePlayer(int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells)
    : MyPlayer(player_index, x, y, max_steps, num_shells)
{}

/**
 * @brief Updates the tank with satellite data and assigns an enemy target.
 */
void AggressivePlayer::updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& satellite_view) {
    MyBattleInfo info(satellite_view, playerIndex, rows, cols, numShells);
    auto grid = info.getGrid();

    auto myPosition = findMyPosition(satellite_view);
    updateKnownEnemies(grid);

    auto closestEnemy = findClosestEnemyWithFallback(myPosition);

    info.setMyPosition(myPosition);
    info.setEnemyPosition(closestEnemy);

    tank.updateBattleInfo(info);
}

/**
 * @brief Updates internal known enemy set from the satellite grid.
 */
std::pair<int, int> AggressivePlayer::findMyPosition(SatelliteView& satellite_view) const {
    for (size_t y = 0; y < rows; ++y) {
        for (size_t x = 0; x < cols; ++x) {
            if (satellite_view.getObjectAt(x, y) == '%') {
                return {static_cast<int>(x), static_cast<int>(y)};
            }
        }
    }
    return {-1, -1};
}

/**
 * @brief Updates the set of known enemy tank positions based on the current grid.
 * Also removes assignments to enemies that are no longer visible.
 * @param grid The current view of the board, marking positions of enemy tanks.
 */
void AggressivePlayer::updateKnownEnemies(const std::vector<std::vector<ObjectType>>& grid) {
    std::set<std::pair<int, int>> updated;

    for (size_t y = 0; y < rows; ++y) {
        for (size_t x = 0; x < cols; ++x) {
            if (grid[y][x] == ObjectType::EnemyTank) {
                updated.insert({static_cast<int>(x), static_cast<int>(y)});
            }
        }
    }

    knownEnemies = updated;
    for (auto it = assignedEnemies.begin(); it != assignedEnemies.end();) {
        if (knownEnemies.count(*it) == 0) {
            it = assignedEnemies.erase(it);
        } else {
            ++it;
        }
    }
}

/**
 * @brief Finds the closest known enemy tank to the given position.
 * Prefers unassigned enemies, but falls back to any known enemy if none are free.
 * @param myPos The current position of this player's tank.
 * @return The position of the selected enemy tank.
 */

std::pair<int, int> AggressivePlayer::findClosestEnemyWithFallback(std::pair<int, int> myPos) {

    std::pair<int, int> bestFree = {-1, -1};
    std::pair<int, int> bestOverall = {-1, -1};
    int minFreeDist = INT_MAX;
    int minOverallDist = INT_MAX;

    for (const auto& enemyPos : knownEnemies) {
        int dx = myPos.first - enemyPos.first;
        int dy = myPos.second - enemyPos.second;
        int dist = dx * dx + dy * dy;

        if (assignedEnemies.count(enemyPos) == 0 && dist < minFreeDist) {
            minFreeDist = dist;
            bestFree = enemyPos;
        }
        if (dist < minOverallDist) {
            minOverallDist = dist;
            bestOverall = enemyPos;
        }
    }

    if (bestFree.first != -1) {
        assignedEnemies.insert(bestFree);
        return bestFree;
    }
    return bestOverall;
}
