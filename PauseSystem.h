#pragma once
// PauseSystem.h
#pragma once
#include <SFML/Graphics.hpp>

class PauseSystem {
public:
    PauseSystem();
    void toggle();  // 切换暂停状态
	void handleInput(const sf::Event& event);  // 处理输入事件
    bool isPaused() const;
    void render(sf::RenderWindow& window) const;
private:
    bool paused;
    sf::Text pauseText;
    sf::Font font;
};
