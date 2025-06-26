// Screen.cpp
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <string>

// 创建开始界面函数
bool CreateStartScreen(sf::RenderWindow& window) {
    // 加载字体
    sf::Font font;
    if (!font.openFromFile("C:/Windows/Fonts/arial.ttf")) {
        return false; // 字体加载失败
    }

    // 设置标题文字
    sf::Text title(font, "Tank Battle 3.0", 60);
    title.setFillColor(sf::Color::Yellow);
    title.setStyle(sf::Text::Bold);
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setOrigin({ titleBounds.size.x / 2, titleBounds.size.y / 2 });
    title.setPosition({ window.getSize().x / 2.f, window.getSize().y / 3.f });

    // 设置提示文字
    sf::Text prompt(font, "Press any key to start", 30);
    prompt.setFillColor(sf::Color::White);
    sf::FloatRect promptBounds = prompt.getLocalBounds();
    prompt.setOrigin({ promptBounds.size.x / 2, promptBounds.size.y / 2 });
    prompt.setPosition({ window.getSize().x / 2.f, window.getSize().y * 2.f / 3.f });

    // 设置控制说明
    sf::Text instructions(font,
        "Arrow keys: Move\nSpace: Fire\nR: Restart after Game Over",20);
    instructions.setFillColor(sf::Color(200, 200, 200));
    sf::FloatRect insBounds = instructions.getLocalBounds();
    instructions.setOrigin({ insBounds.size.x / 2, 0 });
    instructions.setPosition({ window.getSize().x / 2.f, window.getSize().y * 0.75f });

    // 动画计时器
    sf::Clock blinkClock;
    bool showPrompt = true;

    // 循环直到按键
    while (window.isOpen()) {
        while (auto event = window.pollEvent()) {
            // 关闭窗口
            if (event->is<sf::Event::Closed>())
                return false;

            // 任意键开始
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
                return true;
        }

        // 闪烁提示文字
        if (blinkClock.getElapsedTime().asSeconds() >= 0.6f) {
            showPrompt = !showPrompt;
            blinkClock.restart();
        }

        // 绘制
        window.clear(sf::Color(30, 30, 30));
        window.draw(title);
        if (showPrompt) window.draw(prompt);
        window.draw(instructions);
        window.display();
    }

    return false; // 如果窗口被关闭
}