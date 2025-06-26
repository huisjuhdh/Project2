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

    // ��ͣ�˵�ѡ����
    void handlePauseMenuSelection();

    void update(float dt);

    void render();

    // ����������ϷԪ�غ���
    void drawGameElements();

    // �����������˵�����
    void drawMainMenu();

    // ����������ͣ�˵�����
    void drawPauseMenu();

    // �����ı����и�������
    void centerText(sf::Text& text, float windowWidth, float yPos);

    void collideWalls(Tank&);

    bool handleCollision(const Bullet&);

    void spawnExplosion(sf::Vector2f pos);

    void spawnEnemy();

    void resetGame();

    // ����AI��������ƶ���������ƶ��ֿ�
    void updateEnemyAI(Tank& enemy, float dt);

    // �жϵ����Ƿ��׼��ң����Կ���
    bool canEnemyFire(const Tank& enemy);

    GameState currentState = GameState::MainMenu;  // ��ʼ״̬��Ϊ���˵�
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
