#include "game.hpp"
#include "myfunc.hpp"
game::game()
{
    srand(unsigned int(time(NULL)));
    running = true;
    needInitialization = true;

    map = nullptr;
    tmp_map = nullptr;

    sb_intro.loadFromFile("resources/intro.wav");
    sb_damaged.loadFromFile("resources/damaged.wav");
    sb_captured.loadFromFile("resources/captured.wav");

    intro.setBuffer(sb_intro);
    damaged.setBuffer(sb_damaged);
    captured.setBuffer(sb_captured);

    font.loadFromFile("resources/04B_30__.TTF");

    _RenderWindow = new sf::RenderWindow(sf::VideoMode(800, 600), "xonix-x", sf::Style::Titlebar | sf::Style::Close);
    _RenderWindow->setFramerateLimit(fps);

    firstStart = true;
}

bool game::isRunning() const
{
    return running;
}

void game::close()
{
    clear();
    _RenderWindow->close();
}

int game::count_walls() const
{
    int count = 0;
    for (int i = 0; i < map_size.x; ++i)
        for (int j = 0; j < map_size.y; ++j)
            if (map[pos2index(i, j, map_size.x)] == 1)
                ++count;
    return count;
}

void game::update_tmp_map()
{
    for (size_t i = 0; i < map_size.z; ++i)
        tmp_map[i] = map[i];
}

void game::clear()
{
    if (map != nullptr)
        delete map;
    if (tmp_map != nullptr)
        delete tmp_map;
    map = nullptr;
    tmp_map = nullptr;
    leftNeighbours.clear();
    rightNeighbours.clear();
}

void game::init()
{

    if (firstStart)
    {
        fps = 10;
        scale = 20;
        debug = false;
        number_of_enemies = 2;
    }
    // Preparing
    clear();
    needInitialization = false;

    // Window
    scale_vector = {scale, scale};

    // Map
    map_size.x = size_t(_RenderWindow->getSize().x / scale);
    map_size.y = size_t(_RenderWindow->getSize().y / scale);
    map_size.z = map_size.x * map_size.y;
    map = new int[map_size.z];
    tmp_map = new int[map_size.z];
    for (int i = 0; i < map_size.z; ++i)
        map[i] = 0;

    // Map borders
    for (int i = 0; i < map_size.x; ++i)
    {
        map[pos2index(i, 0, map_size.x)] = 1;
        map[pos2index(i, 1, map_size.x)] = 1;
        map[pos2index(i, map_size.y - 2, map_size.x)] = 1;
        map[pos2index(i, map_size.y - 1, map_size.x)] = 1;
    }
    for (int i = 2; i < map_size.y - 2; ++i)
    {
        map[pos2index(0, i, map_size.x)] = 1;
        map[pos2index(1, i, map_size.x)] = 1;
        map[pos2index(map_size.x - 2, i, map_size.x)] = 1;
        map[pos2index(map_size.x - 1, i, map_size.x)] = 1;
    }

    // Player
    player = object(1, int(map_size.x) / 2, 1);
    player.shape = sf::RectangleShape(scale_vector);
    player.direction = -1;

    // Enemies
    enemies.clear();
    for (; enemies.size() < number_of_enemies;)
    {
        enemies.push_back(object(2, 3 + rand() % (map_size.x - 6), 3 + rand() % (map_size.y - 6)));
        enemies.back().shape = sf::RectangleShape(scale_vector);
    }

    // Restarting clock
    _Clock.restart();
    // First start
    if (firstStart)
    {
        intro.play();
        _GlobalClock.restart();
        firstStart = false;
    }
}

