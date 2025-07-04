#include "GameManager.h"

 /**
 * @brief Constructs the GameManager with factories for players and tank algorithms.
 */
GameManager::GameManager(std::unique_ptr<PlayerFactory> pf, std::unique_ptr<TankAlgorithmFactory> tf)
    : playerFactory(std::move(pf)), tankFactory(std::move(tf)), playerTankCount(2, 0) {}

/**
 * @brief Reads and parses a board file, initializes the board and players.
 * @param fileName The name of the input file.
 * @return true if successful, false if there was a parsing or logic error.
 */
bool GameManager::readBoard(const std::string& fileName) {
    resetGameState();

    // Open the input file
    std::ifstream file(fileName);
    if (!file) {
        std::cerr << "Failed to open board file: " << fileName << std::endl;
        return false;
    }

    initOutputLogFromInputFile(fileName);

    std::string line;
    std::getline(file, line); // Skip description
    if (!parseConfigLines(file)) return false;

    auto rawMap = readRawMap(file);
    normalizeRawMap(rawMap);
    board = std::make_unique<Board>(rows, cols);
    placeTerrain(rawMap);
    placeTanks(rawMap);
    if (!inputErrors.empty()) {
        writeInputErrorsToFile();
    }
    if (checkWinConditions()){
        return false;
    }

    return true;
}

/**
 * @brief Runs the main game loop.
 * Advances the game turn-by-turn until a win condition or maximum turn limit is reached.
 */
void GameManager::run(){
    bool won = false; 
    while (stepCounter < maxSteps) {
        stepCounter ++;
        processTurn();
        // printBoardStateToFile();
        if (checkWinConditions()) {
            won = true;
            break;
        }
    }
    if (!won){
        int tankPlayer1 = playerTankCount[0];
        int tankPlayer2 = playerTankCount[1];
        if (outputLog) {
            *outputLog << "Tie, reached max steps = " << maxSteps
                    << ", player 1 has " << tankPlayer1
                    << " tanks, player 2 has " << tankPlayer2 << " tanks\n";
        }
    }
}

/**
     * @brief Executes a full turn in the game.
     * Each player selects an action, actions are applied, and shells are moved.
*/
void GameManager::processTurn() {
    currentTurnActions.clear();
    rebuildPositionMap();

    size_t tankIndex = 0;
    for (const auto& [tankPtr, algoPtr] : tankPerAlgoVector) {
        Tank* t = tankPtr.get();
        TankAlgorithm* algot = algoPtr.get();

        if (!t->getIsAlive()) {
            currentTurnActions.push_back("killed");
            ++tankIndex;
            continue;
        }

        ActionRequest action;
        action = algot->getAction();
        currentTurnActions.push_back(toString(action));
        applyAction(action, *t, *algot, currentTurnActions.size() - 1);
        ++tankIndex;
    }
    
    moveShells();
    if (outputLog) {
        for (size_t i = 0; i < currentTurnActions.size(); ++i) {
            *outputLog << currentTurnActions[i];
            if (i + 1 < currentTurnActions.size())
                *outputLog << ", ";
        }
        *outputLog << std::endl;
    }
}


/**
 * @brief Applies a single action for a specific player.
 * Handles backward state if necessary.
 */
void GameManager::applyAction(ActionRequest action, Tank& tank, TankAlgorithm& algot, size_t i) {

    // Handle backward logic (may consume the entire turn)
    if (handleBackwardState(action, tank, i)) {
        return;
    }

    // TODO: Log the action to the output stream if available - HANDLE OUTPUT!!!
    
    // Apply the actual action (move, rotate, shoot, etc.)
    handleNormalAction(action, tank, algot, i);
    // Update shooting cooldown at the end of the turn
    tank.decreaseShootCooldown();
}

/**
 * @brief Moves all shells two steps with intermediate collision checks.
 */
void GameManager::moveShells() {
    rebuildPositionMap();
    HeadOnCollision();
    moveShellsOneStep();
    rebuildPositionMap();
    resolveCollisions();
    moveShellsOneStep();
    rebuildPositionMap();
    resolveCollisions();
}

/**
 * @brief Moves all shells one step forward according to their direction.
 */
