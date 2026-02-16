#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "obstacle.hpp"

struct TubeCell {
    int lane;
    int segment;
    float intensity;
    glm::vec3 position;
    glm::vec3 normal;
};

class TubeGrid {
public:
    TubeGrid(int lanes, int segments, float radius);
    int getLanesCount();
    void update(float cameraZ, const Obstacle& obstacle);
    const std::vector<TubeCell>& getCells() const;
    int angleToLane(float angle) const;

private:
    int numLanes;
    int numSegments;
    float radius;

    std::vector<TubeCell> cells;
};
