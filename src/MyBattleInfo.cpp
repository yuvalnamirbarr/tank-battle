#include "MyBattleInfo.h"
#include <iostream>

/**
 * @brief Constructs a BattleInfo object by analyzing the satellite view.
 *
 * This constructor scans the entire board via the provided SatelliteView and builds a grid of
 * ObjectType values. It classifies each cell based on the character returned by `getObjectAt`,
 * and fills in:
 * - The internal object grid
 * - The shell positions
 * - The position of the calling tank (`myPosition`)
 * 
 * @param view Reference to a SatelliteView for global board access
 * @param playerIndex The player index (1 or 2) of the calling tank
 * @param rows Number of rows on the board
 * @param cols Number of columns on the board
 * @param numShells The number of shells the tank has (copied into the info)
 */
MyBattleInfo::MyBattleInfo(const SatelliteView& view, int playerIndex, size_t rows, size_t cols, int numShells)
    : grid(rows, std::vector<ObjectType>(cols, ObjectType::Empty)),
    numShells(numShells) {
    for (size_t y = 0; y < rows; ++y) {
        for (size_t x = 0; x < cols; ++x) {
            char c = view.getObjectAt(x, y);
            ObjectType type = ObjectType::Empty;

            switch (c) {
                case '1':
                    type = (playerIndex == 1) ? ObjectType::AllyTank : ObjectType::EnemyTank;
                    break;
                case '2':
                    type = (playerIndex == 2) ? ObjectType::AllyTank : ObjectType::EnemyTank;
                    break;
                case '*':
                    type = ObjectType::Shell;
                    shellsPositions.insert({static_cast<int>(x), static_cast<int>(y)});
                    break;
                case '#':
                    type = ObjectType::Wall;
                    break;
                case '@':
                    type = ObjectType::Mine;
                    break;
                case '%':
                    myPosition = {static_cast<int>(x), static_cast<int>(y)};
                    type = ObjectType::AllyTank;
                    break;
            }
            grid[y][x] = type;
        }
    }
} 