void GameManager::moveShellsOneStep() {
    for (const auto& shellPtr : shells) {
        Shell* shell = shellPtr.get();
        auto [nextX, nextY] = shell->getNextPosition(*board);

        // Update shell's internal position
        shell->setPosition(nextX, nextY);
    }
}

/**
 * @brief Head-on shell collision (coming from opposite directions).
 */
void GameManager::HeadOnCollision() {
    std::set<Shell*> shellsToRemoves;
    std::set<Tank*> tanksToRemoves;

    for (const auto& [tankPtr, _] : tankPerAlgoVector) {
        Tank* tank = tankPtr.get();
        auto[x, y] = tank->getPosition();
        Direction dir = tank->getDirection();
        auto it = positionMap.find({x, y});

        if (tank->getIsAlive() && it != positionMap.end()) {
            for (Entity* ent : it->second) {
                if (Shell* diffShell = dynamic_cast<Shell*>(ent)) {
                    Direction oppDir = diffShell->getDirection();
                    if (areOpposite(dir, oppDir)) {
                        shellsToRemoves.insert(diffShell);
                        tanksToRemoves.insert(tank);
                        Cell& cell = board->getCell(x, y);
                        if (cell.getTerrain() == TerrainType::Mine) {
                            cell.resetMine();
                        }
                        if (cell.getTerrain() == TerrainType::Wall) {
                            cell.incrementWallHits();
                            if (cell.getWallHits() >= 2) {
                                cell.resetWall();
                            }
                        }
                    }
                }
            }
        }
    }

    // Remove all marked entities
    removeMarkedTanks(tanksToRemoves);
    removeMarkedShells(shellsToRemoves);
}


/**
 * @brief Detects and resolves collisions between entities on the board.
 * Handles walls, mines, tank-tank collisions, and shell interactions.
 */
void GameManager::resolveCollisions() {
    std::set<Shell*> shellsToRemove;
    std::set<Tank*> tanksToRemove;

    // Iterate over all board positions with entities
    for (auto& [pos, entities] : positionMap) {
        if (entities.empty()) continue;

        auto [localTanks, localShells] = separateEntities(entities);
        Cell& cell = board->getCell(pos.first, pos.second);

        bool removeTanks = shouldRemoveTanks(cell, localTanks, localShells);
        bool removeShells = shouldRemoveShells(cell, localTanks, localShells);

        // Mark entities for removal
        if (removeShells) {
            for (Shell* shell : localShells) {
                shellsToRemove.insert(shell);
            }
        }

        if (removeTanks) {
            for (Tank* tank : localTanks) {
                tanksToRemove.insert(tank);
            }
        }
    }

    // Remove all marked entities
    removeMarkedTanks(tanksToRemove);
    removeMarkedShells(shellsToRemove);
}


/**
 * @brief Rebuilds the map linking positions to their occupying entities.
 */
void GameManager::rebuildPositionMap() {
    positionMap.clear();

    for (const auto& [tankPtr, _] : tankPerAlgoVector) {
        Tank* tank = tankPtr.get();
        if (tank->getIsAlive()) {
            positionMap[tank->getPosition()].push_back(tank);
        }
    }

    for (const auto& shellPtr : shells) {
        Shell* shell = shellPtr.get();
        positionMap[shell->getPosition()].push_back(shell);
    }
}


/**
 * @brief Checks if the game has ended with a win, tie, or ongoing.
 */
bool GameManager::checkWinConditions(){
    determineWinner();
    switch (winner){
        case 0:
            if (outputLog) *outputLog << "Tie, both players have zero tanks\n";
            return true;
        case 1: {
            int tankPlayer = playerTankCount[0];
            if (outputLog)
                *outputLog << "Player 1 won with " << tankPlayer << " tanks still alive\n";
            return true;
        }
        case 2: {
            int tankPlayer = playerTankCount[1];
            if (outputLog)
                *outputLog << "Player 2 won with " << tankPlayer << " tanks still alive\n";
            return true;
        }
    }
    bool hasAmmo = false;
    for (const auto& [tankPtr, algoPtr] : tankPerAlgoVector) {
        if (tankPtr->getIsAlive() && tankPtr->getAmmo() > 0){
            hasAmmo = true;
            break;
        }
    }
    // Start draw countdown if both are out of ammo
    if (!hasAmmo) {
        if (drawCountdown == -1) {
            drawCountdown = wantedCountdown;
        } else {
            drawCountdown--;
            if (drawCountdown == 0) {
                if (outputLog) *outputLog << "Tie, both players have zero shells for" << wantedCountdown << "steps\n";
                return true;
            }
        }
    }
    return false;
}

