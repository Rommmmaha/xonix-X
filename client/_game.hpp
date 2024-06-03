#pragma once
#include "SFML/Graphics.hpp"
#include "object.hpp"
#include <vector>
class _game
{
  public:
    _game();
    bool running;
    void init();
    void destruct();
    void update();
    void draw();
    bool needInitialization;

  private:
    sf::Color player_color;
    sf::Color enemy_color;
    sf::Color wall_color;
    sf::Color path_color;
    sf::Color background_color;

    bool debug;
    bool gradient;
    int score;
    int number_of_enemies;
    int *map;
    int *tmp_map;
    size_t map_size[3]; // x y map_size

    object player;
    std::vector<object> enemies;

    std::vector<sf::Vector2i> leftNeighbors, rightNeighbors;
    sf::Clock _Clock;
    sf::RenderWindow *_RenderWindow;

    int count_walls() const;
    void update_tmp_map();
};