void game::update()
{
    // Updating title
    {
        int preplaced = 4 * (int(map_size.x) + int(map_size.y) - 4);
        float percentage = 100.0f * (count_walls() - preplaced) / (map_size.z - preplaced);
        if (percentage > 90.0f)
        {
            ++number_of_enemies;
            needInitialization = true;
            return;
        }

        std::string title;
        title += "Level: ";
        title += std::to_string(number_of_enemies - 2);
        title += "  |  ";
        title += std::to_string(int(percentage));
        title += "% < 90%  |  FPS-lock: ";
        if (fps == 0)
            title += "OFF";
        else
            title += std::to_string(fps);
        _RenderWindow->setTitle(title);
    }

    // Events
    bool moved = false;
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
            switch (event.key.code)
            {
            case sf::Keyboard::Up:
            case sf::Keyboard::Down:
            case sf::Keyboard::Left:
            case sf::Keyboard::Right:
                if (((player.direction + event.key.code != sf::Keyboard::Up + sf::Keyboard::Down) &&
                     (player.direction + event.key.code != sf::Keyboard::Left + sf::Keyboard::Right) && !moved) ||
                    map[pos2index(player.x, player.y, map_size.x)] == 1)
                {
                    moved = true;
                    player.direction = event.key.code;
                }
                break;
            case sf::Keyboard::R:
                firstStart = true;
                needInitialization = true;
                return;
            case sf::Keyboard::D:
                debug = !debug;
                break;
            case sf::Keyboard::PageUp:
                ++scale;
                needInitialization = true;
                return;
            case sf::Keyboard::PageDown:
                --scale;
                scale = scale < 5 ? 5 : scale;
                needInitialization = true;
                return;
            case sf::Keyboard::Equal:
                ++fps;
                if (fps < 10)
                    fps = 10;
                _RenderWindow->setFramerateLimit(fps);
                break;
            case sf::Keyboard::Dash:
                --fps;
                if (fps < 10)
                    fps = 0;
                _RenderWindow->setFramerateLimit(fps);
                break;
            case sf::Keyboard::Space:
                return;
            case sf::Keyboard::Escape:
                running = false;
                return;
            }
        }
    }
    // Color gradients
    float speed = 30;
    float hue = _Clock.getElapsedTime().asSeconds() * speed;
    player_color = HSV2RGB(120.0f, 1.0f, 1.0f);
    enemy_color = HSV2RGB(hue, 1.0f, 0.4f);
    wall_color = HSV2RGB(hue, 0.5f, 0.6f);
    path_color = HSV2RGB(hue, 0.4f, 0.2f);
    background_color = HSV2RGB(hue, 0.2f, 0.2f);

    // - Calculations

    // Player movement
    player.move();

    // Prevent player from going out of map
    if (player.x < 0 || player.x > map_size.x - 1 || player.y < 0 || player.y > map_size.y - 1)
    {
        player.direction = -1;
        for (; player.x < 0; ++player.x)
            ;
        for (; player.x > map_size.x - 1; --player.x)
            ;
        for (; player.y < 0; ++player.y)
            ;
        for (; player.y > map_size.y - 1; --player.y)
            ;
    }

    // Check if player is on path
    if (map[pos2index(player.x, player.y, map_size.x)] == 2)
    {
        damaged.play();
        needInitialization = true;
        return;
    }

    // Check if player is on air
    if (map[pos2index(player.x, player.y, map_size.x)] == 0)
    {
        // Creating path
        map[pos2index(player.x, player.y, map_size.x)] = 2;

        // Adding Neighbours
        switch (player.direction)
        {
        case sf::Keyboard::Up:
            leftNeighbours.push_back(sf::Vector2i(player.x - 1, player.y));
            rightNeighbours.push_back(sf::Vector2i(player.x + 1, player.y));
            break;
        case sf::Keyboard::Down:
            leftNeighbours.push_back(sf::Vector2i(player.x + 1, player.y));
            rightNeighbours.push_back(sf::Vector2i(player.x - 1, player.y));
            break;
        case sf::Keyboard::Left:
            leftNeighbours.push_back(sf::Vector2i(player.x, player.y + 1));
            rightNeighbours.push_back(sf::Vector2i(player.x, player.y - 1));
            break;
        case sf::Keyboard::Right:
            leftNeighbours.push_back(sf::Vector2i(player.x, player.y - 1));
            rightNeighbours.push_back(sf::Vector2i(player.x, player.y + 1));
            break;
        }
    }

    if (map[pos2index(player.x, player.y, map_size.x)] == 1)
    {
        if (map[pos2index(player.previous.x, player.previous.y, map_size.x)] == 2)
        {
            for (size_t i = 0; i < map_size.z; ++i)
                if (map[i] == 2)
                    map[i] = 1;
            update_tmp_map();
            std::vector<sf::Vector2i> left, right;

            for (auto &i : leftNeighbours)
                getNeighbours(tmp_map, i, map_size, 'l');
            for (auto &i : rightNeighbours)
                getNeighbours(tmp_map, i, map_size, 'r');

            for (sf::Vector2i i{0, 0}; i.y < map_size.y; ++i.y)
            {
                for (i.x = 0; i.x < map_size.x; ++i.x)
                {
                    if (tmp_map[pos2index(i.x, i.y, map_size.x)] == 'l')
                        left.push_back(i);
                    if (tmp_map[pos2index(i.x, i.y, map_size.x)] == 'r')
                        right.push_back(i);
                }
            }

            if (left.size() < right.size())
                for (auto &i : left)
                    map[pos2index(i.x, i.y, map_size.x)] = 1;
            else
                for (auto &i : right)
                    map[pos2index(i.x, i.y, map_size.x)] = 1;

            leftNeighbours.clear();
            rightNeighbours.clear();
            player.direction = -1;
            captured.play();
        }
    }
    player.updateShape(scale_vector);

    // Enemies movement
    for (auto &enemy : enemies)
    {
        if (map[pos2index(enemy.x, enemy.y, map_size.x)] == 1)
            continue;

        sf::Vector2i old = enemy;

        enemy.move();
        for (; map[pos2index(enemy.x, enemy.y, map_size.x)] == 1;)
        {
            enemy.x = old.x;
            enemy.y = old.y;
            int old_direction = enemy.direction;

            if (map[pos2index(enemy.x - 1, enemy.y, map_size.x)] == 1 ||
                map[pos2index(enemy.x + 1, enemy.y, map_size.x)] == 1)
                enemy.direction = (enemy.direction + 2) % 4;
            if (map[pos2index(enemy.x, enemy.y - 1, map_size.x)] == 1)
                enemy.direction = (enemy.direction + 1) % 4;
            if (map[pos2index(enemy.x, enemy.y + 1, map_size.x)] == 1)
                enemy.direction = (enemy.direction - 1) % 4;

            if (old_direction == enemy.direction)
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
        if (map[pos2index(enemy.x, enemy.y, map_size.x)] == 2)
        {
            damaged.play();
            needInitialization = true;
            return;
        }
        enemy.updateShape(scale_vector);
    }
}

