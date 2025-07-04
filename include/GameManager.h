#pragma once

#include <memory>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>

#include "Board.h"
#include "Tank.h"
#include "Direction.h"
#include "Entity.h"
#include "Player.h"
#include "PlayerFactory.h"
#include "TankAlgorithmFactory.h"
#include "ActionRequest.h"
#include "ActionRequestUtils.h"
#include "MySatelliteView.h"
#include "MyPlayer.h"
#include "Cell.h"

/**
 * @class GameManager
 * @brief Manages the simulation of the tank battle game.
 *        Responsible for initializing the board, running the game loop,
 *        executing tank actions, handling shell movement, and resolving collisions.
 */
class GameManager {
public:
    /**
     * @brief Constructs the GameManager with factories for players and tank algorithms.
     */
    GameManager(std::unique_ptr<PlayerFactory> pf, std::unique_ptr<TankAlgorithmFactory> tf);

    GameManager(const GameManager&) = delete;
    GameManager& operator=(const GameManager&) = delete;
    ~GameManager() = default;

    /**
     * @brief Reads and parses the board configuration file.
     * @param fileName The name of the input file.
     * @return true if board was loaded successfully, false otherwise.
     */
    bool readBoard(const std::string& fileName);

    /**
     * @brief Runs the main game loop, advancing turn by turn until termination.
     */
    void run();

    /**
     * @brief Sets the output log stream for recording actions and events.
     * @param os The output stream to log to.
     */
    void setOutputLog(std::ostream& os);

private:
    // -------------- Game state ---------------
    std::vector<std::unique_ptr<Shell>> shells;
    std::vector<std::unique_ptr<Player>> players;
    std::map<std::pair<int, int>, std::vector<Entity*>> positionMap;
    ///< Maps tanks to their controlling algorithms.
    std::vector<std::pair<std::unique_ptr<Tank>, std::unique_ptr<TankAlgorithm>>> tankPerAlgoVector;
    int stepCounter = 0;

    std::unique_ptr<PlayerFactory> playerFactory;
    std::unique_ptr<TankAlgorithmFactory> tankFactory;

    std::unique_ptr<Board> board;
    std::vector<int> playerTankCount;

    int drawCountdown = -1;   ///< Countdown used to detect repeated draw states.
    int wantedCountdown = 40; ///< Threshold turns before triggering draw scenario.
    int maxSteps = 0;
    int numShells = 0;
    size_t rows = 0;
    size_t cols = 0;
    int winner = -1; // 0 = tie, 1/2 = winner player

    // -------------- Output related -------------
    std::vector<std::string> inputErrors;
    std::unique_ptr<std::ofstream> ownedOutputLog;
    std::ostream* outputLog = nullptr;
    std::vector<std::string> currentTurnActions;
    // std::string inputFileNameOnly;


    // ----------------------------------------------------------------
    //                   GAME FLOW UTILITIES
    // ----------------------------------------------------------------

    /**
     * @brief Processes a single turn of the game.
     *        Executes all tanks' actions, moves shells, checks collisions,
     *        logs the state, and updates game termination conditions.
     */
    void processTurn();

    /**
     * @brief Applies a single action for a specific player.
     * Handles backward state if necessary.
     * @param action The action to apply.
     */
    void applyAction(ActionRequest action, Tank& tank, TankAlgorithm& algot, size_t i);

    /**
     * @brief Moves all shells two steps with intermediate collision checks.
     */
    void moveShells();

    /**
     * @brief Rebuilds the map linking positions to their occupying entities.
     */
    void rebuildPositionMap();

    /**
     * @brief Detects and resolves collisions between entities on the board.
     * Handles walls, mines, tank-tank collisions, and shell interactions.
     */
    void resolveCollisions();

    /**
     * @brief Checks if the game has ended with a win, tie, or ongoing.
     * @return true if the game has ended, false otherwise.
     */
    bool checkWinConditions();

    /**
     * @brief Determines the winner of the game based on remaining tanks.
     *        Updates the winner field (0 = tie, 1/2 = winning player).
     */
    void determineWinner();

    // ----------------------------------------------------------------
    //                   READBOARD UTILITIES
    // ----------------------------------------------------------------

    /**
     * @brief Resets all game state variables to their initial values.
     *        Clears tanks, shells, players, counters, and board for fresh initialization.
     */
    void resetGameState();

    /**
     * @brief Parses the configuration section of the input file.
     *        Extracts key game settings such as board size, shell count, and step limits.
     * @param file The input file stream.
     * @return true if all config lines were parsed successfully, false otherwise.
     */
    bool parseConfigLines(std::ifstream& file);

