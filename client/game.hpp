#pragma once
#include "SFML/Audio.hpp"
#include "SFML/Graphics.hpp"
#include "object.hpp"
#include <vector>
class game
{
  public:
    game();
    void clear();
    void draw_intro();

    void init();
    void update();
    void draw();

    bool running;
    bool needInitialization;

  private:
    bool firstStart;
    int fps;
    int number_of_enemies;
    bool debug;
    float scale;

    int *map;
    int *tmp_map;
    sf::Vector3<size_t> map_size;

    sf::Vector2f scale_vector;
    sf::RenderWindow *_RenderWindow;

    object player;
    std::vector<object> enemies;

    sf::Clock _Clock;
    sf::Clock _GlobalClock;
    std::vector<sf::Vector2i> leftNeighbours, rightNeighbours;

    sf::Color player_color;
    sf::Color enemy_color;
    sf::Color wall_color;
    sf::Color path_color;
    sf::Color background_color;

    sf::SoundBuffer sb_intro;
    sf::SoundBuffer sb_damaged;
    sf::SoundBuffer sb_captured;
    sf::SoundBuffer sb_dead;

    sf::Sound intro;
    sf::Sound damaged;
    sf::Sound captured;

    sf::Font font;
    int count_walls() const;
    void update_tmp_map();
};