// ----------------------------------------------------------------
//                RESOLVE COLLISIONS UTILITIES
// ----------------------------------------------------------------

/**
 * @brief Removes all shells marked for deletion.
 */
void GameManager::removeMarkedShells(const std::set<Shell*>& toRemove) {
    shells.erase(
        std::remove_if(shells.begin(), shells.end(),
            [&](const std::unique_ptr<Shell>& shellPtr) {
                return toRemove.count(shellPtr.get()) > 0;
            }),
        shells.end()
    );
}

/**
 * @brief Removes all tanks marked for deletion.
 */
void GameManager::removeMarkedTanks(const std::set<Tank*>& toRemove) {
    size_t globalIndex = 0;

    for (const auto& [tankPtr, _] : tankPerAlgoVector) {
        Tank* tank = tankPtr.get();
        int id = tank->getPlayer();

        if (toRemove.count(tank)){
            tank->markAsDead();
            playerTankCount[id - 1] -= 1;

            if (globalIndex < currentTurnActions.size()) { 
                currentTurnActions[globalIndex] += " (killed)";
            }
        }
        globalIndex++;
    }
}

/**
 * @brief Splits a list of generic entities into two separate vectors:
 *        one containing only tanks and the other containing only shells.
 */
std::pair<std::vector<Tank*>, std::vector<Shell*>> GameManager::separateEntities(const std::vector<Entity*>& entities) {
    std::vector<Tank*> tanks;
    std::vector<Shell*> shells;
    for (Entity* ent : entities) {
        if (ent->getType() == "Tank")
            tanks.push_back(static_cast<Tank*>(ent));
        else if (ent->getType() == "Shell")
            shells.push_back(static_cast<Shell*>(ent));
    }
    return {tanks, shells};
}

/**
 * @brief Determines if the tanks in the given cell should be removed,
 *        based on the presence of shells or other game rules.
 */
bool GameManager::shouldRemoveTanks(Cell& cell, const std::vector<Tank*>& tanks, const std::vector<Shell*>& shells) {
    bool remove = false;

    // Mine logic: destroy any tank stepping on a mine
    if (cell.getTerrain() == TerrainType::Mine && !tanks.empty()) {
        cell.resetMine();
        remove = true;
    }

    // Shell and tank at same location: both destroyed
    if (!shells.empty() && !tanks.empty()) {
        remove = true;
    }

    // Multiple tanks in the same cell: destroy them
    if (tanks.size() > 1) {
        remove = true;
    }

    return remove;
}

/**
 * @brief Determines if the shells in the given cell should be removed,
 *        typically based on collisions or interactions with tanks.
 */
bool GameManager::shouldRemoveShells(Cell& cell, const std::vector<Tank*>& tanks, const std::vector<Shell*>& shells) {
    bool remove = false;
    // Wall hit logic: 2 hits destroy the wall
    if (cell.getTerrain() == TerrainType::Wall) {
        cell.incrementWallHits();
        if (cell.getWallHits() >= 2)
            cell.resetWall();
        remove = true;
    }

    // Shell and tank at same location: both destroyed
    if (!shells.empty() && !tanks.empty()) {
        remove = true;
    }

    // Multiple shells in the same cell: destroy them
    if (shells.size() > 1) {
        remove = true;
    }

    // Head-on shell collision (coming from opposite directions)
    if (shells.size() == 1 && !remove){
        Shell* shell = shells[0];
        Direction dir = shell->getDirection();
        auto [prevX, prevY] = shell->getPrevPosition(*board);
        auto it = positionMap.find({prevX, prevY});
        if (it != positionMap.end()) {
            for (Entity* ent : it->second) {
                if (Shell* otherShell = dynamic_cast<Shell*>(ent)) {
                    Direction oppDir = otherShell->getDirection();
                    if (areOpposite(dir, oppDir)) {
                        remove = true;
                        break;
                    }
                }
            }
        }
    }
    return remove;
}


