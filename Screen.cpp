// Screen.cpp
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <string>

// ������ʼ���溯��
bool CreateStartScreen(sf::RenderWindow& window) {
    // ��������
    sf::Font font;
    if (!font.openFromFile("C:/Windows/Fonts/arial.ttf")) {
        return false; // �������ʧ��
    }

    // ���ñ�������
    sf::Text title(font, "Tank Battle 3.0", 60);
    title.setFillColor(sf::Color::Yellow);
    title.setStyle(sf::Text::Bold);
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setOrigin({ titleBounds.size.x / 2, titleBounds.size.y / 2 });
    title.setPosition({ window.getSize().x / 2.f, window.getSize().y / 3.f });

    // ������ʾ����
    sf::Text prompt(font, "Press any key to start", 30);
    prompt.setFillColor(sf::Color::White);
    sf::FloatRect promptBounds = prompt.getLocalBounds();
    prompt.setOrigin({ promptBounds.size.x / 2, promptBounds.size.y / 2 });
    prompt.setPosition({ window.getSize().x / 2.f, window.getSize().y * 2.f / 3.f });

    // ���ÿ���˵��
    sf::Text instructions(font,
        "Arrow keys: Move\nSpace: Fire\nR: Restart after Game Over",20);
    instructions.setFillColor(sf::Color(200, 200, 200));
    sf::FloatRect insBounds = instructions.getLocalBounds();
    instructions.setOrigin({ insBounds.size.x / 2, 0 });
    instructions.setPosition({ window.getSize().x / 2.f, window.getSize().y * 0.75f });

    // ������ʱ��
    sf::Clock blinkClock;
    bool showPrompt = true;

    // ѭ��ֱ������
    while (window.isOpen()) {
        while (auto event = window.pollEvent()) {
            // �رմ���
            if (event->is<sf::Event::Closed>())
                return false;

            // �������ʼ
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
                return true;
        }

        // ��˸��ʾ����
        if (blinkClock.getElapsedTime().asSeconds() >= 0.6f) {
            showPrompt = !showPrompt;
            blinkClock.restart();
        }

        // ����
        window.clear(sf::Color(30, 30, 30));
        window.draw(title);
        if (showPrompt) window.draw(prompt);
        window.draw(instructions);
        window.display();
    }

    return false; // ������ڱ��ر�
}