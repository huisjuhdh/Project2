#include <SFML/Graphics.hpp>

#define TILE_SIZE  32
#define MAP_WIDTH  25
#define MAP_HEIGHT  18
#define PLAYER_SPEED  120.f  // 像素/秒
#define BULLET_SPEED  300.f   // 像素/秒
#define FIRE_COOLDOWN  0.5f    // 秒
#define EXPLOSION_DURATION  0.5f

enum class Direction { UP, RIGHT, DOWN, LEFT };

struct Explosion {
    sf::CircleShape shape;
    float timer = 0.f;
};

class Tank {
public:
    Tank(sf::Vector2f pos, sf::Color col, Direction dir) :
        position(pos), color(col), direction(dir), speed(0), fireTimer(0)
    {
        // 底盘长条形
        body.setSize({ float(TILE_SIZE), float(TILE_SIZE) * 0.6f });
        body.setOrigin({ TILE_SIZE / 2.f, (TILE_SIZE * 0.6f) / 2.f });
        body.setFillColor(color);

        // 炮管细长条形，左端为旋转中心
        barrel.setSize({ float(TILE_SIZE) * 0.6f, float(TILE_SIZE) * 0.15f });
        barrel.setOrigin({ 0, barrel.getSize().y / 2.f });
        barrel.setFillColor(sf::Color(80, 80, 80)); // 深灰色

        setDirection(dir);
        updatePosition();
    }

    void setDirection(Direction d) {
        direction = d;
        float rot = 0.f;
        switch (d) {
        case Direction::UP: rot = 0.f; break;
        case Direction::RIGHT: rot = 90.f; break;
        case Direction::DOWN: rot = 180.f; break;
        case Direction::LEFT: rot = 270.f; break;
        }
        sf::Angle rotation = sf::degrees(rot);
        body.setRotation(rotation);
        barrel.setRotation(rotation);
    }

    void setSpeed(float s) { speed = s; }

    sf::Vector2f getPosition() const { return position; }
    Direction getDirection() const { return direction; }

    // 返回炮管末端坐标，子弹从这里发射
    sf::Vector2f getBarrelTip() const {
        sf::Angle angle = barrel.getRotation();
        float angleRad = angle.asDegrees();
        return position + sf::Vector2f(std::cos(angleRad), std::sin(angleRad)) * barrel.getSize().x;
    }

    // 是否可以开火（冷却时间判断）
    std::optional<sf::Vector2f> tryFire() {
        if (fireTimer >= FIRE_COOLDOWN) {
            fireTimer = 0;
            return getBarrelTip();
        }
        return {};
    }

    void update(float dt) {
        sf::Vector2f d{};
        switch (direction) {
        case Direction::UP: d.y = -speed * dt; break;
        case Direction::DOWN: d.y = speed * dt; break;
        case Direction::LEFT: d.x = -speed * dt; break;
        case Direction::RIGHT: d.x = speed * dt; break;
        }
        position += d;
        fireTimer += dt;
        updatePosition();
    }

    void draw(sf::RenderWindow& w) const {
        w.draw(body);
        w.draw(barrel);
    }

private:
    void updatePosition() {
        body.setPosition(position);
        barrel.setPosition(position);
    }

    sf::RectangleShape body;   // 坦克底盘
    sf::RectangleShape barrel; // 炮管
    sf::Vector2f position;
    sf::Color color;
    Direction direction;
    float speed, fireTimer;
};

#pragma once
