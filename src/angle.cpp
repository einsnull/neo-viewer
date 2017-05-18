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
    zoom_out_(false),
    start_("images/start_28.png",  6.f, 6.f),
    pause_("images/pause_28.png", 37.f, 6.f),
    stop_("images/stop_28.png",   68.f, 6.f),
    help_("images/help_28.png",   108.f, 6.f),
    status_(neo::Status::NOT_RUN),
    button_status_(neo::ButtonStatus::BUTTON_NOT_RUN),
    help_message_("images/logo.jpg", 100.f, 100.f),
    show_help_(false) {

    big_circle_radius_pixel_ = windows_size_ / 2 - 50;

    radius_ = big_circle_radius_pixel_ / 4;


    update(distance_);

    setCircleProperty(big_circle_, big_circle_radius_pixel_,
                      50.f, 50.f, 200,
                      sf::Color::Transparent,
                      sf::Color::Green,
                      1.0f);

    setCircleProperty(big_circle_1_, 3 * radius_,
                      50.f + radius_, 50.f + radius_, 100,
                      sf::Color::Transparent,
                      sf::Color::Green,
                      1.0f);

    setCircleProperty(small_circle_, 2 * radius_,
                      50 + 2 * radius_, 50 + 2 * radius_,
                      100,
                      sf::Color::Transparent,
                      sf::Color::Green,
                      1.0f);

    setCircleProperty(small_circle_1_, radius_,
                      50 + 3 * radius_, 50 + 3 * radius_,
                      100,
                      sf::Color::Transparent,
                      sf::Color::Green,
                      1.0f);

    text_font_.loadFromFile("fonts/Sansation.ttf");
    setTextProperty(big_up_,
                    float(windows_size_/2 + 2), 32);
    setTextProperty(small_up_,
                    float(windows_size_/2 + 2), 32 + 2 * radius_);
    setTextProperty(big_left_,
                    55, float(windows_size_/2 - 18));
    setTextProperty(small_left_,
                    float(windows_size_/2 - 2 * radius_ + 2),
                    float(windows_size_/2 - 18));
    setTextProperty(center_,
                    float(windows_size_/2 + 5), float(windows_size_/2 - 18));
    setTextProperty(small_right_,
                    float(windows_size_/2 + 2 * radius_ + 5),
                    float(windows_size_/2 - 18));
    setTextProperty(big_right_,
                    float(windows_size_/2 + big_circle_radius_pixel_ + 5),
                    float(windows_size_/2 - 18));
    setTextProperty(small_bottom_,
                    float(windows_size_/2 + 2),
                    float(windows_size_/2 + 2 * radius_ - 18));
    setTextProperty(big_bottom_,
                    float(windows_size_/2 + 2),
                    float(windows_size_/2 + big_circle_radius_pixel_ - 18));

    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    windows_.create(sf::VideoMode(pixel, pixel), "Micvision Neo LiDAR demo", sf::Style::Default, settings);
    windows_.setFramerateLimit(60);
    windows_.setActive(false);

    // the windows icon and outlier rectangle
    setButtonPreperty(outliers_start_,
                      5.f, 5.f);
    setButtonPreperty(outliers_pause_,
                      36.f, 5.f);
    setButtonPreperty(outliers_stop_,
                      67.f, 5.f);
    setButtonPreperty(outliers_help_,
                      107.f, 5.f);

    setButtonPreperty(outliers_left_,
                      5.f, 5.f,
                      sf::Vector2f(92, 30),
                      sf::Color::Black);
    outliers_left_.setOutlineThickness(5);
    outliers_left_.setOutlineColor(sf::Color::Black);

    setButtonPreperty(outliers_right_,
                      107.f, 5.f,
                      sf::Vector2f(30, 30),
                      sf::Color::Black);
    outliers_right_.setOutlineThickness(5);
    outliers_right_.setOutlineColor(sf::Color::Black);
}


AngleCircles::~AngleCircles() {}

void AngleCircles::update(float distance) {
    big_circle_radius_ = distance;
}

float AngleCircles::getBigCircleRadius() const {
    return big_circle_radius_;
}

