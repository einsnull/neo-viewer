
// build pointcloud when running
void buildPointCloud(neo::scan& scan, AngleCircles& circles,
                     PointCloudMutex &pointCloudMutex,
                     PointCloud &pointCloud) {
    auto currentDistance = circles.getDistance();
    const auto windowSize = circles.windows_.getSize();
    const auto windowMinSize = std::min(windowSize.x, windowSize.y);

    PointCloud localPointCloud;

    for (auto sample : scan.samples) {
        const auto distance = static_cast<double>(sample.distance);

        if (distance > currentDistance)
            continue;
        // unnecessary?

        const auto degree =
            std::fmod((static_cast<double>(sample.angle) / 1000. + 90.), 360.);
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
void buildPointCloudWhenPause(AngleCircles& circles,
                              PointCloudMutex& pointCloudMutex,
                              PointCloud& pointCloud) {
    auto currentDistance = circles.getDistance();
    const auto windowSize = circles.windows_.getSize();
    const auto windowMinSize = std::min(windowSize.x, windowSize.y);

    PointCloud localPointCloud;

    for (auto sample : pointCloud) {
        const auto distance = static_cast<double>(sample.scan_data.distance);
        const auto degree =
            std::fmod((static_cast<double>(sample.scan_data.angle) / 1000. + 90.), 360.);
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
