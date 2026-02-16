#include "tubegrid.hpp"
#include <cmath>

TubeGrid::TubeGrid(int lanes, int segments, float r)
    : numLanes(lanes), numSegments(segments), radius(r)
{
    const float TWO_PI = 6.283185307f;

    for (int s = 0; s < numSegments; ++s)
    {
        for (int l = 0; l < numLanes; ++l)
        {
            float angle = (float)l / numLanes * TWO_PI;

            glm::vec3 normal(
                cos(angle),
                sin(angle),
                0.0f
            );

            glm::vec3 pos = normal * radius;
            pos.z = -s * 1.0f;

            TubeCell cell;
            cell.lane = l;
            cell.segment = s;
            cell.intensity = 0.0f;
            cell.position = pos;
            cell.normal = normal;

            cells.push_back(cell);
        }
    }
}

void TubeGrid::update(float cameraZ, const Obstacle& obstacle)
{
    int alertLane = angleToLane(obstacle.angle);
    

    for (auto& cell : cells)
    {
        // por padrão, apagado
        cell.intensity = 0.0f;

        // só pinta a lane correta
        if (cell.lane != alertLane)
            continue;

        float dz = obstacle.z - cell.position.z;

        if (cell.lane == obstacle.lane)
        {
            // só alerta à frente da câmera
            if (dz >= 0.0f && dz <= 15.0f)
            {
                float t = dz / 15.0f;   // 0 longe → 1 perto
                cell.intensity = t;
            }
            else
            {
                cell.intensity = 0.0f;
            }
        }

    }
}

const std::vector<TubeCell>& TubeGrid::getCells() const
{
    return cells;
}

int TubeGrid::angleToLane(float angle) const
{
    const float TWO_PI = 6.283185307f;

    // normaliza para [0, 2π)
    while (angle < 0)      angle += TWO_PI;
    while (angle >= TWO_PI) angle -= TWO_PI;

    float t = angle / TWO_PI;
    int lane = static_cast<int>(t * numLanes);

    return lane;
}

int TubeGrid::getLanesCount()
{
    return numLanes;
}
