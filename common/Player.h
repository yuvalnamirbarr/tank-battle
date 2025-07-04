#pragma once
#include "TankAlgorithm.h"
#include "SatelliteView.h"
#include <iostream>
#include <cstddef>

class Player {
public:
	Player( int player_index,
            size_t x, size_t y,
            size_t max_steps, size_t num_shells ) 
    {
        (void)player_index;
        (void)x;
        (void)y;
        (void)max_steps;
        (void)num_shells;
    }
	virtual ~Player() {
    }
    virtual void updateTankWithBattleInfo (TankAlgorithm& tank, SatelliteView& satellite_view) = 0;
};