// ----------------------------------------------------------------
//                  APPLY ACTION UTILITIES
// ----------------------------------------------------------------

/**
 * @brief Handles backward movement logic for a tank.
 * Cancels illegal actions, manages delay counter, and executes movement.
 */
bool GameManager::handleBackwardState(ActionRequest action, Tank& tank, size_t i) {
    int backwardDelay = tank.getBackwardDelay();
    // Not in backward state – treat as normal
    if (tank.notInBackwardDelay()) {
        return false;
    }
    // Still waiting for backward delay to reach zero
    if (backwardDelay > 0) {
        tank.decreaseBackwardDelay();
        if (action == ActionRequest::MoveForward) {
            tank.resetBackwardDelay();
        }
        if (action != ActionRequest::DoNothing) {
            currentTurnActions[i] += " (ignored)";
        }
        return true;
    }
    // Delay reached zero – move backward automatically
    if (backwardDelay == 0) {
        tank.moveBackward(*board);
        tank.decreaseBackwardDelay();
        if (action != ActionRequest::DoNothing) {
            currentTurnActions[i] += " (ignored)";
        }
        return true;
    }
    // Backward state not started – wait for command
    if (backwardDelay == -1) {
        if (action == ActionRequest::MoveBackward) {
            tank.moveBackward(*board);
        } else {
            tank.decreaseBackwardDelay();
            return false;
        }
        return true;
    }
    // Any other unexpected state
    return false;
}


/**
 * @brief Handles a normal tank action (move, rotate, shoot, wait).
 */
void GameManager::handleNormalAction(ActionRequest action, Tank& tank, TankAlgorithm& algot, size_t i) {
    switch (action) {
        case ActionRequest::GetBattleInfo: {
            int playerIndex = tank.getPlayer();
            auto pos = tank.getPosition();
            Player& player = *players[playerIndex - 1];
            MySatelliteView view(*board, positionMap, pos);
            player.updateTankWithBattleInfo(algot, view);
            break;
        }
        case ActionRequest::MoveForward:
            tank.moveForward(*board);
            break;
        case ActionRequest::MoveBackward:
            tank.startBackwardDelay();
            break;
        case ActionRequest::RotateLeft45:
            tank.rotate(-1);
            break;
        case ActionRequest::RotateRight45:
            tank.rotate(1);
            break;
        case ActionRequest::RotateLeft90:
            tank.rotate(-2);
            break;
        case ActionRequest::RotateRight90:
            tank.rotate(2);
            break;
        case ActionRequest::Shoot:
            if (tank.canShoot()) {
                Shell* shell = tank.shoot();
                shells.push_back(std::unique_ptr<Shell>(shell));
            }
            else {
                currentTurnActions[i] += " (ignored)";
            }
            break;
        case ActionRequest::DoNothing:
            break;
    }
}

// ----------------------------------------------------------------
//                     READ BOARD UTILITIES
// ----------------------------------------------------------------

/**
 * @brief Parses a key=value line and returns the integer value.
 */
int parseIntFromLine(const std::string& line, const std::string& key) {
    auto pos = line.find('=');
    if (pos == std::string::npos)
        throw std::invalid_argument("Line missing '=': " + line);

    std::string lhs = line.substr(0, pos);
    std::string rhs = line.substr(pos + 1);

    // Remove spaces
    lhs.erase(remove_if(lhs.begin(), lhs.end(), ::isspace), lhs.end());
    rhs.erase(remove_if(rhs.begin(), rhs.end(), ::isspace), rhs.end());

    if (lhs != key)
        throw std::invalid_argument("Expected key '" + key + "', found '" + lhs + "'");

    try {
        return std::stoi(rhs);
    } catch (...) {
        throw std::invalid_argument("Value is not an integer: '" + rhs + "'");
    }
}

/**
 * @brief Resets the game state, clearing the board, players, shells, and counters.
 */
void GameManager::resetGameState() {
    shells.clear();
    positionMap.clear();
    stepCounter = 0;
    players.clear();
    players.resize(2);
    winner = -1;
    board.reset();
}

/**
 * @brief Parses MaxSteps, NumShells, Rows, and Cols from input config lines.
 */
