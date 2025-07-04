#pragma once
#include <utility>

/**
 * @enum Direction
 * @brief Represents 8 possible directions on the board (cardinal + diagonal).
 * 
 * The directions are arranged in clockwise order, starting from Up (U).
 */
enum class Direction {
    U,   ///< Up
    UR,  ///< Up-Right
    R,   ///< Right
    DR,  ///< Down-Right
    D,   ///< Down
    DL,  ///< Down-Left
    L,   ///< Left
    UL   ///< Up-Left
};

/**
 * @brief Returns the (dx, dy) change corresponding to the given direction.
 */
std::pair<int, int> getDelta(Direction dir);

/**
 * @brief Checks if two directions are exact opposites.
 */
bool areOpposite(Direction dir1, Direction dir2);

/**
 * @brief Returns the opposite direction (180-degree turn).
 */
Direction oppositeDirection(Direction dir);

/**
 * @brief Rotates the direction by 1/8 turn to the left (counter-clockwise).
 */
Direction turnLeft(Direction dir);

/**
 * @brief Rotates the direction by 1/8 turn to the right (clockwise).
 */
Direction turnRight(Direction dir);

/**
 * @brief Rotates the direction by 1/4 turn to the left.
 */
Direction turnLeftQuarter(Direction dir);

/**
 * @brief Rotates the direction by 1/4 turn to the right.
 */
Direction turnRightQuarter(Direction dir);
