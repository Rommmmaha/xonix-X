#pragma once
#include "SFML/System.hpp"
#include <vector>
// min max
template <typename T> T min(T a, T b)
{
    return a < b ? a : b;
}
template <typename T> T max(T a, T b)
{
    return a > b ? a : b;
}

// Vector2 functions
template <typename T, typename U> sf::Vector2<T> V2_convert(sf::Vector2<U> a)
{
    return sf::Vector2<T>(a.x, a.y);
}

template <typename T> sf::Vector2<T> V2_invert(sf::Vector2<T> a)
{
    return sf::Vector2<T>(-a.x, -a.y);
}

//
size_t pos2index(size_t x, size_t y, size_t max_x)
{
    return x + y * max_x;
}

sf::Color HSV2RGB(float fH, float fV, float fS)
{
    float fR = 0.0, fG = 0.0, fB = 0.0;
    float fC = fV * fS;
    float fHPrime = float(fmod(fH / 60.0, 6));
    float fX = fC * float(1 - fabs(fmod(fHPrime, 2) - 1));
    float fM = fV - fC;
    if (0 <= fHPrime && fHPrime < 1)
    {
        fR = fC;
        fG = fX;
        fB = 0;
    }
    else if (1 <= fHPrime && fHPrime < 2)
    {
        fR = fX;
        fG = fC;
        fB = 0;
    }
    else if (2 <= fHPrime && fHPrime < 3)
    {
        fR = 0;
        fG = fC;
        fB = fX;
    }
    else if (3 <= fHPrime && fHPrime < 4)
    {
        fR = 0;
        fG = fX;
        fB = fC;
    }
    else if (4 <= fHPrime && fHPrime < 5)
    {
        fR = fX;
        fG = 0;
        fB = fC;
    }
    else if (5 <= fHPrime && fHPrime < 6)
    {
        fR = fC;
        fG = 0;
        fB = fX;
    }
    else
    {
        fR = 0;
        fG = 0;
        fB = 0;
    }
    fR += fM;
    fG += fM;
    fB += fM;
    return sf::Color(sf::Uint8(fR * 255), sf::Uint8(fG * 255), sf::Uint8(fB * 255));
}

void getNeighbours(int *map, sf::Vector2i pos, sf::Vector3<size_t> map_size, int type)
{
    if (pos.x < 0 || pos.y < 0 || pos.x >= map_size.x || pos.y >= map_size.y)
    {
        return;
    }
    if (map[pos2index(pos.x, pos.y, map_size.x)] != 0)
    {
        return;
    }
    map[pos2index(pos.x, pos.y, map_size.x)] = type; // scanned
    getNeighbours(map, sf::Vector2i(pos.x - 1, pos.y), map_size, type);
    getNeighbours(map, sf::Vector2i(pos.x + 1, pos.y), map_size, type);
    getNeighbours(map, sf::Vector2i(pos.x, pos.y - 1), map_size, type);
    getNeighbours(map, sf::Vector2i(pos.x, pos.y + 1), map_size, type);
}
