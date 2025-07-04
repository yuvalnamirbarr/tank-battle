#pragma once
#include "Direction.h"
#include "Entity.h"
#include "Board.h"
#include <string>

/**
 * @class Shell
 * @brief Represents a moving projectile fired by a tank.
 * 
 * Inherits from Entity and holds position and direction information.
 */
class Shell : public Entity {
public:
    /**
     * @brief Constructs a shell at the given position, moving in a given direction.
     * @param x Initial x coordinate
     * @param y Initial y coordinate
     * @param dir Direction the shell is moving
     */
    Shell(int x, int y, Direction dir);

    /**
     * @brief Returns the type of the entity ("Shell").
     */
    std::string getType() const override;
};
