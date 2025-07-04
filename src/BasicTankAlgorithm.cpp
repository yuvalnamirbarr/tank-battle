#include "BasicTankAlgorithm.h"
#include "TankAlgorithm.h"
#include <iostream>

/**
 * @brief Constructor that initializes the tank algorithm with player and tank indices.
 */
BasicTankAlgorithm::BasicTankAlgorithm(int playerIndex, int tankIndex)
    : MyTankAlgorithm(playerIndex, tankIndex) {}


/**
 * @brief Updates internal state using satellite view information.
 *
 * Updates tank position, known shell locations, and number of shells.
 * Delegates grid update to base class.
 */

void BasicTankAlgorithm::updateBattleInfo(BattleInfo& info) {
    auto myinfo = static_cast<MyBattleInfo&>(info);
    myPosition = myinfo.getMyPosition();
    shellsPositions = myinfo.getShellsPositions();
    if (numShells == -1){
        numShells = myinfo.getNumShells();
    }
    updateGrid(info);
}

/**
 * @brief Main method: delegates to basicAlgorithm and rotation if needed.
 */
ActionRequest BasicTankAlgorithm::getAction() {
    UpdateShootDelay();
    if (isEmpty()) { // don't have any information about the grid, have to get it
        return shouldGetBattleInfo();
    }
    
    auto threatPlaces = getThreatsAroundMe();
    auto action = getThreatningNextAction(threatPlaces); //  check if i am threatend

    // If there is no threat, get battle info (if i haven't gotten it in a while)
    if (action == ActionRequest::DoNothing) {
        if (turnsSinceLastUpdate >= 4) {
            return shouldGetBattleInfo();
        }

        else {
            // else - try to rotate or still get battle info
            action = getScaryNextAction(threatPlaces);
            if (action == ActionRequest::DoNothing){
                if(turnsSinceLastUpdate >= 2){
                    return shouldGetBattleInfo();
                }
                else {
                    action = ActionRequest::RotateLeft90;
                }
            }
        }
    }
    turnsSinceLastUpdate++;
    updatePostAction(action);
    return action;
}

/**
 * @brief When no threat is imminent, attempts a proactive action.
 *
 * First tries rotating toward a free cell. If not possible, tries to shoot.
 */

ActionRequest BasicTankAlgorithm::getScaryNextAction(std::set<std::pair<int, int>> threatPlaces) {
    auto act = ActionRequest::DoNothing;

    // check maybe i can turn to a safe place
    act = CheckIfINeedToShootX(6);

    // If we chose to stay, maybe we can shoot instead
    if (act == ActionRequest::DoNothing) {
        act = rotateBasedOnEnemyStrategy(threatPlaces);
    }

    return act;
}

/**
 * @brief Makes a strategic rotation decision based on enemy proximity.
 *
 * If enemy is visible in a straight line (up to radius 3):
 *   - If I can shoot — rotate toward it.
 *   - If not facing it — try to escape or rotate to free cell.
 *   - If already facing it — dont move to shoot next round.
 * If no visible enemy — rotate to a safe adjacent cell.
 */
ActionRequest BasicTankAlgorithm::rotateBasedOnEnemyStrategy(std::set<std::pair<int, int>> threatPlaces) {
    auto maybeTarget = findVisibleEnemyDirectionInRadius(3);
    if (maybeTarget.first == 2) {
        return rotateToFreeCell(threatPlaces);
    }
    auto [Enemydx, Enemydy] = maybeTarget;
    auto [dx, dy] = getDelta(myDirection);
    if (Enemydx != dx || Enemydy != dy) { // not in my direction - i can escape!
        auto act = MoveTankFromDanger(threatPlaces);
        if (act == ActionRequest::DoNothing){
            if (CheckIfCanShoot()) {
                return rotateTowardTarget(Enemydx, Enemydy);
            }
            else {
                return rotateToFreeCell(threatPlaces);
            }
        }
    }
    return ActionRequest::DoNothing;
}

/**
 * @brief Scans in all 8 directions up to a given radius to find visible enemy tank.
 *
 * Returns the (dx, dy) direction to the enemy if seen with no walls in the way.
 * If not found, returns (2, 2) as a sentinel value.
 */

