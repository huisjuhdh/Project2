//Game.cpp
#include "Game.h"

Game::Game() :
    window(sf::VideoMode({ MAP_WIDTH * TILE_SIZE, MAP_HEIGHT * TILE_SIZE }), "Tank Battle 3.0"),
    player({ MAP_WIDTH * TILE_SIZE / 2.f, MAP_HEIGHT * TILE_SIZE / 2.f }, sf::Color::Green, Direction::UP),
    scoreText(font, "Score: 0", 24),
    gameOverText(font, "Game Over!\nPress R to Restart", 40),
    currentState(GameState::MainMenu) // 初始状态设为主菜单
{
    window.setFramerateLimit(60);

    if (!CreateStartScreen(window)) {
        window.close();
    }
    // 墙体边界
    for (int i = 0; i < MAP_WIDTH; i++) {
        walls.emplace_back(i, 0);
        walls.emplace_back(i, MAP_HEIGHT - 1);
    }
    for (int j = 1; j < MAP_HEIGHT - 1; j++) {
        walls.emplace_back(0, j);
        walls.emplace_back(MAP_WIDTH - 1, j);
    }
    // 墙体障碍
    walls.emplace_back(5, 5);
    walls.emplace_back(12, 8);
    walls.emplace_back(18, 10);

    if (!font.openFromFile("C:/Windows/Fonts/arial.ttf")) {
        throw std::runtime_error("Failed to load font");
    }
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition({ 10, 10 });

    gameOverText.setFillColor(sf::Color::Red);
    gameOverText.setPosition({ MAP_WIDTH * TILE_SIZE / 2.f - 150, MAP_HEIGHT * TILE_SIZE / 2.f - 50 });
}

void Game::startGame() {
	player = Tank({ MAP_WIDTH * TILE_SIZE / 2.f, MAP_HEIGHT * TILE_SIZE / 2.f }, sf::Color::Green, Direction::UP);
	enemies.clear();
	bullets.clear();
	explosions.clear();
	score = 0;
    spawnEnemy();
	isGameOver = false;
	// 生成初始敌人
	/*for (int i = 0; i < 3; ++i) {
		spawnEnemy();
	}*/
    
	currentState = GameState::Playing;
}

void Game::run() {
    sf::Clock clk;

    while (window.isOpen()) {
        float dt = clk.restart().asSeconds();

        processInput();

        if (currentState == GameState::Playing && !pauseSystem.isPaused())
            update(dt);

        render();
    }
}

