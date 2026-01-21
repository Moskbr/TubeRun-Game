#include "game.hpp"

int main(int argc, char* argv[])
{
    (void)argc; (void)argv;// remove warnings

    Game game;
    if (!game.init()) return 1;

    game.run();
    game.shutdown();// pode ir para o destrutor

    return 0;
}