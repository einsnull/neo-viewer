// Make use of the CMake build system or compile manually, e.g. with:
// g++ -std=c++11 viewer.cc -lneo -lsfml-graphics -lsfml-window -lsfml-system

#include <cmath>

#include <cstdlib>
#include <iostream>
#include <mutex>
#include <utility>

#include <neo/neo.hpp>


#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#include <angle.h>
#include <button.h>
#include <color.h>

// init distance show on windows
const constexpr auto initDistance = 16 * 100.;


// windows zoom in/out frequency
const sf::Time TimePreFrame = sf::seconds(1.f/20.f);


// One circle per angle / distance measurement
struct radian_distance_pointcloud {
    neo::sample scan_data;
    sf::CircleShape point;
};
using PointCloud = std::vector<struct radian_distance_pointcloud>;
using PointCloudMutex = std::mutex;

// build pointcloud when running
void buildPointCloud(neo::scan *scan, AngleCircles *circles, PointCloudMutex &pointCloudMutex, PointCloud &pointCloud) {
    auto currentDistance = circles->getDistance();
    const auto windowSize = circles->windows_.getSize();
    const auto windowMinSize = std::min(windowSize.x, windowSize.y);
    const constexpr auto kDegreeToRadian = 0.017453292519943295;

    PointCloud localPointCloud;

    for (auto sample : scan->samples) {
        const auto distance = static_cast<double>(sample.distance);

        if (distance > currentDistance)
            continue;
        // unnecessary?

        const auto degree = std::fmod((static_cast<double>(sample.angle) / 1000. + 90.), 360.);
        const auto radian = degree * kDegreeToRadian;

        auto x = std::cos(radian) * distance;
        auto y = std::sin(radian) * distance;

        x = x + currentDistance;
        y = y + currentDistance;
        x = (x / (2 * currentDistance)) * windowMinSize;
        y = (y / (2 * currentDistance)) * windowMinSize;

        sf::CircleShape point{2.0f, 20};
        point.setPosition(x - 1, windowMinSize - y - 1);

        point.setFillColor(sf::Color::White);
        struct radian_distance_pointcloud temp = {sample, point};
        localPointCloud.push_back(std::move(temp));
    }

    // display LiDAR position
    sf::CircleShape point{4.0f, 8};
    point.setPosition(windowSize.x / 2 - 4, windowSize.y / 2 - 4);
    point.setFillColor(sf::Color::Red);
    struct radian_distance_pointcloud temp = {{0, 0, 0}, point};
    localPointCloud.push_back(std::move(temp));
    {
        // Now swap in the new point cloud
        std::lock_guard<PointCloudMutex> sentry{pointCloudMutex};
        pointCloud = std::move(localPointCloud);
    }
}

// build pointcloud when pause, zoom in/out
void buildPointCloudWhenPause(AngleCircles *circles, PointCloudMutex& pointCloudMutex, PointCloud& pointCloud) {
    auto currentDistance = circles->getDistance();
    const auto windowSize = circles->windows_.getSize();
    const auto windowMinSize = std::min(windowSize.x, windowSize.y);
    const constexpr auto kDegreeToRadian = 0.017453292519943295;

    PointCloud localPointCloud;

    for (auto sample : pointCloud) {
        const auto distance = static_cast<double>(sample.scan_data.distance);
        const auto degree = std::fmod((static_cast<double>(sample.scan_data.angle) / 1000. + 90.), 360.);
        const auto radian = degree * kDegreeToRadian;

        auto x = std::cos(radian) * distance;
        auto y = std::sin(radian) * distance;

        x = x + currentDistance;
        y = y + currentDistance;
        x = (x / (2 * currentDistance)) * windowMinSize;
        y = (y / (2 * currentDistance)) * windowMinSize;

        sf::CircleShape point{2.0f, 20};
        point.setPosition(x - 1, windowMinSize - y - 1);

        point.setFillColor(sf::Color::White);
        struct radian_distance_pointcloud temp = {sample.scan_data, point};
        localPointCloud.push_back(std::move(temp));
    }

    sf::CircleShape point{4.0f, 8};
    point.setPosition(windowSize.x / 2 - 4, windowSize.y / 2 - 4);
    point.setFillColor(sf::Color::Red);
    struct radian_distance_pointcloud temp = {{0, 0, 0}, point};
    localPointCloud.push_back(std::move(temp));
    {
        // Now swap in the new point cloud
        std::lock_guard<PointCloudMutex> sentry{pointCloudMutex};
        pointCloud = std::move(localPointCloud);
    }
}
int main(int argc, char* argv[]) try {
	if (argc != 2) {
		std::cout << "Usage: ./neo_viewer /dev/ttyACM0\n";
		return EXIT_FAILURE;
	}
	PointCloud pointCloud;
	PointCloudMutex pointCloudMutex;

	// Render thread displays the point cloud
	AngleCircles circles(800, 16*100.);
	AngleLines lines(800);
	const auto worker = [&](sf::RenderWindow* window) {
		sf::Clock clock;
		sf::Time time_since_last_update = sf::Time::Zero;
		while (window->isOpen()) {
			sf::Time elapsed_time = clock.restart();
			time_since_last_update += elapsed_time;

			while (time_since_last_update > TimePreFrame) {
				time_since_last_update -= TimePreFrame;
				circles.processEvents();
				window->clear(kColorSlateBlue4);
				{
					std::lock_guard<PointCloudMutex> sentry{pointCloudMutex};
					circles.draw();
					lines.draw(window);
					for (auto point : pointCloud)
						window->draw(point.point);
				}
				window->display();
			}


		}
	};

	sf::Thread thread(worker, &circles.windows_);
	thread.launch();

	// Now start scanning in the second thread, swapping in new points for every scan
	neo::neo device{argv[1]};

	neo::scan scan;
    std::cout << "Device connect successful" << std::endl;

	while (circles.windows_.isOpen()) {
        // deal with press button situation
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
            buildPointCloudWhenPause(&circles, pointCloudMutex, pointCloud);
            continue;
        }
        // difference between BUTTON_NOT_RUN and BUTTON_NONE
        // BUTTON_NOT_RUN: the scanning stopped.
        // BUTTON_NONE: the scanner running and send data.

		scan = device.get_scan();
        buildPointCloud(&scan, &circles, pointCloudMutex, pointCloud);
	}

	device.stop_scanning();

} catch (const neo::device_error& e) {
	std::cerr << "Error: " << e.what() << std::endl;
}
