#ifndef _ANGLE_H_
#define _ANGLE_H_

#include <sstream>
#include <iostream>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

class AngleCircles {
public:
    AngleCircles(int pixel, float distance);
    ~AngleCircles();

    void update(float distance);
    const float getBigCircleRadius();          // return distance(cm)
    const float getSmallCircleRadius();        // return distance(cm)
    const float getDistance();

    void setCircleRadius(float distance);   // set var distance

    void processEvents();

    void handleInput(sf::Keyboard::Key key, bool isPressed);
    void processPress();

    const void drawBigCircle();
    const void drawSmallCircle();
    const void draw();

    const void showMarks();

private:
    int big_circle_radius_pixel_;   // big circle radius in pixel;
    float big_circle_radius_;       // big circle radius in centimeter;
    sf::CircleShape big_circle_;

    int small_circle_radius_pixel_; // small circle radius in pixel;
    float small_circle_radius_;     // small circle radius in meter;
    sf::CircleShape small_circle_;

    int windows_size_;              // maximum distance in pixel
    float distance_;                // the maximum distance the windows can display.
                                    // range: [5-50] meter

    // the text show on windows;
    sf::Text                     big_up_,

                                small_up_,

        big_left_, small_left_, center_, small_right_, big_right_,

                              small_bottom_,

                               big_bottom_;

    sf::Font text_font_;

    bool zoom_in_, /* Down */ zoom_out_ /* Up */;
public:
    sf::RenderWindow windows_;
};

class AngleLines {
public:
    AngleLines(int pixel);
    ~AngleLines();

    const void drawVerticalLine(sf::RenderWindow* windows);
    const void drawHorizontalLine(sf::RenderWindow* windows);
    const void draw(sf::RenderWindow* windows);

private:
    int windows_size_;
    // line vertical
    sf::RectangleShape vertical_line_;
    // line horizontal
    sf::RectangleShape horizontal_line_;
};

#endif // _ANGLE_H_
