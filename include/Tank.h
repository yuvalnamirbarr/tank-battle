#pragma once

#include "Direction.h"
#include "Shell.h"
#include "Entity.h"
#include "Board.h"

#include <set>
#include <utility>
#include <iostream>

/**
 * @class Tank
 * @brief Represents a player's tank on the board.
 * 
 * A tank can move, rotate, shoot shells, and manage cooldowns and backward delays.
 */
class Tank : public Entity {
private:
    int player; ///< Player ID (1 or 2)
    int ammo; ///< Current ammunition
    int tankId; ///< The tanks serial number
    bool isAlive = true; ///< True if alive, False if dead
    int shootCooldown = 0; ///< Turns remaining until tank can shoot again
    int backwardDelay = -2; ///< Counter for backward movement delay

public:
    /**
     * @brief Constructs a tank at a given position and direction.
     * @param dir Initial facing direction
     * @param x Initial x coordinate
     * @param y Initial y coordinate
     * @param player Player ID owning the tank
     */
    Tank(Direction dir, int x, int y, int player, int ammo, int id);

    /**
     * @brief Sets the direction the tank is facing.
     */
    void setDirection(Direction dir);

    /**
     * @brief Moves the tank one step forward if no wall is ahead.
     */
    void moveForward(const Board& board);

    /**
     * @brief Rotates the tank by a given number of steps (positive = right, negative = left).
     * @param place Number of steps to rotate
     */
    void rotate(int place);

    /**
     * @brief Returns the player ID that owns this tank.
     */
    int getPlayer() const;

    /**
     * @brief Returns the type of entity ("Tank").
     */
    std::string getType() const override;

    /**
     * @brief Returns the current amount of ammo the tank has.
     */
    int getAmmo() const;

    /**
     * @brief Shoots a shell in the tank's facing direction, decreases ammo.
     * @return Pointer to the newly created Shell.
     */
    Shell* shoot();

    /**
     * @brief Starts a 4-turn cooldown after shooting.
     */
    void startShootCooldown();

    /**
     * @brief Checks if the tank can currently shoot (no cooldown, has ammo).
     */
    bool canShoot() const;

    /**
     * @brief Returns the current shooting cooldown.
     */
    int getShootCooldown() const;

    /**
     * @brief Decreases the shooting cooldown counter by 1 (if greater than 0).
     */
    void decreaseShootCooldown();

    /**
     * @brief Initiates backward delay (tank will move backward after delay).
     */
    void startBackwardDelay();

    /**
     * @brief Checks if the tank is not currently delayed for backward movement.
     * @return True if ready for normal moves.
     */
    bool notInBackwardDelay() const;

    /**
     * @brief Returns the current backward delay counter.
     */
    int getBackwardDelay() const;

    /**
     * @brief Checks if the tank is ready to perform the backward move.
     */
    bool isReadyToMoveBackward() const;

    /**
     * @brief Decreases the backward delay counter by 1 (if active).
     */
    void decreaseBackwardDelay();

    /**
     * @brief Resets the backward delay state to normal.
     */
    void resetBackwardDelay();

    /**
     * @brief Moves the tank one step backward if no wall is behind.
     */
    void moveBackward(const Board& board);

    /**
     * @brief Manually sets the backward delay counter.
     */
    void setBackwardDelay(int x);

    /**
     * @brief Marks the tank as dead (not alive).
     */
    void markAsDead();


    /**
     * @brief Checks if the tank is currently alive.
     * @return True if the tank is alive, false otherwise.
     */
    bool getIsAlive() const;
};
