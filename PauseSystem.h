#pragma once
// PauseSystem.h
#pragma once
#include <SFML/Graphics.hpp>

class PauseSystem {
public:
    PauseSystem();
    void toggle();  // �л���ͣ״̬
	void handleInput(const sf::Event& event);  // ���������¼�
    bool isPaused() const;
    void render(sf::RenderWindow& window) const;
private:
    bool paused;
    sf::Text pauseText;
    sf::Font font;
};