bool GameManager::parseConfigLines(std::ifstream& file) {
    std::string line;
    try {
        if (!std::getline(file, line)) throw std::runtime_error("Missing line for MaxSteps");
        maxSteps = parseIntFromLine(line, "MaxSteps");

        if (!std::getline(file, line)) throw std::runtime_error("Missing line for NumShells");
        numShells = parseIntFromLine(line, "NumShells");

        if (!std::getline(file, line)) throw std::runtime_error("Missing line for Rows");
        rows = parseIntFromLine(line, "Rows");

        if (!std::getline(file, line)) throw std::runtime_error("Missing line for Cols");
        cols = parseIntFromLine(line, "Cols");
    }
    catch (const std::exception& ex) {
        std::cerr << "Unrecoverable board parsing error: " << ex.what() << std::endl;
        return false;
    }
    return true;
}

/**
 * @brief Normalizes the raw map to match declared board dimensions by padding/truncating.
 */
void GameManager::normalizeRawMap(std::vector<std::string>& rawMap) {
    if (rawMap.size() < rows) {
        for (size_t i = rawMap.size(); i < rows; ++i) {
            logRecoverableError("Missing row " + std::to_string(i) + ", adding empty row.");
            rawMap.emplace_back(cols, ' ');
        }
    }
    if (rawMap.size() > rows) {
        logRecoverableError("Too many rows in map, truncating after row " + std::to_string(rows - 1));
        rawMap.resize(rows);
    }

    for (size_t i = 0; i < rawMap.size(); ++i) {
        std::string& row = rawMap[i];
        if (row.size() < cols) {
            logRecoverableError("Row " + std::to_string(i) + " too short, padding with spaces.");
            row.resize(cols, ' ');
        } else if (row.size() > cols) {
            logRecoverableError("Row " + std::to_string(i) + " too long, truncating to " + std::to_string(cols) + " columns.");
            row = row.substr(0, cols);
        }
    }
}

/**
 * @brief Places terrain (walls and mines) on the board based on map characters.
 */
void GameManager::placeTerrain(const std::vector<std::string>& rawMap) {
    for (size_t y = 0; y < rows; ++y) {
        for (size_t x = 0; x < cols; ++x) {
            char c = rawMap[y][x];
            if (c == '#')
                board->getCell(x, y).setTerrain(TerrainType::Wall);
            else if (c == '@')
                board->getCell(x, y).setTerrain(TerrainType::Mine);
        }
    }
}

/**
 * @brief Places tanks on the board and initializes their algorithms and owners.
 */
void GameManager::placeTanks(const std::vector<std::string>& rawMap) {
    for (size_t y = 0; y < rows; ++y) {
        for (size_t x = 0; x < cols; ++x) {
            char c = rawMap[y][x];

            if (std::isdigit(c)) {
                int playerIndex = c - '0';

                if (playerIndex != 1 && playerIndex != 2) {
                    logRecoverableError("Unrecognized character '" + std::string(1, c) + "' at (" + std::to_string(x) + "," + std::to_string(y) + "), treating as space");
                    continue;
                }

                // create the player of he hasn't been created yet
                if (!players[playerIndex - 1]) {
                    players[playerIndex - 1] = playerFactory->create(playerIndex, cols, rows, maxSteps, numShells);
                }

                // adjust the direction of the tank
                Direction cannonDir = (playerIndex == 1) ? Direction::L : Direction::R;

                // create the tank
                auto tank = std::make_unique<Tank>(cannonDir, x, y, playerIndex, numShells, playerTankCount[playerIndex - 1]);
                Tank* tankPtr = tank.get(); 

                // create the tank algorithm
                std::unique_ptr<TankAlgorithm> algorithm = tankFactory->create(playerIndex, playerTankCount[playerIndex - 1]);

                // add to all relevent places
                tankPerAlgoVector.emplace_back(std::move(tank), std::move(algorithm));
                positionMap[{x, y}].push_back(tankPtr);

                // update the tank count
                ++playerTankCount[playerIndex - 1];
            }
            else if (c != ' ' && c != '#' && c != '@') {
                logRecoverableError("Invalid character '" + std::string(1, c) + "' at (" + std::to_string(x) + "," + std::to_string(y) + "), treating as space");
            }
        }
    }
}

