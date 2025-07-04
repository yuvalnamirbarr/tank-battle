#include "ChasingTankAlgorithm.h"

/**
 * @brief Constructor for the chasing tank algorithm.
 */
ChasingTankAlgorithm::ChasingTankAlgorithm(int playerIndex, int tankIndex)
    : MyTankAlgorithm(playerIndex, tankIndex) {}


/**
 * @brief Updates the tank’s internal information based on new battle data.
 */
void ChasingTankAlgorithm::updateBattleInfo(BattleInfo& info){
    auto myinfo = static_cast<MyBattleInfo&>(info);
    myPosition = myinfo.getMyPosition();
    lastEnemyPosition = enemyPosition;
    enemyPosition = myinfo.getEnemyPosition();
    shellsPositions = myinfo.getShellsPositions(); 
    if (numShells == -1){
        numShells = myinfo.getNumShells();
    }
    updateGrid(info);
}

/**
 * @brief Returns the next action for the tank.
 **/
ActionRequest ChasingTankAlgorithm::getAction() {
    UpdateShootDelay();
    // First turn \ search for new target - get information about the game
    if(enemyPosition.first == -1 && enemyPosition.second == -1){
        turnsSinceLastUpdate = 1;
        return ActionRequest::GetBattleInfo;
    }
    // Avoid threats
    ActionRequest action = getThreatningNextAction(getThreatsAroundMe());
    if (action != ActionRequest::DoNothing) {
        updatePostAction(action);
        turnsSinceLastUpdate++;
        return action;
    }
    // Shoot in needed
    if(CheckIfINeedToShootX(6) == ActionRequest::Shoot){
        if(CheckIfCanShoot()){return HandleShootRequest();}
    }
    // Update my info
    if(turnsSinceLastUpdate > 3){
        turnsSinceLastUpdate = 1;
        return ActionRequest::GetBattleInfo;
    }
    // Recalculate path if needed
    if (currentPath.empty() || enemyPosition != lastEnemyPosition || myPosition != plannedPositions.front()) {
        HandleBFS();
    }
    // No path or reached the end — fallback to default attack or get battle information
    if(CheckIfINeedToShootX(1) == ActionRequest::Shoot || currentPath.empty()){
        if(CheckIfCanShoot()){return HandleShootRequest();}
    }
    // Continue along the current path if it exists
    if (!currentPath.empty()) {return ContinueAlongPath();}
    // Fallback
    enemyPosition = {-1, -1};
    turnsSinceLastUpdate = 1;
    return ActionRequest::GetBattleInfo;
}

/**
 * @brief Computes a path to the enemy using BFS.
 */
std::pair<std::vector<ActionRequest>, std::vector<std::pair<int, int>>> ChasingTankAlgorithm::calculatePathBFS() {
    initializeBFS();
    while (!q.empty()) {
        State current = getNextStateFromQueue();
        if (reachedEnemy(current)) {
            return reconstructPath(current);
        }
        tryMoveForward(current);
        tryAllRotations(current);
    }
    return {{}, {}};
}

// ----------------------------------------------------------------
//                    getAction UTILITIES
// ----------------------------------------------------------------

/**
 * @brief Deduces the action needed to go from one state to another.
 */
ActionRequest ChasingTankAlgorithm::inferAction(const State& from, const State& to) {
    if (from.x == to.x && from.y == to.y) {
        // Rotation only
        int diff = (int(to.direction) - int(from.direction) + 8) % 8;
        if (diff == 1) return ActionRequest::RotateRight45;
        if (diff == 2) return ActionRequest::RotateRight90;
        if (diff == 7) return ActionRequest::RotateLeft45;
        if (diff == 6) return ActionRequest::RotateLeft90;
    } else {
        // Position changed — must be a move forward
        return ActionRequest::MoveForward;
    }
    return ActionRequest::DoNothing; // fallback
}

/**
 * @brief Executes a shoot action and updates internal state accordingly.
 */
