//Game.h
#include <iostream>
#include <vector>
#include <optional>
#include <cmath>
#include <random>
#include <string>
#include "Bullet.h"
#include "PauseSystem.h"
#include "Tank.h"

extern bool CreateStartScreen(sf::RenderWindow& window);

enum class GameState {
    MainMenu,
    Playing,
    Paused,
    GameOver
};

class Game {
public:
    Game();

    void startGame();

    void run();

private:
    void processInput();

    // 暂停菜单选择处理
    void handlePauseMenuSelection();

    void update(float dt);

    void render();

    // 新增绘制游戏元素函数
    void drawGameElements();

    // 新增绘制主菜单函数
    void drawMainMenu();

    // 新增绘制暂停菜单函数
    void drawPauseMenu();

    // 新增文本居中辅助函数
    void centerText(sf::Text& text, float windowWidth, float yPos);

    void collideWalls(Tank&);

    bool handleCollision(const Bullet&);

    void spawnExplosion(sf::Vector2f pos);

    void spawnEnemy();

    void resetGame();

    // 敌人AI：朝玩家移动，方向和移动分开
    void updateEnemyAI(Tank& enemy, float dt);

    // 判断敌人是否对准玩家，可以开火
    bool canEnemyFire(const Tank& enemy);

    GameState currentState = GameState::MainMenu;  // 初始状态设为主菜单
    std::vector<std::string> menuOptions = { "Start Game", "Exit" };
    int selectedMenuOption = 0;

    std::vector<std::string> pauseMenuOptions = { "Resume", "Main Menu", "Exit" };
    int selectedPauseOption = 0;

    sf::RenderWindow window;
    PauseSystem pauseSystem;
    Tank player;
    std::vector<Tank> enemies;
    std::vector<Bullet> bullets;
    std::vector<Explosion> explosions;
    std::vector<std::pair<int, int>> walls;
    sf::Font font;
    sf::Text scoreText;
    sf::Text gameOverText;
    float enemyTimer = 0;
    int score = 0;
    bool isGameOver = false;
};

#pragma once