// float AngleCircles::getSmallCircleRadius() const {
//     return small_circle_radius_;
// }

float AngleCircles::getDistance() const {
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

        case sf::Event::MouseWheelMoved:
            if (event.mouseWheel.delta > 0)
                processWheel(false);
            else if (event.mouseWheel.delta < 0)
                processWheel(true);
            break;

        case sf::Event::MouseButtonPressed:
            processMouseButton(true); break;

        case sf::Event::MouseButtonReleased:
            processMouseButton(false); break;

        case sf::Event::Closed:
            windows_.close(); break;

        default:
            break;
        }
        processPress();

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
    } else if (zoom_in_ && big_circle_radius_ > 100) {
        radius = big_circle_radius_ - 100;
        setCircleRadius(radius);
    }

}

void AngleCircles::processWheel(bool flag) {
    float radius;
    if (big_circle_radius_ < 4000 && flag) {
        radius = big_circle_radius_ + 100;
        setCircleRadius(radius);
    } else if (big_circle_radius_ > 100 && !flag) {
        radius = big_circle_radius_ - 100;
        setCircleRadius(radius);
    }
}

void AngleCircles::processMouseButton(bool isPressed) {
    float mouse_x = sf::Mouse::getPosition(windows_).x,
        mouse_y = sf::Mouse::getPosition(windows_).y;

    // for define where did we press the button
    static bool range_start = false,
        range_pause = false,
        range_stop = false,
        range_help = false;

    if (isPressed) {
        if (start_.inRange(mouse_x, mouse_y)) {
            start_.set("images/start_20.png", 10, 10);
            range_start = true;
        } else if (pause_.inRange(mouse_x, mouse_y)) {
            pause_.set("images/pause_20.png", 41, 10);
            range_pause = true;
        } else if (stop_.inRange(mouse_x, mouse_y)) {
            stop_.set("images/stop_20.png", 72, 10);
            range_stop = true;
        } else if (help_.inRange(mouse_x, mouse_y)) {
            help_.set("images/help_20.png", 112, 10);
            range_help = true;
        }
    } else {

        if (range_start) {
            start_.set("images/start_28.png", 6, 6);
            range_start = false;
            if (status_ == neo::Status::STOP || status_ == neo::Status::PAUSE ||
                status_ == neo::Status::NOT_RUN || status_ == neo::Status::HELP) {
                status_ = neo::Status::RUNNING;
                button_status_ = neo::ButtonStatus::BUTTON_START;
                // std::cout << "start status" << button_status_ << std::endl;
            }
        } else if (range_pause) {
            pause_.set("images/pause_28.png", 37, 6);
            range_pause = false;
            if( status_ == neo::Status::RUNNING) {
                status_ = neo::Status::PAUSE;
                button_status_ = neo::ButtonStatus::BUTTON_PAUSE;
                // std::cout << "pause status" << button_status_ << std::endl;
            }
        } else if (range_stop){
            stop_.set("images/stop_28.png", 68, 6);
            range_stop = false;
            if (status_ == neo::Status::RUNNING ||
                status_ == neo::Status::PAUSE ||
                status_ == neo::Status::HELP) {
                if (status_ == neo::Status::HELP && show_help_ == true)
                    show_help_ = false;
                status_ = neo::Status::STOP;
                button_status_ = neo::ButtonStatus::BUTTON_STOP;
                // std::cout << "stop status" << button_status_ << std::endl;
            }
        } else if (range_help) {
            button_status_ = neo::ButtonStatus::BUTTON_HELP;
            status_ = neo::Status::HELP;
            help_.set("images/help_28.png", 108, 6);
            range_help = false;
            show_help_ = !show_help_;
            // std::cout << "help Status" << button_status_ << std::endl;
        } else {
            // button_status_ = neo::ButtonStatus::BUTTON_NONE;
            // std::cout << "else status" << button_status_ << std::endl;
        }
    }

}

void AngleCircles::setButtonStatus(neo::ButtonStatus status) {
    button_status_ = status;
}

neo::ButtonStatus AngleCircles::getButtonStatus() const {
    return button_status_;
}

