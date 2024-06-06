#pragma once
#include "SFML/System.hpp"
class object : public sf::Vector2i
{
  public:
    int type; // 1 - player, 2 - enemy
    int direction;
    sf::RectangleShape shape;
    sf::Vector2i previous = sf::Vector2i(0, 0);
    object()
    {
        this->x = 0;
        this->y = 0;
        this->type = 0;
        this->direction = 0;
        this->previous = sf::Vector2i(0, 0);
    }
    object(int type, int x = 0, int y = 0)
    {
        this->type = type;
        this->x = x;
        this->y = y;
        this->direction = 0;
        this->previous = sf::Vector2i(0, 0);
        if (this->type == 2)
            this->direction = rand() % 4;
    }
    void move()
    {
        if (type == 1) // player movement
        {
            switch (direction)
            {
            case -1:
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
            case -1:
                break;
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
    void updateShape(sf::Vector2f scale)
    {
        this->previous = sf::Vector2i(this->x, this->y);
        this->shape.setPosition(sf::Vector2f(x * scale.x, y * scale.y));
    }
};
