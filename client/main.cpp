#include "_game.hpp"
#pragma warning(disable : 4244 4267 4305)

int main()
{
    _game game;
    while (game.running)
    {
        if (game.needInitialization)
            game.init();
        game.update();
        game.draw();
    }
    game.destruct();
    return 0;
}