std::pair<int, int> BasicTankAlgorithm::findVisibleEnemyDirectionInRadius(int radius) {
    for (int dirInt = 0; dirInt < 8; ++dirInt) {
        Direction dir = static_cast<Direction>(dirInt);
        auto [dx, dy] = getDelta(dir);
        int x = myPosition.first;
        int y = myPosition.second;

        for (int step = 1; step <= radius; ++step) {
            x += dx;
            y += dy;
            wrapPosition(x, y);
            auto obj = grid[y][x];

            if (obj == ObjectType::Wall || obj == ObjectType::AllyTank) {
                break;
            }

            if (obj == ObjectType::EnemyTank) {
                return {dx, dy};
            }
        }
    }
    return {2, 2}; 
}

/**
 * @brief Decides the minimal rotation needed to face a given direction (dx, dy).
 *
 * Returns the appropriate ActionRequest to rotate 45° or 90°.
 * Includes handling for multi-step rotations (e.g. facing backward = 180°).
 */
ActionRequest BasicTankAlgorithm::rotateTowardTarget(int dx, int dy) {
    Direction targetDir;
    for (int i = 0; i < 8; ++i) {
        Direction d = static_cast<Direction>(i);
        auto delta = getDelta(d);
        if (delta == std::make_pair(dx, dy)) {
            targetDir = d;
            break;
        }
    }
    if (myDirection == targetDir) {
        return ActionRequest::DoNothing;
    }
    int diff = (static_cast<int>(targetDir) - static_cast<int>(myDirection) + 8) % 8;
    switch (diff) {
        case 1: return ActionRequest::RotateRight45;
        case 2: return ActionRequest::RotateRight90;
        case 3: return ActionRequest::RotateLeft90; // double turn
        case 4: return ActionRequest::RotateRight90; // double turn
        case 5: return ActionRequest::RotateRight90; // double turn
        case 6: return ActionRequest::RotateLeft90;
        case 7: return ActionRequest::RotateLeft45;
        default: return ActionRequest::DoNothing;
    }
}


/**
 * @brief Rotates the tank toward a free adjacent cell, if any exist.
 * 
 * Tries small rotations first (1/8), then larger ones (1/4).
 */
ActionRequest BasicTankAlgorithm::rotateToFreeCell(std::set<std::pair<int, int>> threatPlaces) {
    // Try all four rotation options
    std::vector<std::pair<ActionRequest, Direction>> rotations = {
        {ActionRequest::RotateLeft45, static_cast<Direction>((static_cast<int>(myDirection) + 7) % 8)},
        {ActionRequest::RotateRight45, static_cast<Direction>((static_cast<int>(myDirection) + 1) % 8)},
        {ActionRequest::RotateLeft90, static_cast<Direction>((static_cast<int>(myDirection) + 6) % 8)},
        {ActionRequest::RotateRight90, static_cast<Direction>((static_cast<int>(myDirection) + 2) % 8)},
    };
    int x = myPosition.first;
    int y = myPosition.second;
    
    // Check which direction leads to a safe cell
    for (const auto& [action, newDir] : rotations) {
        auto delta = getDelta(newDir);
        int newX = x + delta.first;
        int newY = y + delta.second;
        if (threatPlaces.count({newX, newY}) == 0) {
            return action;
        }
    }

    return ActionRequest::DoNothing;
}

/**
 * @brief Requests updated battlefield info and resets turn counter.
 */
ActionRequest BasicTankAlgorithm::shouldGetBattleInfo() {
    turnsSinceLastUpdate = 1;
    updatePostAction(ActionRequest::GetBattleInfo);
    return ActionRequest::GetBattleInfo;
}

/**
 * @brief Checks if the internal grid is completely empty (i.e., not yet updated).
 *
 * @return true if all grid cells are ObjectType::Empty, false otherwise.
 */
bool BasicTankAlgorithm::isEmpty() const {
    for (const auto& row : grid) {
        for (ObjectType obj : row) {
            if (obj != ObjectType::Empty) {
                return false;
            }
        }
    }
    return true;
}
