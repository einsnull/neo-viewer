#include <button.h>

namespace neo
{
    Button::Button(const std::string& filename,
                   const float position_x,
                   const float position_y) {

        load(filename);
        button_sprite_.setTexture(button_texture_);
        button_sprite_.setPosition(position_x, position_y);
    }

    Button::~Button() {}

    void Button::load(const std::string& filename) {
        button_texture_.loadFromFile(filename);
    }

    // void Button::set(const std::string& filename, float position_x)

    void Button::draw(sf::RenderWindow* windows) {
        windows->draw(button_sprite_);
    }

    Buttons::Buttons() :
        start_("../images/start_28.png",  6.f, 6.f),
        pause_("../images/pause_28.png", 37.f, 6.f),
        stop_("../images/stop_28.png",   68.f, 6.f),
        help_("../images/help_28.png",   108.f, 6.f) {
        outliers_start_.setSize(sf::Vector2f(30, 30));
        outliers_start_.setPosition(5, 5);
        outliers_start_.setFillColor(sf::Color::White);

        outliers_pause_.setSize(sf::Vector2f(30, 30));
        outliers_pause_.setPosition(36, 5);
        outliers_pause_.setFillColor(sf::Color::White);

        outliers_stop_.setSize(sf::Vector2f(30, 30));
        outliers_stop_.setPosition(67, 5);
        outliers_stop_.setFillColor(sf::Color::White);

        outliers_help_.setSize(sf::Vector2f(30, 30));
        outliers_help_.setPosition(107, 5);
        outliers_help_.setFillColor(sf::Color::White);

        outliers_left_.setSize(sf::Vector2f(92, 30));
        outliers_left_.setPosition(5, 5);
        outliers_left_.setFillColor(sf::Color::Black);
        outliers_left_.setOutlineThickness(5);
        outliers_left_.setOutlineColor(sf::Color::Black);

        outliers_right_.setSize(sf::Vector2f(30, 30));
        outliers_right_.setPosition(107, 5);
        outliers_right_.setFillColor(sf::Color::Black);
        outliers_right_.setOutlineThickness(5);
        outliers_right_.setOutlineColor(sf::Color::Black);
    }

    Buttons::~Buttons() {}

    void Buttons::draw(sf::RenderWindow* windows) {
        windows->draw(outliers_left_);
        windows->draw(outliers_right_);
        windows->draw(outliers_start_);
        windows->draw(outliers_pause_);
        windows->draw(outliers_stop_);
        windows->draw(outliers_help_);

        start_.draw(windows);
        pause_.draw(windows);
        stop_.draw(windows);
        help_.draw(windows);
    }
}
