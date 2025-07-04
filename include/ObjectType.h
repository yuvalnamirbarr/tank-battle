#pragma once

/**
 * @brief Enumeration representing different types of objects on the board.
 */
enum class ObjectType {
    Empty,     ///< No object (free cell)
    Wall,      ///< Indestructible or destructible wall
    Mine,      ///< Mine that destroys tanks
    Shell,     ///< Active shell (bullet)
    AllyTank,  ///< A tank that belongs to the current player
    EnemyTank  ///< A tank that belongs to the opposing player
};
