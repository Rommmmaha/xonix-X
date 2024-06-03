#include "_game.hpp"
#include "iostream"
#include "myfunc.hpp"
_game::_game()
{
    running = true;
    needInitialization = true;
    gradient = true;
    number_of_enemies = 3;
}

void _game::init()
{
    this->destruct();
    srand(0);
    // srand(time(NULL));
    needInitialization = false;
    // Initialization
    _Clock.restart();
    // Window
    _RenderWindow = new sf::RenderWindow(sf::VideoMode(800, 600), "xonix-x", sf::Style::Titlebar | sf::Style::Close);
    _RenderWindow->setFramerateLimit(15);
    // Map
    map_size[0] = _RenderWindow->getSize().x / 10;
    map_size[1] = _RenderWindow->getSize().y / 10;
    map_size[2] = map_size[0] * map_size[1];
    map = new int[map_size[2]];
    tmp_map = new int[map_size[2]];
    for (int i = 0; i < map_size[2]; ++i)
    {
        map[i] = 0;
        tmp_map[i] = 0;
    }
    // Map borders
    for (int i = 0; i < map_size[0]; ++i)
    {
        map[pos2index(i, 0, map_size[0])] = 1;
        map[pos2index(i, 1, map_size[0])] = 1;
        map[pos2index(i, map_size[1] - 2, map_size[0])] = 1;
        map[pos2index(i, map_size[1] - 1, map_size[0])] = 1;
    }
    for (int i = 2; i < map_size[1] - 2; ++i)
    {
        map[pos2index(0, i, map_size[0])] = 1;
        map[pos2index(1, i, map_size[0])] = 1;
        map[pos2index(map_size[0] - 2, i, map_size[0])] = 1;
        map[pos2index(map_size[0] - 1, i, map_size[0])] = 1;
    }
    // Variables
    score = 0;
    // Player
    player = object(1, int(map_size[0]) / 2, 1);
    player.direction = sf::Keyboard::Space;
    // Enemies
    enemies.clear();
    for (; enemies.size() < number_of_enemies;)
    {
        enemies.push_back(object(2, 3 + rand() % (map_size[0] - 6), 3 + rand() % (map_size[1] - 6)));
    }
    // Colors
    player_color = sf::Color::Green;
    enemy_color = sf::Color::Magenta;
    wall_color = sf::Color::White;
    path_color = sf::Color::White;
    background_color = sf::Color::Black;
}

void _game::destruct()
{
    if (_RenderWindow != nullptr)
    {
        delete _RenderWindow;
    }
    if (map != nullptr)
    {
        delete map;
    }
    if (tmp_map != nullptr)
    {
        delete tmp_map;
    }
    leftNeighbors.clear();
    rightNeighbors.clear();
}

