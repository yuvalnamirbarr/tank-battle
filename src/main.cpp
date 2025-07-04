#include "GameManager.h"
#include "MyPlayerFactory.h"
#include "MyTankAlgorithmFactory.h"

int main(int argc, char* argv[]) {
    if (argc != 2) return 1;

    GameManager game(
        std::make_unique<MyPlayerFactory>(), 
        std::make_unique<MyTankAlgorithmFactory>()
    );

    if (!game.readBoard(argv[1])) return 1;
    
    game.run();
    return 0;
}