#include "Tank.h"

/**
 * @brief Constructs a new tank with given position, direction, and player ownership.
 */
Tank::Tank(Direction dir, int x, int y, int player, int ammo, int id)
    : Entity(x, y, dir),
      player(player),
      ammo(ammo),
      tankId(id),
      isAlive(true),
      shootCooldown(0),
      backwardDelay(-2)
      {}

/**
 * @brief Sets the tank's direction.
 */
void Tank::setDirection(Direction dir) {
    direction = dir;
}

/**
 * @brief Returns the tank's current ammo count.
 */
int Tank::getAmmo() const {
    return ammo;
}

/**
 * @brief Returns the player ID who owns this tank.
 */
int Tank::getPlayer() const {
    return player;
}

/**
 * @brief Returns the type of entity ("Tank").
 */
std::string Tank::getType() const {
    return "Tank";
}

/**
 * @brief Moves the tank forward in its current direction, if the next cell is not a wall.
 */
void Tank::moveForward(const Board& board) {
    auto [newX, newY] = getNextPosition(board);
    const Cell& cell = board.getCell(newX, newY);
    if (cell.getTerrain() != TerrainType::Wall) {
        setPosition(newX, newY);
    }
}

/**
 * @brief Rotates the tank by a number of steps.
 * @param place Number of 1/8 turns to rotate (positive = right, negative = left)
 */
void Tank::rotate(int place) {
    int dirValue = (static_cast<int>(direction) + place) % 8;
    if (dirValue < 0) dirValue += 8;
    setDirection(static_cast<Direction>(dirValue));
}

/**
 * @brief Shoots a shell from the tank's current position and direction.
 * @return A new Shell object (allocated with new).
 */
Shell* Tank::shoot() {
    ammo--;
    startShootCooldown();
    return new Shell(x, y, direction); // Shell gets its own serial number internally
}

/**
 * @brief Starts a 4-step shooting cooldown.
 */
void Tank::startShootCooldown() {
    shootCooldown = 4;
}

/**
 * @brief Checks if the tank can currently shoot (has ammo and no cooldown).
 */
bool Tank::canShoot() const {
    return shootCooldown == 0 && ammo > 0;
}

/**
 * @brief Returns the current cooldown value.
 */
int Tank::getShootCooldown() const {
    return shootCooldown;
}

/**
 * @brief Decreases the cooldown value by 1, if it's greater than 0.
 */
void Tank::decreaseShootCooldown() {
    if (shootCooldown > 0) {
        --shootCooldown;
    }
}

/**
 * @brief Starts a 2-step delay for backward movement.
 */
void Tank::startBackwardDelay() {
    backwardDelay = 2;
}

/**
 * @brief Checks if tank is in normal movement state (not delayed for backward).
 */
bool Tank::notInBackwardDelay() const {
    return backwardDelay == -2;
}

/**
 * @brief Returns the current backward delay counter.
 */
int Tank::getBackwardDelay() const {
    return backwardDelay;
}

/**
 * @brief Sets the backward delay to a specific value.
 */
void Tank::setBackwardDelay(int x) {
    backwardDelay = x;
}

/**
 * @brief Checks if the tank is ready to perform its backward move.
 */
bool Tank::isReadyToMoveBackward() const {
    return backwardDelay == 0;
}

/**
 * @brief Decreases the backward delay by 1, if active.
 */
void Tank::decreaseBackwardDelay() {
    if (backwardDelay > -2) {
        --backwardDelay;
    }
}

/**
 * @brief Resets the backward delay state to normal (-2).
 */
void Tank::resetBackwardDelay() {
    backwardDelay = -2;
}

/**
 * @brief Moves the tank backward if the previous cell is not a wall.
 */
void Tank::moveBackward(const Board& board) {
    auto [newX, newY] = getPrevPosition(board);
    const Cell& cell = board.getCell(newX, newY);
    if (cell.getTerrain() != TerrainType::Wall) {
        setPosition(newX, newY);
    }
}

/**
 * @brief Marks the tank as dead (not alive).
 */
void Tank::markAsDead() {
    isAlive = false;
}

/**
 * @brief Checks if the tank is currently alive.
 * @return True if the tank is alive, false otherwise.
 */
bool Tank::getIsAlive() const {
    return isAlive;
}