void Game::processInput() {
    while (auto ev = window.pollEvent()) {

        if (ev->is<sf::Event::Closed>()) window.close();

        // 主菜单输入处理
        if (currentState == GameState::MainMenu) {
            if (auto kp = ev->getIf<sf::Event::KeyPressed>()) {
                switch (kp->code) {
                case sf::Keyboard::Key::Up:
                    selectedMenuOption = (selectedMenuOption - 1 + menuOptions.size()) % menuOptions.size();
                    break;
                case sf::Keyboard::Key::Down:
                    selectedMenuOption = (selectedMenuOption + 1) % menuOptions.size();
                    break;
                case sf::Keyboard::Key::Enter:
                    if (selectedMenuOption == 0) startGame();
                    else window.close();
                    break;
                }
            }
            return;
        }

        // 游戏进行中输入处理
        if (currentState == GameState::Playing) {
            // 处理玩家输入
            if (auto kp = ev->getIf<sf::Event::KeyPressed>()) {
                switch (kp->code) {
                case sf::Keyboard::Key::Up:
                    if (!isGameOver) {
                        player.setDirection(Direction::UP);
                        player.setSpeed(PLAYER_SPEED);
                    }
                    break;
                case sf::Keyboard::Key::Down:
                    if (!isGameOver) {
                        player.setDirection(Direction::DOWN);
                        player.setSpeed(PLAYER_SPEED);
                    }
                    break;
                case sf::Keyboard::Key::Left:
                    if (!isGameOver) {
                        player.setDirection(Direction::LEFT);
                        player.setSpeed(PLAYER_SPEED);
                    }
                    break;
                case sf::Keyboard::Key::Right:
                    if (!isGameOver) {
                        player.setDirection(Direction::RIGHT);
                        player.setSpeed(PLAYER_SPEED);
                    }
                    break;
                case sf::Keyboard::Key::Space:
                    if (!isGameOver) {
                        if (auto p = player.tryFire()) {
                            bullets.emplace_back(*p, player.getDirection(), true);
                        }
                    }
                    break;
                case sf::Keyboard::Key::R:
                    if (isGameOver) {
                        resetGame();
                        isGameOver = false;
                    }
                    break;
                }
            }
            if (auto kr = ev->getIf<sf::Event::KeyReleased>()) {
                if (kr->code == sf::Keyboard::Key::Up || kr->code == sf::Keyboard::Key::Down ||
                    kr->code == sf::Keyboard::Key::Left || kr->code == sf::Keyboard::Key::Right)
                {
                    player.setSpeed(0);
                }
            }
            pauseSystem.handleInput(*ev);
			// 切换暂停状态
			currentState = pauseSystem.isPaused() ? GameState::Paused : GameState::Playing;

            return;
        }

        // 暂停菜单输入处理
        if (currentState == GameState::Paused) {
            if (auto kp = ev->getIf<sf::Event::KeyPressed>()) {
                switch (kp->code) {
                case sf::Keyboard::Key::Up:
                    selectedPauseOption = (selectedPauseOption - 1 + pauseMenuOptions.size()) % pauseMenuOptions.size();
                    break;
                case sf::Keyboard::Key::Down:
                    selectedPauseOption = (selectedPauseOption + 1) % pauseMenuOptions.size();
                    break;
                case sf::Keyboard::Key::Enter:
                    handlePauseMenuSelection();
                    break;
                case sf::Keyboard::Key::P:
                    pauseSystem.toggle();
                    currentState = GameState::Playing;
                    break;
                }
            }
            return;
        }

    }
}

void Game::handlePauseMenuSelection() {
    switch (selectedPauseOption) {
    case 0: // 继续游戏
        pauseSystem.toggle();
        currentState = GameState::Playing;
        break;
    case 1: // 返回主菜单
        currentState = GameState::MainMenu;
        resetGame();
        break;
    case 2: // 退出游戏
        window.close();
        break;
    }
}

void Game::update(float dt) {
    if (isGameOver) return;

    player.update(dt);
    collideWalls(player);

    enemyTimer += dt;
    for (auto& e : enemies) {
        updateEnemyAI(e, dt);
        collideWalls(e);
    }

    if (enemyTimer >= 1.5f) {
        enemyTimer = 0;
        for (auto& e : enemies) {
            if (canEnemyFire(e)) {
                bullets.emplace_back(e.getBarrelTip(), e.getDirection(), false);
            }
        }
    }

    for (size_t i = 0; i < bullets.size();) {
        auto& b = bullets[i];
        b.update(dt);
        if (b.out() || handleCollision(b)) {
            bullets.erase(bullets.begin() + i);
        }
        else ++i;
    }

    for (auto it = explosions.begin(); it != explosions.end(); ) {
        it->timer += dt;
        float r = (it->timer / EXPLOSION_DURATION) * TILE_SIZE;
        it->shape.setRadius(r);
        it->shape.setOrigin({ r, r });
        if (it->timer >= EXPLOSION_DURATION) {
            it = explosions.erase(it);
        }
        else ++it;
    }
}

void Game::render() {
    window.clear(sf::Color(30, 30, 30));

    if (currentState == GameState::MainMenu) {
        // 绘制主菜单
        drawMainMenu();
    }
    else if (currentState == GameState::Playing&&!isGameOver) {
        // 绘制游戏元素
        drawGameElements();
    }
    else if (currentState == GameState::Paused) {
        // 绘制游戏画面和暂停菜单
        drawGameElements();
        //pauseSystem.render(window);
        drawPauseMenu();
    }
    else if (isGameOver) {
        //std::cout << "Game Over! Press R to Restart" << std::endl;
        drawGameElements();
        window.draw(gameOverText);
    }

    window.display();
}

