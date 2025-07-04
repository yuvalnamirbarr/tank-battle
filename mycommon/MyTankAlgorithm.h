#pragma once

#include "TankAlgorithm.h"
#include "Direction.h"
#include "ActionRequest.h"
#include "BattleInfo.h"
#include "ObjectType.h"
#include "MyBattleInfo.h"
#include <set>
#include <vector>
#include <utility>

class MyTankAlgorithm : public TankAlgorithm {
protected:
    int playerIndex;
    int tankIndex;
    int rows;
    int cols;
    std::vector<std::vector<ObjectType>> grid;
    std::pair<int,int> myPosition;
    Direction myDirection;
    std::set<std::pair<int, int>> shellsPositions; // positions of shells in the last grid
    int turnsSinceLastUpdate;
    int shootDelay;
    int numShells;

    /**
    * @brief Updates internal tank state after executing an action.
    * @param action The action that was just performed.
    */
    void updatePostAction(ActionRequest action);

    /**
    * @brief Updates the internal board representation based on external battle info.
    * @param info The BattleInfo object containing updated board data.
    */
    void updateGrid(BattleInfo& info);

    /**
    * @brief Moves one step in the given direction with wrapping.
    */
    std::pair<int, int> moveInDirectionD(int x, int y, int d, Direction dir);
    
    /**
    * @brief Core logic deciding whether to move, shoot, or wait.
    * 
    * First checks if the tank is standing in a danger zone.
    * If yes, attempts to move to a safer location.
    * Otherwise, considers shooting if an enemy is in sight.
    */
    ActionRequest getThreatningNextAction(std::set<std::pair<int, int>> threatPlaces);

    /**
    * @brief Attempts to move the tank away from danger, either forward or backward.
    */
    ActionRequest MoveTankFromDanger(std::set<std::pair<int, int>> threatPlaces);

    /**
    * @brief Checks if the tank should fire at an enemy tank in its line of sight.
    */
    ActionRequest CheckIfINeedToShootX(int x);

    /**
    * @brief Aggregates threats from vicinity and shell trajectories.
    * @return std::set<std::pair<int, int>> All current threats.
    */
    std::set<std::pair<int, int>> getThreatsAroundMe();

    /**
    * @brief Returns dangerous cells around current position (e.g., mines, walls, tanks).
    * @return Set of dangerous nearby cell positions.
    */
    std::set<std::pair<int, int>> getThreatCellsAroundMe() const;

    /**
    * @brief Predicts shell danger zones based on prior positions and turns passed.
    * @return Set of predicted shell impact zones.
    */
    std::set<std::pair<int, int>> getCurrThreatShells();

    /**
    * @brief Returns the 3x3 neighborhood deltas including center.
    */
    std::vector<std::pair<int, int>> getXDeltas(int x) const;

    /**
    * @brief Computes all surrounding coordinates (including self).
    */
    std::set<std::pair<int, int>> doDVicinity(int x, int y, int d) const;

    /**
    * @brief Wraps coordinates if they go beyond the limits.
    */
    void wrapPosition(int& x, int& y) const;

    /**
    * @brief Checks if the tank can fire
    */
    bool CheckIfCanShoot() const;

    /**
    * @brief Update backward delay of the shoot
    */
    void ShootAShell();

    /**
    * @brief Update backward delay of the shoot
    */
    void UpdateShootDelay();

public:
    /**
    * @brief Constructs a new MyTankAlgorithm object.
    * @param playerIndex Index of the player (1 or 2).
    * @param tankIndex Index of the tank (currently always 0).
    */
    MyTankAlgorithm(int playerIndex, int tankIndex);

    /**
    * @brief Gets the next action to perform (pure virtual).
    * @return ActionRequest Chosen action.
    */
    virtual ActionRequest getAction() override = 0;

    /**
    * @brief Updates the tank with battle information (pure virtual).
    * @param info The updated BattleInfo.
    */
    virtual void updateBattleInfo(BattleInfo& info) override = 0;

    /**
    * @brief Destructor.
    */
    ~MyTankAlgorithm() override = default;
};