neo::Status AngleCircles::getStatus() const {
    return status_;
}

void AngleCircles::drawBigCircle() {
    windows_.draw(big_circle_);
}

void AngleCircles::drawSmallCircle() {
    windows_.draw(small_circle_);
}

void AngleCircles::draw() {
    drawBigCircle();
    windows_.draw(big_circle_1_);
    drawSmallCircle();
    windows_.draw(small_circle_1_);
    showMarks();

    // icon outline rectangle
    windows_.draw(outliers_left_);
    windows_.draw(outliers_right_);
    windows_.draw(outliers_start_);
    windows_.draw(outliers_pause_);
    windows_.draw(outliers_stop_);
    windows_.draw(outliers_help_);

    // icon button
    start_.draw(&windows_);
    pause_.draw(&windows_);
    stop_.draw(&windows_);
    help_.draw(&windows_);

    if (show_help_)
        help_message_.draw(&windows_);
}

void AngleCircles::showMarks() {
    big_up_.setString(toString(big_circle_radius_/100));
    windows_.draw(big_up_);

    small_up_.setString(toString(big_circle_radius_/200));
    windows_.draw(small_up_);

    big_left_.setString(toString(big_circle_radius_/100));
    windows_.draw(big_left_);

    small_left_.setString(toString(big_circle_radius_/200));
    windows_.draw(small_left_);

    center_.setString("0 m");
    windows_.draw(center_);

    small_right_.setString(toString(big_circle_radius_/200));
    windows_.draw(small_right_);

    big_right_.setString(toString(big_circle_radius_/100));
    windows_.draw(big_right_);

    small_bottom_.setString(toString(big_circle_radius_/200));
    windows_.draw(small_bottom_);

    big_bottom_.setString(toString(big_circle_radius_/100));
    windows_.draw(big_bottom_);
}

void AngleCircles::setCircleProperty(sf::CircleShape& circle, int radius,
                                     float position_x, float position_y,
                                     int point_num,
                                     sf::Color fillColor,
                                     sf::Color outlineColor,
                                     float thickness) {
    // circle radius
    circle.setRadius(radius);
    // circle position
    circle.setPosition(position_x, position_y);
    // circle pointcloud number
    circle.setPointCount(point_num);
    // circle fill color
    circle.setFillColor(fillColor);
    // circle outlier color
    circle.setOutlineColor(outlineColor);
    // circle outlier thickness
    circle.setOutlineThickness(thickness);
}

void AngleCircles::setTextProperty(sf::Text& text,
                                   float position_x, float position_y,
                                   int characterSize,
                                   sf::Color characterColor) {
    text.setFont(text_font_);
    text.setPosition(position_x, position_y);
    text.setCharacterSize(characterSize);
    text.setColor(characterColor);
}

void AngleCircles::setButtonPreperty(sf::RectangleShape& button_outliers,
                                     float position_x, float position_y,
                                     sf::Vector2f size,
                                     sf::Color color) {
    button_outliers.setSize(size);
    button_outliers.setPosition(position_x, position_y);
    button_outliers.setFillColor(color);
}


AngleLines::AngleLines(int pixel) : windows_size_(pixel) {
    vertical_line_.setSize(sf::Vector2f(windows_size_ - 50, 1));
    vertical_line_.setRotation(90);
    vertical_line_.setPosition(windows_size_/2, 25);
    vertical_line_.setFillColor(sf::Color::Green);


    horizontal_line_.setSize(sf::Vector2f(windows_size_ - 50, 1));
    horizontal_line_.setPosition(25, windows_size_/2);
    horizontal_line_.setFillColor(sf::Color::Green);
}

AngleLines::~AngleLines() {}



void AngleLines::drawVerticalLine(sf::RenderWindow& windows) {
    windows.draw(vertical_line_);
}

void AngleLines::drawHorizontalLine(sf::RenderWindow& windows) {
    windows.draw(horizontal_line_);
}

void AngleLines::draw(sf::RenderWindow& windows) {
    drawVerticalLine(windows);
    drawHorizontalLine(windows);
}
