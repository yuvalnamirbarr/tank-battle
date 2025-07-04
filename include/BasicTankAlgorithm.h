#pragma once

#include "MyTankAlgorithm.h"
#include "BattleInfo.h"
#include "MyBattleInfo.h"
#include "ObjectType.h"
#include "Direction.h"

/**
 * @class BasicTankAlgorithm
 * @brief A simple tank algorithm that avoids threats, rotates for vision, and occasionally shoots.
 *
 * This algorithm is designed to provide a balanced behavior — defensive at first, aggressive if needed.
 * It inherits from MyTankAlgorithm and leverages grid knowledge to choose safe and meaningful actions.
 */
class BasicTankAlgorithm : public MyTankAlgorithm {
private:
    /**
     * @brief Determines an action to take when no immediate danger exists.
     * Tries rotating or shooting in the direction of potential threats.
     * @param threatPlaces Set of grid locations considered dangerous.
     * @return An ActionRequest (rotation, shoot, or DoNothing).
     */
    ActionRequest getScaryNextAction(std::set<std::pair<int, int>> threatPlaces);

    /**
    * @brief Makes a strategic rotation decision based on enemy proximity.
    *
    * If enemy is visible in a straight line (up to radius 3):
    *   - If I can shoot — rotate toward it.
    *   - If not facing it — try to escape or rotate to free cell.
    *   - If already facing it — rotate to explore surroundings.
    * If no visible enemy — rotate to a safe adjacent cell.
    */
    ActionRequest rotateBasedOnEnemyStrategy(std::set<std::pair<int, int>> threatPlaces);

    /**
    * @brief Decides the minimal rotation needed to face a given direction (dx, dy).
    *
    * Returns the appropriate ActionRequest to rotate 45° or 90°.
    * Includes handling for multi-step rotations (e.g. facing backward = 180°).
    */
    ActionRequest rotateTowardTarget(int dx, int dy);

    /**
    * @brief Scans in all 8 directions up to a given radius to find visible enemy tank.
    *
    * Returns the (dx, dy) direction to the enemy if seen with no walls in the way.
    * If not found, returns (2, 2) as a sentinel value.
    */
    std::pair<int, int> findVisibleEnemyDirectionInRadius(int radius);

    /**
     * @brief Tries rotating the tank toward a nearby non-threatened cell.
     * Prioritizes small rotations before larger ones.
     * @param threatPlaces Set of positions considered dangerous.
     * @return The best rotation action to take, or DoNothing if none.
     */
    ActionRequest rotateToFreeCell(std::set<std::pair<int, int>> threatPlaces);

    /**
     * @brief Asks the game for updated battle info and resets update timer.
     * @return The ActionRequest::GetBattleInfo action.
     */
    ActionRequest shouldGetBattleInfo();

    /**
     * @brief Checks if the internal grid is empty (not yet updated).
     * @return true if all grid cells are ObjectType::Empty, false otherwise.
     */
    bool isEmpty() const;

public:
    /**
     * @brief Constructs the tank algorithm for a given player and tank.
     * @param playerIndex The index of the player (1 or 2).
     * @param tankIndex The index of the tank controlled by this algorithm.
     */
    BasicTankAlgorithm(int playerIndex, int tankIndex);

    /**
     * @brief Updates internal knowledge about the game state using satellite view info.
     * @param info A reference to the latest battle info, including shell positions and grid content.
     */
    void updateBattleInfo(BattleInfo& info) override; // not has to be implemented in this class, can be in parent class
    
    /**
     * @brief Returns the next action the tank should take.
     * Decides whether to move, shoot, rotate, or get information.
     * @return The chosen ActionRequest.
     */
    ActionRequest getAction() override;
};
