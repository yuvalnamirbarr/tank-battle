#pragma once

#include "MyTankAlgorithm.h"
#include "MyBattleInfo.h"
#include "Tank.h"
#include "ActionRequest.h"
#include "BattleInfo.h"
#include "ObjectType.h"
#include <queue>
#include <vector>
#include <tuple>
#include <map>
#include <utility>
#include <cmath>
#include <algorithm>

/**
 * @class ChasingTankAlgorithm
 * @brief Implements a tank algorithm that actively chases the enemy using BFS pathfinding.
 *        Combines reactive behavior (avoiding threats, shooting) with strategic planning.
 */
class ChasingTankAlgorithm : public MyTankAlgorithm {
public:
    /**
     * @brief Constructor.
     * @param playerIndex The index of the player (1 or 2).
     * @param tankIndex The index of the tank for this player.
     */
    ChasingTankAlgorithm(int playerIndex, int tankIndex);

    /**
     * @brief Computes the next action for the tank on this turn.
     *        Prioritizes avoiding threats, shooting, and path-following toward the enemy.
     * @return The chosen ActionRequest.
     */
    ActionRequest getAction() override;

    /**
     * @brief Updates the internal tank state with new battlefield information.
     * @param info The latest BattleInfo from the game engine.
     */
    void updateBattleInfo(BattleInfo& info) override;

private:
    std::vector<ActionRequest> currentPath;                      ///< Cached path of actions to perform.
    std::vector<std::pair<int, int>> plannedPositions;           ///< Planned positions associated with currentPath.
    std::pair<int, int> enemyPosition = { -1, -1 };              ///< Last known position of the enemy.
    std::pair<int, int> lastEnemyPosition = { -1, -1 };          ///< Previous known position of the enemy.

    /**
     * @struct State
     * @brief Represents a state in the grid, including position and direction.
     *        For the use of BFS calculations.
     */
    struct State {
        int x, y;
        Direction direction;

        State() : x(0), y(0), direction(Direction::U) {}
        State(int x, int y, Direction dir) : x(x), y(y), direction(dir) {}

        bool operator<(const State& other) const {
            return std::tie(x, y, direction) < std::tie(other.x, other.y, other.direction);
        }

        bool operator==(const State& other) const {
            return x == other.x && y == other.y && direction == other.direction;
        }

        bool operator!=(const State& other) const {
            return !(*this == other);
        }
    };

    // --- BFS pathfinding state ---
    
    std::queue<State> q;                                            ///< BFS queue.
    std::map<State, State> parent;                                  ///< BFS parent map for path reconstruction.
    std::vector<std::vector<std::vector<bool>>> visited;            ///< 3D visited grid (x, y, direction).

    // --- BFS helper methods ---

    /**
     * @brief Initializes BFS data structures starting from the current tank position.
     */
    void initializeBFS();

    /**
     * @brief Retrieves and removes the next state from the BFS queue.
     * @return The next state to process.
     */
    State getNextStateFromQueue();

    /**
     * @brief Checks whether the current state matches the enemy’s known position.
     * @param current The current state being examined.
     * @return True if this state is at the enemy position.
     */
    bool reachedEnemy(const State& current) const;

    /**
     * @brief Reconstructs the path to the enemy using the parent map.
     * @param current The state where the enemy was found.
     * @return A pair containing the action sequence and planned positions.
     */
    std::pair<std::vector<ActionRequest>, std::vector<std::pair<int, int>>> reconstructPath(State current);

    /**
     * @brief Attempts to move forward from the given state, adds to BFS if valid.
     * @param current The state to move from.
     */
    void tryMoveForward(const State& current);

    /**
     * @brief Attempts all possible rotations from the current state, adds to BFS if unvisited.
     * @param current The state to rotate from.
     */
    void tryAllRotations(const State& current);

    /**
     * @brief Performs BFS to find a path to the enemy tank.
     * @return A pair of action list and planned positions from current location to the enemy.
     */
    std::pair<std::vector<ActionRequest>, std::vector<std::pair<int, int>>> calculatePathBFS();

    // --- Action decision helpers ---

    /**
     * @brief Infers the action needed to move from one state to another (forward or rotation).
     * @param from The current state.
     * @param to The next state.
     * @return The ActionRequest to transition from 'from' to 'to'.
     */
    ActionRequest inferAction(const State& from, const State& to);

    /**
     * @brief Executes a shoot action, updates internal state, and resets enemy position.
     * @return ActionRequest::Shoot
     */
    ActionRequest HandleShootRequest();

    /**
     * @brief Triggers BFS path recalculation and stores the new path.
     */
    void HandleBFS();

    /**
     * @brief Executes the next action in the current path, updating the tank's internal state.
     * @return The next ActionRequest in the path.
     */
    ActionRequest ContinueAlongPath();
    
};
