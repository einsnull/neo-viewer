#ifndef _BUTTON_H_
#define _BUTTON_H_

#include <iostream>
#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

namespace neo
{
	class Button
	{
    public:
        Button(const std::string& filename = NULL, const float position_x = 0, const float position_y = 0);
        ~Button();

        void load(const std::string& filename);
        void draw(sf::RenderWindow* windows);

		sf::Texture button_texture_;
        sf::Sprite button_sprite_;

        //float x_, y_;   // location
	};

    class Buttons
    {
    public:
        Buttons();
        ~Buttons();

        void draw(sf::RenderWindow* windows);

        Button start_, pause_, stop_, help_;
        sf::RectangleShape outliers_start_;
        sf::RectangleShape outliers_pause_;
        sf::RectangleShape outliers_stop_;
        sf::RectangleShape outliers_help_;
        sf::RectangleShape outliers_left_;
        sf::RectangleShape outliers_right_;

    };
} // end namespace neo

#endif // _BUTTON_H_
