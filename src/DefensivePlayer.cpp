#include "DefensivePlayer.h"

/**
 * @brief Constructs a DefensivePlayer by passing arguments to MyPlayer.
 * 
 * @param player_index The index of the player (1 or 2).
 * @param x Number of columns on the board (cols).
 * @param y Number of rows on the board (rows).
 * @param max_steps Maximum steps allowed per turn.
 * @param num_shells Initial number of shells per tank.
 */
DefensivePlayer::DefensivePlayer(int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells)
    : MyPlayer(player_index, x, y, max_steps, num_shells)
{}

/**
 * @brief Provides the tank with updated battle information using MyBattleInfo.
 * 
 * Converts the satellite view to MyBattleInfo and sends it to the tank algorithm.
 * 
 * @param tank The tank to be updated.
 * @param satellite_view The current satellite view of the game board.
 */
void DefensivePlayer::updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& satellite_view) {
    MyBattleInfo info(satellite_view, playerIndex, rows, cols, numShells);
    tank.updateBattleInfo(info);
} 
