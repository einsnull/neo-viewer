#ifndef _BUTTON_H_
#define _BUTTON_H_

#include <iostream>
#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

namespace neo
{
    enum Status {
        NOT_RUN,
        RUNNING,
        PAUSE,
        STOP,
    };

    enum ButtonStatus {
        BUTTON_NOT_RUN, // status: not run or stop or pause
        BUTTON_START,
        BUTTON_PAUSE,
        BUTTON_STOP,
        BUTTON_HELP,
        BUTTON_NONE,    // status for running, but status cannot be others.
    };
	class Button
	{
    public:
        Button(const std::string& filename = NULL, const float position_x = 0, const float position_y = 0);
        ~Button();

        void load(const std::string& filename);
        void draw(sf::RenderWindow* windows);

        bool inRange(float a, float b);

        void set(const std::string& filename, const float position_x, const float position_y);

		sf::Texture button_texture_;
        sf::Sprite button_sprite_;



        float x_min_, y_min_;   // range
        float x_max_, y_max_;
	};

    // class Buttons
    // {
    // public:
    //     Buttons();
    //     ~Buttons();

    //     void draw(sf::RenderWindow* windows);

    //     Button start_, pause_, stop_, help_;
    //     sf::RectangleShape outliers_start_;
    //     sf::RectangleShape outliers_pause_;
    //     sf::RectangleShape outliers_stop_;
    //     sf::RectangleShape outliers_help_;
    //     sf::RectangleShape outliers_left_;
    //     sf::RectangleShape outliers_right_;

    // };
} // end namespace neo

#endif // _BUTTON_H_