void _game::update()
{
    // Update title
    {
        int preplaced = 4 * (map_size[0] + map_size[1] - 4);
        float percentage = 100.0f * (count_walls() - preplaced) / (map_size[2] - preplaced);
        _RenderWindow->setTitle(std::to_string(percentage));
        if (percentage > 90.0f)
        {
            needInitialization = true;
            return;
        }
    }
    // Events
    sf::Event event;
    while (_RenderWindow->pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            running = false;
            return;
        }
        if (event.type == sf::Event::KeyPressed)
        {
            bool moved = false;
            switch (event.key.code)
            {
            case sf::Keyboard::Up:
            case sf::Keyboard::Down:
            case sf::Keyboard::Left:
            case sf::Keyboard::Right:
                if (((player.direction + event.key.code != sf::Keyboard::Up + sf::Keyboard::Down) &&
                     (player.direction + event.key.code != sf::Keyboard::Left + sf::Keyboard::Right) && !moved) ||
                    map[pos2index(player.x, player.y, map_size[0])] == 1)
                {
                    moved = true;
                    player.direction = event.key.code;
                }
                break;
            case sf::Keyboard::G:
                gradient = !gradient;
                needInitialization = true;
                return;
            case sf::Keyboard::R:
                needInitialization = true;
                return;
            case sf::Keyboard::T:
                debug = !debug;
                break;
            case sf::Keyboard::Escape:
                running = false;
                return;
            }
        }
    }
    // Color gradients
    if (gradient)
    {
        float speed = 30;
        float hue = _Clock.getElapsedTime().asSeconds() * speed;
        player_color = HSV2RGB(120.0f, 1.0f, 1.0f);
        enemy_color = HSV2RGB(hue, 1.0f, 0.4f);
        wall_color = HSV2RGB(hue, 0.5f, 0.6f);
        path_color = HSV2RGB(hue, 0.4f, 0.2f);
        background_color = HSV2RGB(hue, 0.2f, 0.2f);
    }
    // - Calculations
    // Player movement
    player.move();
    // Prevent player from going out of map
    if (player.x < 0 || player.x > map_size[0] - 1 || player.y < 0 || player.y > map_size[1] - 1)
    {
        player.direction = sf::Keyboard::Space;
        for (; player.x < 0; player.x++)
            ;
        for (; player.x > map_size[0] - 1; player.x--)
            ;
        for (; player.y < 0; player.y++)
            ;
        for (; player.y > map_size[1] - 1; player.y--)
            ;
    }
    // Check if player is on path
    if (map[pos2index(player.x, player.y, map_size[0])] == 2)
    {
        needInitialization = true;
        return;
    }
    // Check if player is on air
    if (map[pos2index(player.x, player.y, map_size[0])] == 0)
    {
        // Creating path
        map[pos2index(player.x, player.y, map_size[0])] = 2;
        // Adding neighbors
        switch (player.direction)
        {
        case sf::Keyboard::Up:
            leftNeighbors.push_back(sf::Vector2i(player.x - 1, player.y));
            rightNeighbors.push_back(sf::Vector2i(player.x + 1, player.y));
            break;
        case sf::Keyboard::Down:
            leftNeighbors.push_back(sf::Vector2i(player.x + 1, player.y));
            rightNeighbors.push_back(sf::Vector2i(player.x - 1, player.y));
            break;
        case sf::Keyboard::Left:
            leftNeighbors.push_back(sf::Vector2i(player.x, player.y + 1));
            rightNeighbors.push_back(sf::Vector2i(player.x, player.y - 1));
            break;
        case sf::Keyboard::Right:
            leftNeighbors.push_back(sf::Vector2i(player.x, player.y - 1));
            rightNeighbors.push_back(sf::Vector2i(player.x, player.y + 1));
            break;
        }
    }
    if (map[pos2index(player.x, player.y, map_size[0])] == 1)
    {
        if (map[pos2index(player.previous.x, player.previous.y, map_size[0])] == 2)
        {
            for (size_t i = 0; i < map_size[2]; ++i)
            {
                if (map[i] == 2)
                    map[i] = 1;
            }
            update_tmp_map();
            std::vector<sf::Vector2i> left, right;
            for (auto &i : leftNeighbors)
                getNeighbors(tmp_map, i, map_size, 'l');
            for (auto &i : rightNeighbors)
                getNeighbors(tmp_map, i, map_size, 'r');
            for (size_t i = 0; i < map_size[0]; ++i)
            {
                for (size_t j = 0; j < map_size[1]; ++j)
                {
                    if (tmp_map[pos2index(i, j, map_size[0])] == 'l')
                        left.push_back(sf::Vector2i(i, j));
                    if (tmp_map[pos2index(i, j, map_size[0])] == 'r')
                        right.push_back(sf::Vector2i(i, j));
                }
            }
            if (left.size() < right.size())
                for (auto i : left)
                    map[pos2index(i.x, i.y, map_size[0])] = 1;
            else
                for (auto i : right)
                    map[pos2index(i.x, i.y, map_size[0])] = 1;
            leftNeighbors.clear();
            rightNeighbors.clear();
            player.direction = sf::Keyboard::Space;
        }
    }
    player.updateShape();
    // Enemies movement
    for (auto &enemy : enemies)
    {
        if (map[pos2index(enemy.x, enemy.y, map_size[0])] == 1)
        {
            continue;
        }
        int x = enemy.x;
        int y = enemy.y;
        enemy.move();
        for (; map[pos2index(enemy.x, enemy.y, map_size[0])] == 1;)
        {
            enemy.x = x;
            enemy.y = y;
            bool changed = false;
            if (map[pos2index(enemy.x - 1, enemy.y, map_size[0])] == 1 ||
                map[pos2index(enemy.x + 1, enemy.y, map_size[0])] == 1)
            {
                enemy.direction = (enemy.direction + 2) % 4;
                changed = true;
            }
            if (map[pos2index(enemy.x, enemy.y - 1, map_size[0])] == 1)
            {
                enemy.direction = (enemy.direction + 1) % 4;
                changed = true;
            }
            if (map[pos2index(enemy.x, enemy.y + 1, map_size[0])] == 1)
            {
                enemy.direction = (enemy.direction - 1) % 4;
                changed = true;
            }
            if (!changed)
            {
                switch (enemy.direction)
                {
                case 0:
                    enemy.direction = 3;
                    break;
                case 1:
                    enemy.direction = 2;
                    break;
                case 2:
                    enemy.direction = 1;
                    break;
                case 3:
                    enemy.direction = 0;
                    break;
                }
            }
            enemy.move();
        }
        // Check if enemy is on path
        if (map[pos2index(enemy.x, enemy.y, map_size[0])] == 2)
        {
            needInitialization = true;
            return;
        }
        enemy.updateShape();
    }
}