void Game::drawGameElements() {
    // 绘制墙体
    sf::RectangleShape sq({ float(TILE_SIZE), float(TILE_SIZE) });
    sq.setFillColor(sf::Color::Blue);
    for (auto& w : walls) {
        sq.setPosition({ float(w.first * TILE_SIZE), float(w.second * TILE_SIZE) });
        window.draw(sq);
    }

    // 绘制其他游戏元素
    player.draw(window);
    for (auto& e : enemies) e.draw(window);
    for (auto& b : bullets) b.draw(window);
    for (auto& ex : explosions) window.draw(ex.shape);

    // 绘制分数
    scoreText.setString("Score: " + std::to_string(score));
    window.draw(scoreText);
}

void Game::drawMainMenu() {
    // 标题
    sf::Text title(font, "Tank Battle 3.0", 60);
    title.setFillColor(sf::Color::Yellow);
    title.setStyle(sf::Text::Bold);
    centerText(title, window.getSize().x, window.getSize().y / 3.0f);
    window.draw(title);

    // 菜单选项
    sf::Text menuText(font, "", 40);
    //menuText.setFont(font);
    //menuText.setCharacterSize(40);
    menuText.setStyle(sf::Text::Bold);

    for (size_t i = 0; i < menuOptions.size(); ++i) {
        menuText.setString(menuOptions[i]);
        menuText.setFillColor(selectedMenuOption == i ? sf::Color::Yellow : sf::Color::White);
        centerText(menuText, window.getSize().x, window.getSize().y / 2.0f + i * 60);
        window.draw(menuText);
    }
}

void Game::drawPauseMenu() {
    // 半透明遮罩
    sf::RectangleShape overlay(sf::Vector2f(window.getSize()));
    overlay.setFillColor(sf::Color(0, 0, 0, 150));
    window.draw(overlay);

    // 暂停菜单文本
    sf::Text menuText(font, "", 30);
    //menuText.setFont(font);
    //menuText.setCharacterSize(30);
    menuText.setStyle(sf::Text::Bold);

    for (size_t i = 0; i < pauseMenuOptions.size(); ++i) {
        menuText.setString(pauseMenuOptions[i]);
        menuText.setFillColor(selectedPauseOption == i ? sf::Color::Yellow : sf::Color::White);
        centerText(menuText, window.getSize().x, window.getSize().y / 2.0f + i * 40);
        window.draw(menuText);
    }
}

void Game::centerText(sf::Text& text, float windowWidth, float yPos) {
    sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin({ bounds.position.x + bounds.size.x / 2.0f, bounds.position.y + bounds.size.y / 2.0f });
    text.setPosition({ windowWidth / 2.0f, yPos });
}

void Game::collideWalls(Tank& t) {
    auto p = t.getPosition();
    int gx = int(p.x / TILE_SIZE), gy = int(p.y / TILE_SIZE);
    for (auto& w : walls) {
        if (w.first == gx && w.second == gy) {
            // 简单回退
            sf::Vector2f back{ p };
            switch (t.getDirection()) {
            case Direction::UP: back.y += PLAYER_SPEED * 0.02f; break;
            case Direction::DOWN: back.y -= PLAYER_SPEED * 0.02f; break;
            case Direction::LEFT: back.x += PLAYER_SPEED * 0.02f; break;
            case Direction::RIGHT: back.x -= PLAYER_SPEED * 0.02f; break;
            }
            t = Tank(back, sf::Color::Green, t.getDirection());
            break;
        }
    }
}