ActionRequest ChasingTankAlgorithm::HandleShootRequest(){
    updatePostAction(ActionRequest::Shoot);
    turnsSinceLastUpdate++;
    enemyPosition = {-1, -1};
    return ActionRequest::Shoot;
}

/**
 * @brief Recomputes a BFS path to the enemy and stores it in currentPath and plannedPositions.
 */
void ChasingTankAlgorithm::HandleBFS(){
    auto pathResult = calculatePathBFS();
    currentPath = std::move(pathResult.first);
    plannedPositions = std::move(pathResult.second);
    lastEnemyPosition = enemyPosition;
}

/**
 * @brief Performs the next action along the precomputed path.
 */
ActionRequest ChasingTankAlgorithm::ContinueAlongPath(){
    ActionRequest nextAction = currentPath.front();
    currentPath.erase(currentPath.begin());
    plannedPositions.erase(plannedPositions.begin());
    updatePostAction(nextAction);
    turnsSinceLastUpdate++;
    return nextAction;
}

// ----------------------------------------------------------------
//                calculatePathBFS UTILITIES
// ----------------------------------------------------------------

/**
 * @brief Initializes the BFS queue and visited states starting from the tank's current position.
 */
void ChasingTankAlgorithm::initializeBFS() {
    visited = std::vector(rows, std::vector(cols, std::vector(8, false)));

    auto [startX, startY] = myPosition;
    State start{startX, startY, myDirection};
    q = std::queue<State>();
    parent.clear();

    q.push(start);
    visited[start.y][start.x][(int)start.direction] = true;
}

/**
 * @brief Retrieves and removes the front state from the BFS queue.
 */
ChasingTankAlgorithm::State ChasingTankAlgorithm::getNextStateFromQueue() {
    State current = q.front();
    q.pop();
    return current;
}

/**
 * @brief Checks whether the given state corresponds to the current known enemy position.
 */
bool ChasingTankAlgorithm::reachedEnemy(const State& current) const {
    auto [enemyX, enemyY] = enemyPosition;
    return current.x == enemyX && current.y == enemyY;
}

/**
 * @brief Reconstructs the path of actions and positions from the enemy to the tank using the BFS parent map.
 */
std::pair<std::vector<ActionRequest>, std::vector<std::pair<int, int>>> 
ChasingTankAlgorithm::reconstructPath(State current) {
    std::vector<ActionRequest> actions;
    std::vector<std::pair<int, int>> positions;

    while (current != State{myPosition.first, myPosition.second, myDirection}) {
        State prev = parent[current];
        actions.push_back(inferAction(prev, current));
        positions.push_back({current.x, current.y});
        current = prev;
    }

    positions.push_back(myPosition);
    std::reverse(actions.begin(), actions.end());
    std::reverse(positions.begin(), positions.end());
    return {actions, positions};
}


/**
 * @brief Attempts to move forward from the current state.
 */
void ChasingTankAlgorithm::tryMoveForward(const State& current) {
    auto [nx, ny] = moveInDirectionD(current.x, current.y, 1, current.direction);
    ObjectType content = grid[ny][nx];

    if (content != ObjectType::Wall &&
        content != ObjectType::Mine &&
        content != ObjectType::AllyTank &&
        !visited[ny][nx][(int)current.direction]) {
        
        visited[ny][nx][(int)current.direction] = true;
        State next{nx, ny, current.direction};
        q.push(next);
        parent[next] = current;
    }
}

/**
 * @brief Tries all possible rotations from the current state.
 */
void ChasingTankAlgorithm::tryAllRotations(const State& current) {
    for (auto turnFunc : {&turnLeft, &turnRight, &turnLeftQuarter, &turnRightQuarter}) {
        Direction newDir = (*turnFunc)(current.direction);
        if (!visited[current.y][current.x][(int)newDir]) {
            visited[current.y][current.x][(int)newDir] = true;
            State next{current.x, current.y, newDir};
            q.push(next);
            parent[next] = current;
        }
    }
}
