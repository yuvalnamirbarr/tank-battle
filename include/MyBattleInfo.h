#pragma once
#include "BattleInfo.h"
#include "SatelliteView.h"
#include "ObjectType.h"
#include <vector>
#include <set>
#include <utility>

/**
 * @brief Custom implementation of BattleInfo for accessing global board data.
 * 
 * This class stores a full grid of detected objects (walls, mines, tanks, shells),
 * the position of the calling tank, enemy tank, and shell locations.
 * It is constructed using a SatelliteView and player-specific data.
 */
class MyBattleInfo : public BattleInfo {
private:
    std::vector<std::vector<ObjectType>> grid; ///< 2D grid representing object types
    std::pair<int, int> myPosition = {-1, -1}; ///< Position of this player's tank
    std::pair<int, int> enemyPosition = {-1, -1}; ///< Last known enemy position
    std::set<std::pair<int, int>> shellsPositions; ///< Set of shell coordinates
    int numShells; ///< Number of shells remaining


public:
    /**
     * @brief Constructs battle info from a satellite view.
     * 
     * Parses the entire visible board using `view.getObjectAt()` and fills internal structures.
     * Also stores the shell count and player-specific visibility.
     * 
     * @param view Global satellite view of the board
     * @param playerIndex Index of the player (1 or 2)
     * @param rows Number of rows on the board
     * @param cols Number of columns on the board
     * @param numShells Number of shells available to this tank
     */
    MyBattleInfo(const SatelliteView& view, int playerIndex, size_t rows, size_t cols, int numShells);
    ~MyBattleInfo() override = default;

    /** @brief Returns the current tank's position. */
    std::pair<int, int> getMyPosition() const { return myPosition; }

    /** @brief Returns the last known position of the enemy tank. */
    std::pair<int, int> getEnemyPosition() const { return enemyPosition; }

    /** @brief Returns the set of all known shell positions. */
    std::set<std::pair<int, int>> getShellsPositions() const { return shellsPositions; }

    /** @brief Returns the full 2D object grid parsed from the satellite view. */
    std::vector<std::vector<ObjectType>> getGrid() const { return grid; }

    /** @brief Returns the number of shells this tank has left. */
    int getNumShells() const { return numShells; }

    /** @brief Manually update the known enemy position. */
    void setEnemyPosition(const std::pair<int, int>& pos) { enemyPosition = pos; }

    /** @brief Manually update the current tank's position. */
    void setMyPosition(const std::pair<int, int>& pos) { myPosition = pos; }
};