#pragma once
#include "SFML/System.hpp"
class object : public sf::Vector2i
{
  public:
    sf::Vector2i previous = sf::Vector2i(0, 0);
    int direction;
    int type; // 1 - player, 2 - enemy
    sf::RectangleShape shape = sf::RectangleShape(sf::Vector2f(10, 10));
    object()
    {
        this->x = 0;
        this->y = 0;
        int direction = 0;
        this->type = 0;
    }
    object(int type, int x = 0, int y = 0)
    {
        this->type = type;
        this->x = x;
        this->y = y;
        if (this->type == 2)
            this->direction = rand() % 4;
    }
    void move()
    {
        float step = 10;
        if (type == 1) // player movement
        {
            switch (direction)
            {
            case sf::Keyboard::Space:
                break;
            case sf::Keyboard::Up:
                --y;
                break;
            case sf::Keyboard::Down:
                ++y;
                break;
            case sf::Keyboard::Left:
                --x;
                break;
            case sf::Keyboard::Right:
                ++x;
                break;
            }
        }
        if (type == 2) // enemy movement
        {
            switch (direction)
            {
            case 0: // UL
                --x;
                --y;
                break;
            case 1: // DL
                --x;
                ++y;
                break;
            case 2: // UR
                ++x;
                --y;
                break;
            case 3: // DR
                ++x;
                ++y;
                break;
            }
        }
    }
    void updateShape()
    {
        this->previous = sf::Vector2i(this->x, this->y);
        this->shape.setPosition(sf::Vector2f(x * 10.0f, y * 10.0f));
    }
};
