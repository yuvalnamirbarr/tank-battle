#include "MyTankAlgorithm.h"
#include <iostream>

/**
 * @brief Constructs a new MyTankAlgorithm object.
 * @param playerIndex Index of the player (1 or 2).
 * @param tankIndex Index of the tank (currently always 0).
 */
MyTankAlgorithm::MyTankAlgorithm(int playerIndex, int tankIndex)
    : playerIndex(playerIndex),
      tankIndex(tankIndex),
      myPosition({-1, -1}),
      myDirection(playerIndex == 1 ? Direction::L : Direction::R),
      turnsSinceLastUpdate(0), 
      shootDelay(0),
      numShells(-1)
    
{}

/**
 * @brief Updates internal tank state after executing an action.
 * @param action The action that was just performed.
 */
void MyTankAlgorithm::updatePostAction(ActionRequest action) {
    switch (action) {
        case ActionRequest::MoveForward:
            myPosition = moveInDirectionD(myPosition.first, myPosition.second, 1, myDirection);
            break;
        case ActionRequest::MoveBackward:
            myPosition = moveInDirectionD(myPosition.first, myPosition.second, -1, myDirection);
            break;
        case ActionRequest::RotateLeft90:
            myDirection = turnLeftQuarter(myDirection);
            break;
        case ActionRequest::RotateRight90:
            myDirection = turnRightQuarter(myDirection);
            break;
        case ActionRequest::RotateLeft45:
            myDirection = turnLeft(myDirection);
            break;
        case ActionRequest::RotateRight45:
            myDirection = turnRight(myDirection);
            break;
        case ActionRequest::DoNothing:
            break;
        case ActionRequest::Shoot:
            ShootAShell();
            break;
        case ActionRequest::GetBattleInfo:
            break;
    }
}

/**
 * @brief Updates the internal board representation based on external battle info.
 * @param info The BattleInfo object containing updated board data.
 */
void MyTankAlgorithm::updateGrid(BattleInfo& info){
    auto myinfo = static_cast<MyBattleInfo&>(info);
    grid = myinfo.getGrid();
    rows = static_cast<int>(grid.size());
    cols = static_cast<int>(grid[0].size());
}

/**
 * @brief Moves one step in the given direction with wrapping.
 */
std::pair<int, int> MyTankAlgorithm::moveInDirectionD(int x, int y, int d, Direction dir) {
    auto [dx, dy] = getDelta(dir);
    int nx = x + dx * d;
    int ny = y + dy * d;
    wrapPosition(nx, ny);
    return {nx, ny};
}


/**
 * @brief Core logic deciding whether to move, shoot, or wait.
 * 
 * First checks if the tank is standing in a danger zone.
 * If yes, attempts to move to a safer location.
 * Otherwise, considers shooting if an enemy is in sight.
 */
ActionRequest MyTankAlgorithm::getThreatningNextAction(std::set<std::pair<int, int>> threatPlaces) {
    auto act = ActionRequest::DoNothing;

    int x = myPosition.first;
    int y = myPosition.second;

    // If we're currently standing in a danger zone, try to escape
    if (threatPlaces.count({x, y}) == 1) {
        act = MoveTankFromDanger(threatPlaces);
    }

    // If we chose to stay, maybe we can shoot instead
    if (act == ActionRequest::DoNothing) {
        act = CheckIfINeedToShootX(2);
    }
    return act;
}

/**
 * @brief Attempts to move the tank away from danger, either forward or backward.
 */
ActionRequest MyTankAlgorithm::MoveTankFromDanger(std::set<std::pair<int, int>> threatPlaces) {
    Direction dir = myDirection;
    auto [dx, dy] = getDelta(dir);
    int x = myPosition.first;
    int y = myPosition.second;

    // Try moving forward if the next cell is safe
    int newX = x + dx;
    int newY = y + dy;
    if (threatPlaces.count({newX, newY}) == 0) {
        return ActionRequest::MoveForward;
    }
    return ActionRequest::DoNothing;
}


/**
 * @brief Checks if the tank can fire
 */
bool MyTankAlgorithm::CheckIfCanShoot() const {
    if (numShells > 0 && shootDelay == 0){
        return true;
    }
    return false;
}


/**
 * @brief Update backward delay of the shoot
 */
void MyTankAlgorithm::ShootAShell() {
    numShells --;
    shootDelay = 4;
}

/**
 * @brief Update backward delay of the shoot
 */
