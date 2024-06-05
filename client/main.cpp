#include "game.hpp"
int main()
{
    game _game;
    while (_game.running)
    {
        if (_game.needInitialization)
            _game.init();
        _game.update();
        _game.draw();
    }
    return 0;
}
int WinMain()
{
    return main();
}
