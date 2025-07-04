#pragma once
#include <utility>
#include <string>
#include "Direction.h"
#include "Board.h"
#include "Direction.h"

/**
 * @class Entity
 * @brief Base class for all board entities (Tank, Shell).
 * 
 * Defines shared logic for handling position and direction.
 * Enables polymorphic access to type, position, and movement-related behavior.
 */
class Entity {
protected:
    int x; ///< X coordinate of the entity
    int y; ///< Y coordinate of the entity
    Direction direction; ///< Current facing direction of the entity

public:
    /**
     * @brief Construct a new Entity at a given position and direction.
     * 
     * @param x Initial x-coordinate of the entity.
     * @param y Initial y-coordinate of the entity.
     * @param dir Initial facing direction of the entity.
     */
    Entity(int x, int y, Direction dir)
        : x(x), y(y), direction(dir) {}

    /**
     * @brief Returns the type name of the entity (e.g., "Tank", "Shell").
     */
    virtual std::string getType() const = 0;

    /**
     * @brief Virtual destructor for proper cleanup of derived types.
     */
    virtual ~Entity() = default;

    // Rule of five: default copy/move constructors and assignment operators
    Entity(const Entity&) = default;
    Entity& operator=(const Entity&) = default;
    Entity(Entity&&) = default;
    Entity& operator=(Entity&&) = default;

    /**
     * @brief Gets the current direction the entity is facing.
     */
    Direction getDirection() const;

    /**
     * @brief Returns the current position of the entity on the board.
     */
    std::pair<int, int> getPosition() const;

    /**
     * @brief Updates the entity's position.
     * @param newX New X coordinate
     * @param newY New Y coordinate
     */
    void setPosition(int newX, int newY);

    /**
     * @brief Returns the next position the entity would move to (with wraparound).
     * @param board Reference to board (for wrapping)
     * @return Wrapped (x, y) position one step forward in current direction.
     */
    std::pair<int, int> getNextPosition(const Board& board) const;

    /**
     * @brief Returns the previous position the entity would move to (with wraparound).
     * @param board Reference to board (for wrapping)
     * @return Wrapped (x, y) position one step back in current direction.
     */
     std::pair<int, int> getPrevPosition(const Board& board) const;
};