    /**
     * @brief Adjusts the raw map lines to have consistent width and formatting.
     *        Pads or truncates rows to fit the board's declared size.
     * @param rawMap Reference to the raw map strings to normalize.
     */
    void normalizeRawMap(std::vector<std::string>& rawMap);

    /**
     * @brief Places terrain elements (walls, mines, etc.) on the board based on the raw map.
     * @param rawMap The normalized raw map representation.
     */
    void placeTerrain(const std::vector<std::string>& rawMap);

    /**
     * @brief Places tanks on the board for both players based on their positions in the raw map.
     * @param rawMap The normalized raw map representation.
     */
    void placeTanks(const std::vector<std::string>& rawMap);

        /**
     * @brief Reads the raw map section from the input file.
     * @param file The input file stream.
     * @return A vector of strings representing the raw board layout.
     */
    std::vector<std::string> readRawMap(std::ifstream& file);

    // ----------------------------------------------------------------
    //                  APPLY ACTION UTILITIES
    // ----------------------------------------------------------------

    /**
     * @brief Handles a tank that is in backward movement state.
     * Manages the delay and movement logic related to moving backward.
     * @param action Player's attempted action.
     * @param tank The player's tank.
     * @return true if backward handling was performed, false otherwise.
     */
    bool handleBackwardState(ActionRequest action, Tank& tank, size_t i);

    /**
     * @brief Handles a normal tank action (move, rotate, shoot, wait).
     * @param action The action to perform.
     * @param tank The tank performing the action.
     */
    void handleNormalAction(ActionRequest action, Tank& tank, TankAlgorithm& algot, size_t i);

    // ----------------------------------------------------------------
    //                RESOLVE COLLISIONS UTILITIES
    // ----------------------------------------------------------------

    void HeadOnCollision();

    /**
     * @brief Removes all shells marked for deletion.
     * @param toRemove Set of shell pointers to remove.
     */
    void removeMarkedShells(const std::set<Shell*>& toRemove);

    /**
     * @brief Removes all tanks marked for deletion.
     * @param toRemove Set of tank pointers to remove.
     */
    void removeMarkedTanks(const std::set<Tank*>& toRemove);

    /**
     * @brief Splits a list of generic entities into two separate vectors:
     *        one containing only tanks and the other containing only shells.
     * @param entities A vector of pointers to Entity objects.
     * @return A pair of vectors: (tanks, shells).
     */
    std::pair<std::vector<Tank*>, std::vector<Shell*>> separateEntities(const std::vector<Entity*>& entities);

    /**
     * @brief Determines if the tanks in the given cell should be removed,
     *        based on the presence of shells or other game rules.
     * @param cell The cell being evaluated.
     * @param tanks List of tanks currently in the cell.
     * @param shells List of shells currently in the cell.
     * @return True if the tanks should be removed from the game.
     */
    bool shouldRemoveTanks(Cell& cell, const std::vector<Tank*>& tanks, const std::vector<Shell*>& shells);

    /**
     * @brief Determines if the shells in the given cell should be removed,
     *        typically based on collisions or interactions with tanks.
     * @param cell The cell being evaluated.
     * @param tanks List of tanks currently in the cell.
     * @param shells List of shells currently in the cell.
     * @return True if the shells should be removed from the game.
     */
    bool shouldRemoveShells(Cell& cell, const std::vector<Tank*>& tanks, const std::vector<Shell*>& shells);

    
    // ----------------------------------------------------------------
    //                   PRINTING/LOGS UTILITIES
    // ----------------------------------------------------------------
    
    /**
     * @brief Records a recoverable error during board parsing.
     * @param msg Error message.
     */
    void logRecoverableError(const std::string& msg);

    /**
     * @brief Writes all recorded input errors into a text file.
     */
    void writeInputErrorsToFile();

    /**
     * @brief Initializes the output log file stream using the provided input file name.
     *        The log will be written to a file with the same base name and .log extension.
     * @param inputFileName The name of the input file used to generate the log file name.
     */
    void initOutputLogFromInputFile(const std::string& inputFileName);

    // /**
    //  * @brief Writes the current board state into a file ("boards.txt").
    //  * Each board is separated by a line containing "===".
    //  */
    // void printBoardStateToFile();


    // ----------------------------------------------------------------
    //                   MOVE SHELLS UTILITIES
    // ----------------------------------------------------------------
        
    /**
     * @brief Moves all shells one step forward according to their direction.
     */
    void moveShellsOneStep();
};