#include <SFML/Graphics.hpp>
#include "tank.h"

class Bullet {
public:
    Bullet(sf::Vector2f p, Direction d, bool fromPlayer) :
        position(p), dir(d), fromPlayer(fromPlayer)
    {
        shape.setRadius(4);
        shape.setOrigin({ 4,4 });
        shape.setFillColor(fromPlayer ? sf::Color::Yellow : sf::Color::Red);
        shape.setPosition(position);
    }
    void update(float dt) {
        switch (dir) {
        case Direction::UP: position.y -= BULLET_SPEED * dt; break;
        case Direction::DOWN: position.y += BULLET_SPEED * dt; break;
        case Direction::LEFT: position.x -= BULLET_SPEED * dt; break;
        case Direction::RIGHT: position.x += BULLET_SPEED * dt; break;
        }
        shape.setPosition(position);
    }
    void draw(sf::RenderWindow& w) const { w.draw(shape); }
    bool out() const {
        return position.x < 0 || position.x > MAP_WIDTH * TILE_SIZE || position.y < 0 || position.y > MAP_HEIGHT * TILE_SIZE;
    }
    sf::Vector2f getPos() const { return position; }
    bool isFromPlayer() const { return fromPlayer; }

private:
    sf::CircleShape shape;
    sf::Vector2f position;
    Direction dir;
    bool fromPlayer;
};

#pragma once