bool Game::handleCollision(const Bullet& b) {
    int gx = int(b.getPos().x / TILE_SIZE), gy = int(b.getPos().y / TILE_SIZE);
    for (auto& w : walls) {
        if (w.first == gx && w.second == gy) {
            spawnExplosion(b.getPos());
            return true;
        }
    }

    if (b.isFromPlayer()) {
        for (size_t i = 0; i < enemies.size(); ++i) {
            auto epos = enemies[i].getPosition();
            float dx = b.getPos().x - epos.x, dy = b.getPos().y - epos.y;
            if (std::hypot(dx, dy) < TILE_SIZE / 2.f) {
                spawnExplosion(epos);
                enemies.erase(enemies.begin() + i);
                score += 100;
                spawnEnemy();
                return true;
            }
        }
    }
    else {
        auto ppos = player.getPosition();
        float dx = b.getPos().x - ppos.x, dy = b.getPos().y - ppos.y;
        if (std::hypot(dx, dy) < TILE_SIZE / 2.f) {
            spawnExplosion(ppos);
            isGameOver = true;
            //      std::cout << "Game Over!" << std::endl;  // 添加调试信息
            return true;
        }
    }
    return false;
}

void Game::spawnExplosion(sf::Vector2f pos) {
    Explosion ex;
    ex.shape.setPosition(pos);
    ex.shape.setFillColor(sf::Color::Red);
    ex.timer = 0;
    ex.shape.setRadius(0);
    ex.shape.setOrigin({ 0, 0 });
    explosions.push_back(ex);
}

void Game::spawnEnemy() {
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> dx(1, MAP_WIDTH - 2), dy(1, MAP_HEIGHT - 2);
    int x, y; bool ok = false;
    while (!ok) {
        x = dx(gen); y = dy(gen);
        ok = true;
        for (auto& w : walls) if (w.first == x && w.second == y) { ok = false; break; }
    }
    enemies.emplace_back(sf::Vector2f(x * TILE_SIZE + TILE_SIZE / 2.f, y * TILE_SIZE + TILE_SIZE / 2.f),
        sf::Color::Red, static_cast<Direction>(std::rand() % 4));
}

void Game::resetGame() {
    player = Tank({ MAP_WIDTH * TILE_SIZE / 2.f, MAP_HEIGHT * TILE_SIZE / 2.f }, sf::Color::Green, Direction::UP);
    enemies.clear();
    bullets.clear();
    explosions.clear();
    isGameOver = false;
    currentState = GameState::MainMenu;  // 重置为初始状态
    score = 0;
    spawnEnemy();
}

void Game::updateEnemyAI(Tank& enemy, float dt) {
    sf::Vector2f ppos = player.getPosition();
    sf::Vector2f epos = enemy.getPosition();

    float dx = ppos.x - epos.x;
    float dy = ppos.y - epos.y;

    Direction desiredDir;

    if (std::abs(dx) > std::abs(dy)) {
        desiredDir = dx > 0 ? Direction::RIGHT : Direction::LEFT;
    }
    else {
        desiredDir = dy > 0 ? Direction::DOWN : Direction::UP;
    }

    if (enemy.getDirection() != desiredDir) {
        enemy.setDirection(desiredDir);
        enemy.setSpeed(0);
    }
    else {
        enemy.setSpeed(PLAYER_SPEED * 0.6f);
    }
    enemy.update(dt);
}

bool Game::canEnemyFire(const Tank& enemy) {
    sf::Vector2f ppos = player.getPosition();
    sf::Vector2f epos = enemy.getPosition();

    Direction d = enemy.getDirection();

    switch (d) {
    case Direction::UP:
        return std::abs(epos.x - ppos.x) < TILE_SIZE / 2 && ppos.y < epos.y;
    case Direction::DOWN:
        return std::abs(epos.x - ppos.x) < TILE_SIZE / 2 && ppos.y > epos.y;
    case Direction::LEFT:
        return std::abs(epos.y - ppos.y) < TILE_SIZE / 2 && ppos.x < epos.x;
    case Direction::RIGHT:
        return std::abs(epos.y - ppos.y) < TILE_SIZE / 2 && ppos.x > epos.x;
    }
    return false;
}