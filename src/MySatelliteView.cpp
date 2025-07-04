#include "MySatelliteView.h"

/**
 * @brief Constructs a MySatelliteView object.
 * 
 * This class provides a read-only satellite-style view of the board,
 * including terrain, tanks, and shells for the requesting tank.
 * 
 * @param board Reference to the full game board.
 * @param posMap Map of current entity positions on the board.
 * @param requestingTankPos The position of the tank requesting the view.
 */
MySatelliteView::MySatelliteView(const Board& board,
                                 const std::map<std::pair<int, int>, std::vector<Entity*>>& posMap,
                                 std::pair<int, int> requestingTankPos)
    : board(board), positionMap(posMap),
      rows(board.getRows()), cols(board.getCols()),
      requestingTankPosition(requestingTankPos) {}


/**
 * @brief Returns a character representing the object at the given board coordinates.
 * 
 * The character can be:
 * - '#' for a wall
 * - '@' for a mine
 * - '*' for a shell
 * - '1' or '2' for a tank owned by player 1 or 2
 * - '%' for the requesting tank itself
 * - ' ' for empty space
 * - '&' for out-of-bounds access
 * 
 * @param x Column index (0-based) - represents X-axis (cols)
 * @param y Row index (0-based)    - represents Y-axis (rows)
 * @return A char representing the object at the specified coordinates
 */
char MySatelliteView::getObjectAt(size_t x, size_t y) const {
    if (y >= (size_t)rows || x >= (size_t)cols){
        return '&';
    }

    auto it = positionMap.find({(int)x, (int)y});
    if (it != positionMap.end()) {
        for (Entity* e : it->second) {
            if (e->getType() == "Tank") {
                if (e->getPosition() == requestingTankPosition){
                    return '%';
                }

                Tank* tank = dynamic_cast<Tank*>(e);
                if (tank) {
                    int owner = tank->getPlayer();
                    return owner == 1 ? '1' : '2';
                }
            }
            if (e->getType() == "Shell") {
                return '*';
            }
        }
    }

    const Cell& cell = board.getCell((int)x, (int)y);
    TerrainType terrain = cell.getTerrain();

    if (terrain == TerrainType::Wall)
        return '#';
    if (terrain == TerrainType::Mine)
        return '@';

    return ' ';
}
