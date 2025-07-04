#include "Cell.h"

/**
 * @brief Initializes a cell at given coordinates, defaulting to empty terrain.
 */
Cell::Cell(int x, int y)
    : x(x), y(y), terrain(TerrainType::Empty), wallHits(0) {}

/**
 * @brief Returns the (x, y) position of the cell.
 */
std::pair<int, int> Cell::getPoint() const {
    return {x, y};
}

/**
 * @brief Gets the current terrain type of the cell.
 * @return The terrain type of the cell.
 */
TerrainType Cell::getTerrain() const {
    return terrain;
}

/**
 * @brief Sets the terrain type of the cell.
 * @param t The new terrain type to set.
 */
void Cell::setTerrain(TerrainType t) {
    terrain = t;
}

/**
 * @brief Returns the number of hits the wall has taken, or -1 if not a wall.
 * @return The number of wall hits, or -1 if terrain is not a wall.
 */
int Cell::getWallHits() const {
    return wallHits;
}

/**
 * @brief Increments the wall hit counter.
 * Increases the count of hits taken by the wall.
 */
void Cell::incrementWallHits() {
    wallHits++;
}

/**
 * @brief Resets the wall — clears hit count and turns cell into empty terrain.
 */
void Cell::resetWall() {
    wallHits = 0;
    terrain = TerrainType::Empty;
}

/**
 * @brief Resets the mine — sets terrain back to empty.
 */
void Cell::resetMine() {
    terrain = TerrainType::Empty;
}