void game::draw()
{
    if (!running)
        return;
    // - Render Start

    // Background
    _RenderWindow->clear(background_color);

    // Init abojects and colors
    sf::Vector2i i = sf::Vector2i(0, 0);

    sf::RectangleShape wall(scale_vector);
    sf::RectangleShape path(scale_vector);
    sf::RectangleShape scanR(scale_vector);
    sf::RectangleShape scanL(scale_vector);
    wall.setFillColor(wall_color);
    path.setFillColor(path_color);
    scanL.setFillColor(sf::Color(0, 0, 255, 100));
    scanR.setFillColor(sf::Color(255, 0, 0, 100));

    // [d] Drawing sides
    if (debug)
    {
        for (auto &i : leftNeighbours)
        {
            scanL.setPosition(sf::Vector2f(i.x * scale, i.y * scale));
            _RenderWindow->draw(scanL);
        }
        for (auto &i : rightNeighbours)
        {
            scanR.setPosition(sf::Vector2f(i.x * scale, i.y * scale));
            _RenderWindow->draw(scanR);
        }
    }
    for (i.y = 0; i.y < map_size.y; ++i.y)
    {
        for (i.x = 0; i.x < map_size.x; ++i.x)
        {
            // Drawing walls
            if (map[pos2index(i.x, i.y, map_size.x)] == 1)
            {
                wall.setPosition(sf::Vector2f(i.x * scale, i.y * scale));
                _RenderWindow->draw(wall);
            }
            // Drawing player path
            if (map[pos2index(i.x, i.y, map_size.x)] == 2)
            {
                path.setPosition(sf::Vector2f(i.x * scale, i.y * scale));
                _RenderWindow->draw(path);
            }
            // [d] Drawing sides
            if (debug && map[pos2index(player.x, player.y, map_size.x)] == 1)
            {
                if (tmp_map[pos2index(i.x, i.y, map_size.x)] == 'l')
                {
                    scanL.setPosition(sf::Vector2f(i.x * scale, i.y * scale));
                    _RenderWindow->draw(scanL);
                }
                if (tmp_map[pos2index(i.x, i.y, map_size.x)] == 'r')
                {
                    scanR.setPosition(sf::Vector2f(i.x * scale, i.y * scale));
                    _RenderWindow->draw(scanR);
                }
            }
        }
    }

    // Drawing enemies
    for (auto &enemy : enemies)
    {
        enemy.shape.setFillColor(enemy_color);
        _RenderWindow->draw(enemy.shape);
    }

    // Drawing player
    player.shape.setFillColor(player_color);
    _RenderWindow->draw(player.shape);

    // Drawing intro overlay
    if (_GlobalClock.getElapsedTime().asSeconds() < 5)
    {
        float seconds = _GlobalClock.getElapsedTime().asSeconds();
        int alpha = 0, alpha2 = 255;
        _RenderWindow->setFramerateLimit(0);
        if (0.16 < seconds && seconds < 1.18)
            alpha = int((seconds - 0.16f) / 0.025f) % 2 ? int(100.0f * (seconds - 0.16f)) : 0;
        if (1.16 < seconds && seconds < 3)
            alpha = 255;
        if (3 < seconds)
        {
            _RenderWindow->setFramerateLimit(fps);
            alpha = int((5 - _GlobalClock.getElapsedTime().asSeconds()) / 2.0f * 255);
            alpha2 = alpha;
        }

        sf::Text _Text("xonix-X", font, 120);
        _Text.setPosition(sf::Vector2f(_RenderWindow->getSize().x / 2 - _Text.getGlobalBounds().width / 2,
                                       _RenderWindow->getSize().y / 2 - _Text.getGlobalBounds().height / 2 -
                                           _Text.getCharacterSize() / 2));
        _Text.setFillColor(sf::Color(255, 255, 255, alpha));
        sf::RectangleShape bg(V2_convert<float>(_RenderWindow->getSize()));
        bg.setFillColor(sf::Color(0, 0, 0, alpha2));

        _RenderWindow->draw(bg);
        _RenderWindow->draw(_Text);
    }
    // - Display
    _RenderWindow->display();
}
