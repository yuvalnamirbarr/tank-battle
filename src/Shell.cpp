#include "Shell.h"
#include <utility>

/**
 * @brief Constructs a shell with initial position and direction.
 */
Shell::Shell(int x, int y, Direction dir)
    : Entity(x, y, dir) {}

/**
 * @brief Returns the type of the entity ("Shell").
 */
std::string Shell::getType() const {
    return "Shell";
}
