#include "Direction.h"

/**
 * @brief Maps each Direction to its corresponding (dx, dy) step.
 */
std::pair<int, int> getDelta(Direction dir) {
    switch (dir) {
        case Direction::U:  return {0, -1}; 
        case Direction::UR: return {1, -1};
        case Direction::R:  return {1, 0};
        case Direction::DR: return {1, 1};
        case Direction::D:  return {0, 1};
        case Direction::DL: return {-1, 1};
        case Direction::L:  return {-1, 0};
        case Direction::UL: return {-1, -1};
        default:            return {0, 0}; // Defensive default
    }
}


/**
 * @brief Checks if dir1 and dir2 point in opposite directions.
 */
bool areOpposite(Direction dir1, Direction dir2) {
    auto [dx1, dy1] = getDelta(dir1);
    auto [dx2, dy2] = getDelta(dir2);
    return (dx1 == -dx2) && (dy1 == -dy2);
}

/**
 * @brief Returns the direction opposite to the given one.
 */
Direction oppositeDirection(Direction dir) {
    return static_cast<Direction>((static_cast<int>(dir) + 4) % 8);
}

/**
 * @brief Rotates the direction 1/8 turn to the left.
 */
Direction turnLeft(Direction dir) {
    return static_cast<Direction>((static_cast<int>(dir) + 7) % 8);
}

/**
 * @brief Rotates the direction 1/8 turn to the right.
 */
Direction turnRight(Direction dir) {
    return static_cast<Direction>((static_cast<int>(dir) + 1) % 8);
}

/**
 * @brief Rotates the direction 1/4 turn to the left (2 steps).
 */
Direction turnLeftQuarter(Direction dir) {
    return static_cast<Direction>((static_cast<int>(dir) + 6) % 8);
}

/**
 * @brief Rotates the direction 1/4 turn to the right (2 steps).
 */
Direction turnRightQuarter(Direction dir) {
    return static_cast<Direction>((static_cast<int>(dir) + 2) % 8);
}