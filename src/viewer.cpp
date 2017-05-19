// Make use of the CMake build system or compile manually, e.g. with:
// g++ -std=c++11 viewer.cc -lneo -lsfml-graphics -lsfml-window -lsfml-system

#include <cmath>

#include <cstdlib>
#include <iostream>
#include <mutex>
#include <utility>

// #include <neo/neo.hpp>


#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#include <angle.h>
#include <button.h>
#include <color.h>
#include <pointcloud.h>


int main(int argc, char* argv[]) try {
	if (argc != 2) {
		std::cout << "Usage: ./neo_viewer /dev/ttyACM0\n";
		return EXIT_FAILURE;
	}
	PointCloud pointCloud;
	PointCloudMutex pointCloudMutex;

    // motor shows
    sf::Font motor_font;
    motor_font.loadFromFile("fonts/Sansation.ttf");
    sf::Text motor_text;
    motor_text.setFont(motor_font);
    motor_text.setPosition(650, 5);
    motor_text.setCharacterSize(14);
    motor_text.setColor(sf::Color::Green);

	// Render thread displays the point cloud
    neo::AngleCircles circles(800, 16*100.);
    neo::AngleLines lines(800);
    motor_text.setString("Motor Speed: " + toString(circles.getMotorSpeed()));

	// Now start scanning in the second thread, swapping in new points for every scan
	neo::neo device{argv[1]};

	neo::scan scan;
    std::cout << "Device connect successful" << std::endl;

    sf::Clock clock;
    sf::Time time_since_last_update = sf::Time::Zero;
    int currentSpeed = circles.getMotorSpeed();

    while (circles.windows_.isOpen()) {
        sf::Time elapsed_time = clock.restart();
        time_since_last_update += elapsed_time;

        motor_text.setString("Motor Speed: " + toString(circles.getMotorSpeed()));
        // windows render frequency
        while (time_since_last_update > TimePreFrame) {
            time_since_last_update -= TimePreFrame;
            circles.processEvents();
            circles.windows_.clear(kColorSlateBlue4);
            {
                std::lock_guard<PointCloudMutex> sentry{pointCloudMutex};
                circles.draw();
                lines.draw(circles.windows_);
                circles.windows_.draw(motor_text);
                for (auto point : pointCloud)
                    circles.windows_.draw(point.point);
            }
            circles.windows_.display();
        }
        // deal with press button situation
        if (circles.getButtonStatus() == neo::ButtonStatus::BUTTON_START) { // START running
            if (circles.getStatus() != neo::Status::RUNNING) {
                circles.setStatus(neo::Status::RUNNING);
                device.set_motor_speed(circles.getMotorSpeed());
                device.start_scanning();
            } else {
                if (currentSpeed != circles.getMotorSpeed()) {
                    currentSpeed = circles.getMotorSpeed();
                    device.stop_scanning();
                    device.set_motor_speed(currentSpeed);
                    device.start_scanning();
                }
            }
        } else if (circles.getButtonStatus() == neo::ButtonStatus::BUTTON_PAUSE) {  // PAUSE
            buildPointCloudWhenPause(circles, pointCloudMutex, pointCloud);
            if (circles.getStatus() == neo::Status::PAUSE) continue;
            circles.setStatus(neo::Status::PAUSE);
            device.stop_scanning();
            device.set_motor_speed(0);
            // std::cout << "pause, and keep the pointcloud";
            continue;
        } else if (circles.getButtonStatus() == neo::ButtonStatus::BUTTON_STOP) {  // STOP
            // std::cout << "stop and reset the scanner" << std::endl;
            if (circles.getStatus() == neo::Status::STOP) continue;
            circles.setStatus(neo::Status::STOP);
            pointCloud.clear();
            device.stop_scanning();
            device.set_motor_speed(0);
            continue;
        } else if (circles.getButtonStatus() == neo::ButtonStatus::BUTTON_HELP) {  // HELP
            if (circles.getStatus() == neo::Status::HELP) continue;
            circles.setStatus(neo::Status::HELP);
            device.stop_scanning();
            device.set_motor_speed(0);
            // std::cout << "Open the help window" << std::endl;
            continue;
        }

		scan = device.get_scan();
        buildPointCloud(scan, circles, pointCloudMutex, pointCloud);
    }
	device.stop_scanning();
    device.set_motor_speed(0);

} catch (const neo::device_error& e) {
	std::cerr << "Error: " << e.what() << std::endl;
}
