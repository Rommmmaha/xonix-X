#pragma once
#include "SFML/System.hpp"
class _object : public sf::Vector2i
{
  public:
    int direction;
    sf::RectangleShape shape;
    sf::Vector2i previous = sf::Vector2i(0, 0);
    _object(int x = 0, int y = 0)
    {
        this->x = x;
        this->y = y;
        this->direction = 0;
        this->previous = sf::Vector2i(0, 0);
    }
    virtual void move()
    {
    }
    void updateShape(sf::Vector2f scale)
    {
        this->previous = sf::Vector2i(this->x, this->y);
        this->shape.setPosition(sf::Vector2f(x * scale.x, y * scale.y));
    }
};

class _player : public _object
{
  public:
    using _object::_object;
    void move()
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
};

class _enemy : public _object
{
  public:
    using _object::_object;
    void move()
    {
        if (direction == -1)
            this->direction = rand() % 4;
        switch (direction)
        {
        case -1:
            break;
        case 0:
            --x;
            --y;
            break;
        case 1:
            --x;
            ++y;
            break;
        case 2:
            ++x;
            --y;
            break;
        case 3:
            ++x;
            ++y;
            break;
        }
    }
};
