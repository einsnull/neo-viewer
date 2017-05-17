// Make use of the CMake build system or compile manually, e.g. with:
// g++ -std=c++11 viewer.cc -lneo -lsfml-graphics -lsfml-window -lsfml-system

#include <cmath>

#include <cstdlib>
#include <iostream>
#include <mutex>
#include <utility>

#include <neo/neo.hpp>

#include <angle.h>
#include <button.h>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

// Zoom into 5x5 meter area
const constexpr auto initDistance = 16 * 100.;

// Use cream for the background and denim for points
static const sf::Color kColorRed{255, 0, 0};
static const sf::Color kColorCream{250, 240, 230};
static const sf::Color kColorDenim{80, 102, 127};
static const sf::Color kColorBlack{0, 0, 0};
static const sf::Color kColorWhite{255, 255, 255};
// color for background
static const sf::Color kColorSlateBlue{106, 90, 205};
static const sf::Color kColorMidnightBlue{25, 25, 112};
static const sf::Color kColorDarkSlateBlue{72, 61, 139};
static const sf::Color kColorBlue4{0, 0, 255};
static const sf::Color kColorSlateBlue4{15, 16, 64};

// windows zoom in/out frequency
const sf::Time TimePreFrame = sf::seconds(1.f/20.f);


// One circle per angle / distance measurement
using PointCloud = std::vector<sf::CircleShape>;
using PointCloudMutex = std::mutex;

int main(int argc, char* argv[]) try {
	if (argc != 2) {
		std::cout << "Usage: ./neo_viewer /dev/ttyACM0\n";
		return EXIT_FAILURE;
	}

	// sf::ContextSettings settings;
	// settings.antialiasingLevel = 8;
	// sf::RenderWindow window(sf::VideoMode(800, 800), "Micvision Neo LiDAR", sf::Style::Default, settings);

	// window.setFramerateLimit(30);
	// window.setActive(false); // activated on render thread

	PointCloud pointCloud;
	PointCloudMutex pointCloudMutex;
	// sf::Texture mTexture;
	// sf::Sprite mPlayer;
	// mTexture.loadFromFile("../images/logo.jpg");
	// mPlayer.setTexture(mTexture);
	// mPlayer.setPosition(0.0f, 0.0f);

	// Render thread displays the point cloud
	AngleCircles circles(800, 16*100.);
	AngleLines lines(800);
    // neo::Buttons button;
	const auto worker = [&](sf::RenderWindow* window) {
		sf::Clock clock;
		sf::Time time_since_last_update = sf::Time::Zero;
		while (window->isOpen()) {
			//sf::Event event;

			//while (window->pollEvent(event)) {
			//if (event.type == sf::Event::Closed)
				//window->close();
				//else if (event.type == sf::Event::KeyPressed)
				//if (event.key.code == sf::Keyboard::Escape)
					//window->close();
			//}
			sf::Time elapsed_time = clock.restart();
			time_since_last_update += elapsed_time;

			while (time_since_last_update > TimePreFrame) {
				time_since_last_update -= TimePreFrame;
				circles.processEvents();
				//circles.processPress();
				window->clear(kColorSlateBlue4);
				{
					std::lock_guard<PointCloudMutex> sentry{pointCloudMutex};

					// window->draw(mPlayer);
					circles.draw();
					lines.draw(window);
                    // button.draw(window);
					for (auto point : pointCloud)
						window->draw(point);
				}

				window->display();
			}


		}
	};

	sf::Thread thread(worker, &circles.windows_);
	thread.launch();

	// Now start scanning in the second thread, swapping in new points for every scan
	neo::neo device{argv[1]};
	// device.set_motor_speed(5);
	// device.start_scanning();

	neo::scan scan;
    std::cout << "Device connect successful" << std::endl;

	while (circles.windows_.isOpen()) {
        if (circles.getButtonStatus() == neo::ButtonStatus::BUTTON_START) {
            static bool first_start = true;
            // std::cout << "start running" << std::endl;
            circles.setButtonStatus(neo::ButtonStatus::BUTTON_NONE);
            if (first_start) {
                device.set_motor_speed(5);
                device.start_scanning();
                first_start = false;
            } else
                device.start_scanning();
        } else if (circles.getButtonStatus() == neo::ButtonStatus::BUTTON_PAUSE) {
            // std::cout << "pause, and keep the pointcloud";
            circles.setButtonStatus(neo::ButtonStatus::BUTTON_NOT_RUN);
            continue;
        } else if (circles.getButtonStatus() == neo::ButtonStatus::BUTTON_STOP) {
            // std::cout << "stop and reset the scanner" << std::endl;
            circles.setButtonStatus(neo::ButtonStatus::BUTTON_NOT_RUN);
            pointCloud.clear();
            continue;
        } else if (circles.getButtonStatus() == neo::ButtonStatus::BUTTON_HELP) {
            // std::cout << "Open the help window" << std::endl;
            circles.setButtonStatus(neo::ButtonStatus::BUTTON_NOT_RUN);
            continue;
        } else if (circles.getButtonStatus() == neo::ButtonStatus::BUTTON_NOT_RUN) {
            // std::cout << "Not_run" << std::endl;
            continue;
        }

        // difference between BUTTON_NOT_RUN and BUTTON_NONE
        // BUTTON_NOT_RUN: the scanning stopped.
        // BUTTON_NONE: the scanner running and send data.

		scan = device.get_scan();
		auto currentDistance = circles.getDistance();
        // std::cout << "currentDistance: " << currentDistance << std::endl;

		// 40m max radius => display as 80m x 80m square
		const auto windowSize = circles.windows_.getSize();
		const auto windowMinSize = std::min(windowSize.x, windowSize.y);

		PointCloud localPointCloud;

		for (auto sample : scan.samples) {
			const constexpr auto kDegreeToRadian = 0.017453292519943295;

			const auto distance = static_cast<double>(sample.distance);

			// Discard samples above our we zoomed-in view box
			if (distance > currentDistance)
				continue;

			// From milli degree to degree and adjust to device orientation
			const auto degree = std::fmod((static_cast<double>(sample.angle) / 1000. + 90.), 360.);
			const auto radian = degree * kDegreeToRadian;

			// From angle / distance to to Cartesian
			auto x = std::cos(radian) * distance;
			auto y = std::sin(radian) * distance;

			// Make positive
			x = x + currentDistance;
			y = y + currentDistance;

			// Scale to window size
			x = (x / (2 * currentDistance)) * windowMinSize;
			y = (y / (2 * currentDistance)) * windowMinSize;

			sf::CircleShape point{2.0f, 20};
			point.setPosition(x-1, windowMinSize - y-1);

			// Base transparency on signal strength
			auto color = kColorRed;
			color.a = sample.signal_strength;
			point.setFillColor(sf::Color::White);

			localPointCloud.push_back(std::move(point));
		}

		// display LiDAR position
		sf::CircleShape point{4.0f, 8};
		point.setPosition(windowSize.x / 2-4, windowSize.y / 2-4);
		point.setFillColor(sf::Color::Red);
		localPointCloud.push_back(std::move(point));

		{
			// Now swap in the new point cloud
			std::lock_guard<PointCloudMutex> sentry{pointCloudMutex};
			pointCloud = std::move(localPointCloud);
		}
	}

	device.stop_scanning();

} catch (const neo::device_error& e) {
	std::cerr << "Error: " << e.what() << std::endl;
}
