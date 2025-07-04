#pragma once

#include "TankAlgorithmFactory.h"
#include "TankAlgorithm.h"
#include "MyTankAlgorithm.h"
#include "BasicTankAlgorithm.h"
#include "ChasingTankAlgorithm.h"
#include <memory>

/**
 * @brief Factory that creates different tank algorithms based on player index.
 * 
 * Player 1 receives a BasicTankAlgorithm, while player 2 receives a ChasingTankAlgorithm.
 */
class MyTankAlgorithmFactory : public TankAlgorithmFactory {
public:
    /**
     * @brief Creates a tank algorithm instance per player.
     * 
     * @param player_index Index of the player (1 or 2).
     * @param tank_index Index of the tank (usually 0).
     * @return A smart pointer to the created tank algorithm.
     */
    std::unique_ptr<TankAlgorithm> create(int player_index, int tank_index) const override {
        if (player_index == 1){
            return std::make_unique<BasicTankAlgorithm>(player_index, tank_index);
        }
        else {
            return std::make_unique<ChasingTankAlgorithm>(player_index, tank_index);
        }
    }

    /**
     * @brief Virtual destructor.
     */
    ~MyTankAlgorithmFactory() override = default;
};