void MyTankAlgorithm::UpdateShootDelay() {
    if (shootDelay > 0){
        shootDelay --;
    }
}


/**
 * @brief Checks if the tank should fire at an enemy tank in its line of sight.
 */
ActionRequest MyTankAlgorithm::CheckIfINeedToShootX(int p) {
    if (!CheckIfCanShoot()){
        return ActionRequest::DoNothing;
    }
    Direction dir = myDirection;
    auto [dx, dy] = getDelta(dir);
    int newX = myPosition.first;
    int newY = myPosition.second;
    int left = myPosition.first;
    int right = myPosition.second;

    // Look ahead up to x steps
    for (int step = 1; step <= p; ++step) {
        newX += dx;
        newY += dy;
        wrapPosition(newX, newY);
        auto content = grid[newY][newX];
        // If we hit a wall, line of sight is blocked
        if (content == ObjectType::Wall || content == ObjectType::AllyTank || (newX == left && newY == right)) {
            break;
        }
        if (content == ObjectType::EnemyTank) {
            return ActionRequest::Shoot;
        }
    }
    return ActionRequest::DoNothing;
}

/**
 * @brief Aggregates threats from vicinity and shell trajectories.
 * @return std::set<std::pair<int, int>> All current threats.
 */
std::set<std::pair<int, int>> MyTankAlgorithm::getThreatsAroundMe(){
    auto shells = getCurrThreatShells();
    auto cells = getThreatCellsAroundMe();
    shells.insert(cells.begin(), cells.end());
    return shells;
}

/**
 * @brief Returns dangerous cells around current position (e.g., mines, walls, tanks).
 * @return Set of dangerous nearby cell positions.
 */
std::set<std::pair<int, int>> MyTankAlgorithm::getThreatCellsAroundMe() const {
    int x = myPosition.first;
    int y = myPosition.second;
    std::set<std::pair<int, int>> threatPlaces;
    auto vicinity = doDVicinity(x, y, 1);
    for (const auto& pos : vicinity) {
        int nx = pos.first;
        int ny = pos.second;
        wrapPosition(nx, ny);
        auto content = grid[ny][nx];
        if (content == ObjectType::Wall || content == ObjectType::Mine || content == ObjectType::AllyTank || content == ObjectType::EnemyTank) {
            threatPlaces.insert({nx, ny});
        }
    }
    return threatPlaces;
}

/**
 * @brief Predicts shell danger zones based on prior positions and turns passed.
 * @return Set of predicted shell impact zones.
 */
std::set<std::pair<int, int>> MyTankAlgorithm::getCurrThreatShells() {
    std::set<std::pair<int, int>> threatShells;
    for (const auto& [x, y] : shellsPositions) {
        // Calculate the number of steps based on turns since last update for both blocks the shell move in a single turn
        int stepsSecondCell = 2 * (turnsSinceLastUpdate + 1); 
        int stepsFirstCell = stepsSecondCell - 1; // first cell is one step behind the second cell
        auto currScaryStepOne = doDVicinity(x, y, stepsFirstCell);
        auto currScaryStepTwo = doDVicinity(x, y, stepsSecondCell);
        threatShells.insert(currScaryStepOne.begin(), currScaryStepOne.end());
        threatShells.insert(currScaryStepTwo.begin(), currScaryStepTwo.end());
    }
    return threatShells;
}


/**
 * @brief Returns the 3x3 neighborhood deltas including center.
 */
std::vector<std::pair<int, int>> MyTankAlgorithm::getXDeltas(int x) const {
    return {
        {x * -1, x * -1}, {x * -1, 0}, {x * -1, x},
        {0, x * -1},                   {0, x},
        {x, x * -1},      {x, 0},      {x, x}
    };
}

/**
 * @brief Computes all surrounding coordinates (including self).
 */
std::set<std::pair<int, int>> MyTankAlgorithm::doDVicinity(int x, int y, int d) const{
    auto deltas = getXDeltas(d);
    std::set<std::pair<int, int>> vicinity;
    for (const auto& [dx, dy] : deltas) {
        int nx = x + dx;
        int ny = y + dy;
        wrapPosition(nx, ny);
        vicinity.insert({nx, ny});
    }
    return vicinity;
}

/**
 * @brief Wraps coordinates if they go beyond the limits.
 */
void MyTankAlgorithm::wrapPosition(int& x, int& y) const {
    y = (y + rows) % rows;
    x = (x + cols) % cols;
}