#include "game.hpp"
int main()
{
    game _game;
    while (_game.isRunning())
    {
        if (_game.needInitialization)
            _game.init();
        _game.update();
        _game.draw();
    }
    _game.close();
    return 0;
}
int WinMain()
{
    return main();
}
