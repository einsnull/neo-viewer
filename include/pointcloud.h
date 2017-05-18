#ifndef _POINTCLOUD_H_
#define _POINTCLOUD_H_

#include <mutex>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <neo/neo.hpp>
#include <angle.h>

// init distance show on windows
const constexpr auto initDistance = 16 * 100.;

// windows zoom in/out frequency
const sf::Time TimePreFrame = sf::seconds(1.f/20.f);

// PI/180, for degree to radian
const constexpr auto kDegreeToRadian = 0.017453292519943295;


// One circle per angle / distance measurement
struct radian_distance_pointcloud {
    neo::sample scan_data;
    sf::CircleShape point;
};
using PointCloud = std::vector<struct radian_distance_pointcloud>;
using PointCloudMutex = std::mutex;

void buildPointCloud(neo::scan& scan, AngleCircles& circles,
                     PointCloudMutex& pointCloudMutex, PointCloud& pointCloud);
void buildPointCloudWhenPause(AngleCircles& circles,
                              PointCloudMutex& pointCloudMutex,
                              PointCloud& pointCloud);

#include "pointcloud.inc"
#endif // _POINTCLOUD_H_
