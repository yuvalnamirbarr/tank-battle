#pragma once
#include <set>
#include <utility>
#include <string>

class Tank;  // forward declaration
class Shell; // forward declaration

/**
 * @enum TerrainType
 * @brief Represents the type of terrain in a cell.
 */
enum class TerrainType {
    Empty,  ///< No obstacle
    Wall,   ///< A wall that can be hit and destroyed
    Mine    ///< A mine that explodes on tank entry
};

/**
 * @class Cell
 * @brief Represents a single tile on the game board.
 *
 * Each cell stores its position, terrain type, and wall damage state.
 */
class Cell {
private:
    int x; ///< X coordinate of the cell
    int y; ///< Y coordinate of the cell
    TerrainType terrain; ///< Current terrain type of the cell
    int wallHits; ///< Number of hits taken by wall

public:
    /**
     * @brief Constructs an empty cell at a given (x, y) position.
     */
    Cell(int x, int y);

    /**
     * @brief Returns the coordinates of the cell.
     */
    std::pair<int, int> getPoint() const;

    /**
     * @brief Gets the current terrain type of the cell.
     */
    TerrainType getTerrain() const;

    /**
     * @brief Sets the terrain type of the cell.
     */
    void setTerrain(TerrainType t);

    /**
     * @brief Returns how many times a wall was hit (or -1 if not a wall).
     */
    int getWallHits() const;

    /**
     * @brief Increments the wall hit counter.
     */
    void incrementWallHits();

    /**
     * @brief Resets the wall (turns it into empty cell and clears hits).
     */
    void resetWall();

    /**
     * @brief Resets a mine (turns terrain into empty).
     */
    void resetMine();
};
