// PauseSystem.cpp
#include "PauseSystem.h"
#include "tank.h"

PauseSystem::PauseSystem() : pauseText(font, "PAUSED", 40) , paused(false) {
    if (!font.openFromFile("C:/Windows/Fonts/arial.ttf")) {
        return;
        // 错误处理
    }
    // 设置位置...
	pauseText.setFillColor(sf::Color::Black);
	pauseText.setStyle(sf::Text::Bold);
    sf::FloatRect titleBounds = pauseText.getLocalBounds();
    pauseText.setOrigin({ titleBounds.size.x / 2, titleBounds.size.y / 2 });
 
}

void PauseSystem::toggle() {
    paused = !paused;
}

void PauseSystem::handleInput(const sf::Event& event) {
    if (const auto* keyEvent = event.getIf<sf::Event::KeyPressed>()) {
        if (keyEvent->code == sf::Keyboard::Key::P) {
            toggle();
        }
    }
}

bool PauseSystem::isPaused() const {
    return paused;
}

void PauseSystem::render(sf::RenderWindow& window) const {  
   if (!paused) return;  

   // 添加半透明背景  
   sf::RectangleShape overlay(sf::Vector2f(window.getSize()));  
   overlay.setFillColor(sf::Color(0, 0, 0, 150));  
   window.draw(overlay);  

   // 居中显示暂停文本  
   sf::Text tempPauseText = pauseText; // 创建一个可修改的副本  
   tempPauseText.setPosition({ window.getSize().x / 2.f, window.getSize().y / 2.f });  
   window.draw(tempPauseText);  
}