/**
 * @brief Determines the game winner based on remaining players and tanks.
 */
void GameManager::determineWinner() {
    bool p0_dead = !players[0];
    bool p1_dead = !players[1];

    if (!p0_dead) {
        auto* Player0 = players[0].get();
        p0_dead = !Player0 || playerTankCount[0] == 0;
    }

    if (!p1_dead) {
        auto* Player1 = players[1].get();
        p1_dead = !Player1 || playerTankCount[1] == 0;
    }

    if (p0_dead && p1_dead) {
        winner = 0;
    } else if (p0_dead) {
        winner = 2;
    } else if (p1_dead) {
        winner = 1;
    }
}

/**
 * @brief Reads the raw map portion from the input file into a vector of strings.
 */
std::vector<std::string> GameManager::readRawMap(std::ifstream& file) {
    std::vector<std::string> rawMap;
    std::string line;
    while (std::getline(file, line)) {
        rawMap.push_back(line);
    }
    return rawMap;
}


// ----------------------------------------------------------------
//                   PRINTING/LOGS UTILITIES
// ----------------------------------------------------------------

/**
 * @brief Sets the output log stream for recording actions and events.
 */
void GameManager::setOutputLog(std::ostream& os) {
    outputLog = &os;
}

/**
 * @brief Records a recoverable error during board parsing.
 */
void GameManager::logRecoverableError(const std::string& msg) {
    inputErrors.push_back(msg);
}

/**
 * @brief Writes all recorded input errors into a text file.
 */
void GameManager::writeInputErrorsToFile() {
    std::ofstream out("input_errors.txt");
    for (const std::string& msg : inputErrors) {
        out << msg << "\n";
    }
}

 /**
 * @brief Initializes the output log based on the input file name.
 */
void GameManager::initOutputLogFromInputFile(const std::string& inputFileName) {
    // extract file name only from full path (e.g. "input/input_a.txt" -> "input_a.txt")
    size_t lastSlash = inputFileName.find_last_of("/\\");
    std::string fileNameOnly = (lastSlash != std::string::npos) ? 
        inputFileName.substr(lastSlash + 1) : inputFileName;

    
    // this->inputFileNameOnly = fileNameOnly;

    std::string outputFileName = "output_" + fileNameOnly;

    auto outputStream = std::make_unique<std::ofstream>(outputFileName);
    if (!outputStream->is_open()) {
        std::cerr << "Failed to open output file: " << outputFileName << std::endl;
        return;
    }

    outputLog = outputStream.get();
    ownedOutputLog = std::move(outputStream);
}

// /**
//  * @brief Writes the current state of the board to a file named "boards_<input_suffix>.txt".
//  * Each board snapshot includes walls, mines, tanks, and shells.
//  * A separator line ("===") is printed between each turn.
//  * For BONUS purposes
//  */
// void GameManager::printBoardStateToFile() {
//     std::string fileName = "boards_" + inputFileNameOnly;
//     std::ofstream file(fileName, std::ios::app); // append mode
//     if (!file.is_open()) return;
//     std::vector<std::vector<char>> display(rows, std::vector<char>(cols, '.'));
//     for (size_t row = 0; row < rows; ++row) {
//         for (size_t col = 0; col < cols; ++col) {
//             const Cell& cell = board->getCell(col, row);
//             switch (cell.getTerrain()) {
//                 case TerrainType::Wall: display[row][col] = '#'; break;
//                 case TerrainType::Mine: display[row][col] = '@'; break;
//                 default: break;
//             }
//         }
//     }
//     for (const auto& [tankPtr, _] : tankPerAlgoVector) {
//         const Tank* tank = tankPtr.get();
//         if (tank->getIsAlive()) {
//             auto [x, y] = tank->getPosition();
//             display[y][x] = (tank->getPlayer() == 1 ? '1' : '2');
//         }
//     }
//     for (const auto& shellPtr : shells) {
//         auto [x, y] = shellPtr->getPosition();
//         display[y][x] = '*';
//     }
//     for (const auto& row : display) {
//         for (char c : row) file << c;
//         file << '\n';
//     }
//     file << "===\n"; // separator between board states
// }