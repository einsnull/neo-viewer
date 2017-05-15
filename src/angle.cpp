#include <angle.h>

template <typename T>
std::string toString(const T& value) {
    std::stringstream stream;
    stream << value;
    return stream.str();
}

AngleCircles::AngleCircles(int pixel, float distance) :
    windows_size_(pixel),
    distance_(distance),
    zoom_in_(false),
    zoom_out_(false) {
    big_circle_radius_pixel_ = windows_size_ / 2 - 50;

    small_circle_radius_pixel_ = big_circle_radius_pixel_ / 2;

    update(distance_);

    // big circle radius
    big_circle_.setRadius(big_circle_radius_pixel_);
    // big circle position
    big_circle_.setPosition(50, 50);
    // big circle pointcloud number
    big_circle_.setPointCount(200);
    // big circle fill color
    big_circle_.setFillColor(sf::Color::Transparent);
    // big circle outlier color
    big_circle_.setOutlineColor(sf::Color::Red);
    // big circle outlier thickness
    big_circle_.setOutlineThickness(1.0f);

    small_circle_.setRadius(small_circle_radius_pixel_);
    small_circle_.setPosition(50 + small_circle_radius_pixel_, 50 + small_circle_radius_pixel_);
    small_circle_.setPointCount(100);
    small_circle_.setFillColor(sf::Color::Transparent);
    small_circle_.setOutlineColor(sf::Color::Red);
    small_circle_.setOutlineThickness(1.0f);

    text_font_.loadFromFile("../fonts/Sansation.ttf");
    big_up_.setFont(text_font_);
    big_up_.setPosition(windows_size_/2 + 2 , 32);
    big_up_.setCharacterSize(12);

    small_up_.setFont(text_font_);
    small_up_.setPosition(windows_size_/2 + 2, 32 + small_circle_radius_pixel_);
    small_up_.setCharacterSize(12);

    big_left_.setFont(text_font_);
    big_left_.setPosition(55, windows_size_/2 - 18);
    big_left_.setCharacterSize(12);

    small_left_.setFont(text_font_);
    small_left_.setPosition(windows_size_/2 - small_circle_radius_pixel_ + 2, windows_size_/2 - 18);
    small_left_.setCharacterSize(12);

    center_.setFont(text_font_);
    center_.setPosition(windows_size_/2 + 5, windows_size_/2 - 18);
    center_.setCharacterSize(12);

    small_right_.setFont(text_font_);
    small_right_.setPosition(windows_size_ / 2 + small_circle_radius_pixel_ + 5,
                            windows_size_/2 - 18);
    small_right_.setCharacterSize(12);

    big_right_.setFont(text_font_);
    big_right_.setPosition(windows_size_ / 2 + big_circle_radius_pixel_ + 5,
                            windows_size_/2 - 18);
    big_right_.setCharacterSize(12);

    small_bottom_.setFont(text_font_);
    small_bottom_.setPosition(windows_size_/2 + 2, windows_size_/2 + small_circle_radius_pixel_ - 18);
    small_bottom_.setCharacterSize(12);

    big_bottom_.setFont(text_font_);
    big_bottom_.setPosition(windows_size_/2 + 2, windows_size_/2 + big_circle_radius_pixel_ - 18);
    big_bottom_.setCharacterSize(12);

    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    windows_.create(sf::VideoMode(pixel, pixel), "Micvision Neo LiDAR demo", sf::Style::Default, settings);
    windows_.setFramerateLimit(60);
    windows_.setActive(false);
}

AngleCircles::~AngleCircles() {}

void AngleCircles::update(float distance) {
    big_circle_radius_ = distance;
    small_circle_radius_ = big_circle_radius_ / 2;
}

const float AngleCircles::getBigCircleRadius() {
    return big_circle_radius_;
}

const float AngleCircles::getSmallCircleRadius() {
    return small_circle_radius_;
}

const float AngleCircles::getDistance() {
    return distance_;
}

void AngleCircles::setCircleRadius(float radius) {
    // TODO:
    update(radius);
    distance_ = radius * (windows_size_ / 2) /big_circle_radius_pixel_;
}

void AngleCircles::processEvents() {
    sf::Event event;
    while (windows_.pollEvent(event)) {
        switch (event.type) {
        case sf::Event::KeyPressed:
            handleInput(event.key.code, true); break;

        case sf::Event::KeyReleased:
            handleInput(event.key.code, false); break;

        case sf::Event::Closed:
            windows_.close(); break;
        }
    }
}

void AngleCircles::handleInput(sf::Keyboard::Key key, bool isPressed) {
    if (key == sf::Keyboard::Up)
        zoom_out_ = isPressed;
    else if (key == sf::Keyboard::Down)
        zoom_in_ = isPressed;
}

void AngleCircles::processPress() {
    float radius;
    if (zoom_out_ && big_circle_radius_ < 4000) {
        radius = big_circle_radius_ + 100;
        setCircleRadius(radius);
    } else if (zoom_in_ && big_circle_radius_ > 500) {
        radius = big_circle_radius_ - 100;
        setCircleRadius(radius);
    }

}

const void AngleCircles::drawBigCircle() {
    windows_.draw(big_circle_);
}

const void AngleCircles::drawSmallCircle() {
    windows_.draw(small_circle_);
}

const void AngleCircles::draw() {
    drawBigCircle();
    drawSmallCircle();
    showMarks();
}

const void AngleCircles::showMarks() {
    big_up_.setString(toString(big_circle_radius_/100));
    windows_.draw(big_up_);

    small_up_.setString(toString(small_circle_radius_/100));
    windows_.draw(small_up_);

    big_left_.setString(toString(big_circle_radius_/100));
    windows_.draw(big_left_);

    small_left_.setString(toString(small_circle_radius_/100));
    windows_.draw(small_left_);

    center_.setString("0 m");
    windows_.draw(center_);

    small_right_.setString(toString(small_circle_radius_/100));
    windows_.draw(small_right_);

    big_right_.setString(toString(big_circle_radius_/100));
    windows_.draw(big_right_);

    small_bottom_.setString(toString(small_circle_radius_/100));
    windows_.draw(small_bottom_);

    big_bottom_.setString(toString(big_circle_radius_/100));
    windows_.draw(big_bottom_);
}

AngleLines::AngleLines(int pixel) : windows_size_(pixel) {
    vertical_line_.setSize(sf::Vector2f(windows_size_ - 50, 1));
    vertical_line_.setRotation(90);
    vertical_line_.setPosition(windows_size_/2, 25);
    vertical_line_.setFillColor(sf::Color::Red);


    horizontal_line_.setSize(sf::Vector2f(windows_size_ - 50, 1));
    horizontal_line_.setPosition(25, windows_size_/2);
    horizontal_line_.setFillColor(sf::Color::Red);
}

AngleLines::~AngleLines() {}



const void AngleLines::drawVerticalLine(sf::RenderWindow* windows) {
    windows->draw(vertical_line_);
}

const void AngleLines::drawHorizontalLine(sf::RenderWindow* windows) {
    windows->draw(horizontal_line_);
}

const void AngleLines::draw(sf::RenderWindow* windows) {
    drawVerticalLine(windows);
    drawHorizontalLine(windows);
}