void _game::draw()
{
    // - Render Start
    // Background
    _RenderWindow->clear(background_color);
    // Draw walls
    sf::RectangleShape wall(sf::Vector2f(10, 10));
    sf::RectangleShape path(sf::Vector2f(10, 10));
    sf::RectangleShape scanR(sf::Vector2f(10, 10));
    sf::RectangleShape scanL(sf::Vector2f(10, 10));
    wall.setFillColor(wall_color);
    path.setFillColor(path_color);
    scanL.setFillColor(sf::Color::Blue);
    scanR.setFillColor(sf::Color::Red);
    sf::Vector2i i = sf::Vector2i(0, 0);
    if (debug)
    {
        for (auto &i : leftNeighbors)
        {
            scanL.setPosition(sf::Vector2f(i.x * 10.0f, i.y * 10.0f));
            _RenderWindow->draw(scanL);
        }
        for (auto &i : rightNeighbors)
        {
            scanR.setPosition(sf::Vector2f(i.x * 10.0f, i.y * 10.0f));
            _RenderWindow->draw(scanR);
        }
    }
    for (i.y = 0; i.y < map_size[1]; ++i.y)
    {
        for (i.x = 0; i.x < map_size[0]; ++i.x)
        {
            if (map[pos2index(i.x, i.y, map_size[0])] == 1)
            {
                wall.setPosition(sf::Vector2f(i.x * 10.0f, i.y * 10.0f));
                _RenderWindow->draw(wall);
            }
            if (map[pos2index(i.x, i.y, map_size[0])] == 2)
            {
                path.setPosition(sf::Vector2f(i.x * 10.0f, i.y * 10.0f));
                _RenderWindow->draw(path);
            }
            if (debug && map[pos2index(player.x, player.y, map_size[0])] == 1)
            {
                if (tmp_map[pos2index(i.x, i.y, map_size[0])] == 'l')
                {
                    scanL.setPosition(sf::Vector2f(i.x * 10.0f, i.y * 10.0f));
                    _RenderWindow->draw(scanL);
                }
                if (tmp_map[pos2index(i.x, i.y, map_size[0])] == 'r')
                {
                    scanR.setPosition(sf::Vector2f(i.x * 10.0f, i.y * 10.0f));
                    _RenderWindow->draw(scanR);
                }
            }
        }
    }
    // Draw enemies
    for (auto &enemy : enemies)
    {
        enemy.shape.setFillColor(enemy_color);
        _RenderWindow->draw(enemy.shape);
    }
    // Draw player
    player.shape.setFillColor(player_color);
    _RenderWindow->draw(player.shape);
    // - Display
    _RenderWindow->display();
}

int _game::count_walls() const
{
    int count = 0;
    for (int i = 0; i < map_size[0]; ++i)
    {
        for (int j = 0; j < map_size[1]; ++j)
        {
            if (map[pos2index(i, j, map_size[0])] == 1)
                count++;
        }
    }
    return count;
}

void _game::update_tmp_map()
{
    for (size_t i = 0; i < map_size[2]; ++i)
    {
        tmp_map[i] = map[i];
    }
}
