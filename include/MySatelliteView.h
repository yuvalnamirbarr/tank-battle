#pragma once

#include "SatelliteView.h"
#include "Entity.h"
#include "Board.h"
#include "Tank.h"
#include "Shell.h"
#include "Cell.h"
#include <map>
#include <vector>
#include <utility>

/**
 * @brief A concrete implementation of SatelliteView providing a read-only view of the board.
 * 
 * Allows a tank to query the state of any cell on the board, including terrain, other tanks,
 * and shells. Used by the algorithm to get a global picture of the game.
 */
class MySatelliteView : public SatelliteView {
private:
    const Board& board;
    const std::map<std::pair<int, int>, std::vector<Entity*>>& positionMap;
    int rows;
    int cols;
    std::pair<int, int> requestingTankPosition;

public:
    /**
     * @brief Constructs a satellite view for the specified tank.
     * 
     * @param board The full game board.
     * @param posMap The mapping of positions to entities (tanks, shells).
     * @param requestingTankPos The position of the tank requesting the view (used to identify itself).
     */
    MySatelliteView(const Board& board,
                    const std::map<std::pair<int, int>, std::vector<Entity*>>& posMap,
                    std::pair<int, int> requestingTankPos);

    /**
     * @brief Returns a character representing the object at the given board location.
     * 
     * @param x Row index (0-based)
     * @param y Column index (0-based)
     * @return Character representing the object: wall, mine, tank, shell, or empty.
     */
    char getObjectAt(size_t x, size_t y) const override;

    /**
     * @brief Default destructor.
     */
    ~MySatelliteView() override = default;
};
