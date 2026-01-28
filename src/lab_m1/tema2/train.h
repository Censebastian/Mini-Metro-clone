#pragma once 

#include "lab_m1/tema2/path.h"

namespace m1
{
    class Train
    {
     public:
        Train(Path path, std::vector<std::vector<int>>* buildingTileMap,
            int verticesPerUnit, float verticesDistance)
            : path(path), buildingTileMap(buildingTileMap)
        {
            this->tileLength = verticesPerUnit * verticesDistance;
            toBeRemoved = false;
        }

        ~Train()
        {
        }

        void startTrip()
        {
            if (end == 0) // first run
            {
                end = 1;

                pathIdx = 0;
                tileCoords = glm::ivec2(path.path[pathIdx].first, path.path[pathIdx].second);
                nextTileCoords = glm::ivec2(path.path[pathIdx + end].first, path.path[pathIdx + end].second);

                coordinates = glm::vec2((tileCoords.x + 0.5f) * tileLength, (tileCoords.y + 0.5f) * tileLength);
            }
            else
            {
                end *= -1;
                nextTileCoords = glm::ivec2(path.path[pathIdx + end].first, path.path[pathIdx + end].second);
            }
            dir = nextTileCoords - tileCoords;
            prevDir = glm::ivec2(2, 2);
            rotating = false;
            moving = true;
            waiting = false;
            waitingTime = 0.0f;

            passengers = std::vector<int>(3, 0);
            visitedStations.clear();
            visitedStations.push_back(path.path[pathIdx]);
        }   

        void startTrip(Path path)
        {
            this->path = path;
            this->startTrip();
        }   

        void update(float deltaTimeSeconds, float timeMultiplier)
        {
            nearCenter = false;
            updateTileCoords();
            if (!rotating) // linear movement
            {
                float step = speed * deltaTimeSeconds * timeMultiplier;
                coordinates = glm::vec2(coordinates.x + step * dir.x, coordinates.y + step * dir.y);
            }
            else // circular movement
            {
                if (subtractAngle) {
                    angle -= angularVelocity * deltaTimeSeconds * timeMultiplier;
                } else {
                    angle += angularVelocity * deltaTimeSeconds * timeMultiplier;
                }

                if (angle >= endAngle && !subtractAngle)
                {
                    angle = endAngle;
                    rotating = false;
                }
                if (angle <= endAngle && subtractAngle)
                {
                    angle = endAngle;
                    rotating = false;
                }

                coordinates = pivotPoint + glm::vec2(glm::cos(angle), glm::sin(angle)) * (tileLength / 2);
            }
            updateOrientation();

            endX = tileCoords.x * tileLength + tileLength / 2;
            endZ = tileCoords.y * tileLength + tileLength / 2;
            tolerance = tileLength / 9;

            if (coordinates.x <= endX + tolerance && coordinates.x >= endX - tolerance && 
                coordinates.y <= endZ + tolerance && coordinates.y >= endZ - tolerance)
            {
                nearCenter = true;
            }

            if ((pathIdx == path.path.size() - 1 && end == 1) || (pathIdx == 0 && end == -1))
            {
                if (nearCenter)
                {
                    moving = false;
                    coordinates.x = endX;
                    coordinates.y = endZ;
                    return;
                }
            }
        }

        void updateTileCoords()
        {
            int tileX = coordinates.x / tileLength;
            int tileZ = coordinates.y / tileLength;

            // std::cout << "\ntileX tileZ " << tileX << " " << tileZ << std::endl;

            if (tileCoords != glm::ivec2(tileX, tileZ))
            {
                prevDir = dir;
                pathIdx += end;
                if (pathIdx < 0)
                {
                    pathIdx = 0;
                }
                if (pathIdx >= path.path.size())
                {
                    pathIdx = path.path.size() - 1;
                }

                tileCoords = glm::ivec2(path.path[pathIdx].first, path.path[pathIdx].second);
                if ((pathIdx == path.path.size() - 1 && end == 1) ||
                    (pathIdx == 0 && end == -1))
                {
                    dir = prevDir;
                }
                else
                {
                    nextTileCoords = glm::ivec2(path.path[pathIdx + end].first, path.path[pathIdx + end].second);
                    dir = nextTileCoords - tileCoords;
                }

                if (dir == prevDir || dir == glm::ivec2(prevDir.x * -1, prevDir.y * -1) || prevDir == glm::ivec2(2, 2))
                {
                    rotating = false;
                }
                else
                {
                    rotating = true;
                    glm::vec2 tileCenter = glm::vec2((tileCoords.x + 0.5f) * tileLength, 
                                            (tileCoords.y + 0.5f) * tileLength);

                    pivotPoint = tileCenter + glm::vec2(dir.x, dir.y) * (tileLength / 2.0f) 
                                    - glm::vec2(prevDir.x, prevDir.y) * (tileLength / 2.0f);
                    calculateAngleInterval();
                }
            }
        }

        void calculateAngleInterval()
        {
            if (dir.x != 0)
            {
                if (dir.x == 1)
                {
                    startAngle = glm::pi<float>();
                    if (prevDir.y == 1)
                    {
                        endAngle = glm::pi<float>() / 2;
                        subtractAngle = true;
                    }
                    if (prevDir.y == -1)
                    {
                        endAngle = glm::pi<float>() * 3 / 2;
                        subtractAngle = false;
                    }
                }
                if (dir.x == -1)
                {
                    startAngle = 0.0f;
                    if (prevDir.y == 1)
                    {
                        endAngle = glm::pi<float>() / 2;
                        subtractAngle = false;
                    }
                    if (prevDir.y == -1)
                    {
                        endAngle = -glm::pi<float>() / 2;
                        subtractAngle = true;
                    }
                }
            }
            else if (dir.y != 0)
            {
                if (dir.y == 1)
                {
                    startAngle = glm::pi<float>() * 3 / 2;
                    if (prevDir.x == 1)
                    {
                        endAngle = glm::pi<float>() * 2;
                        subtractAngle = false;
                    }
                    if (prevDir.x == -1)
                    {
                        endAngle = glm::pi<float>();
                        subtractAngle = true;
                    }
                }
                if (dir.y == -1)
                {
                    startAngle = glm::pi<float>() / 2;
                    if (prevDir.x == 1)
                    {
                        endAngle = 0.0f;
                        subtractAngle = true;
                    }
                    if (prevDir.x == -1)
                    {
                        endAngle = glm::pi<float>();
                        subtractAngle = false;
                    }
                }
            }
            angle = startAngle;
        }

        void updateOrientation()
        {
            if (rotating)
            {
                orientation = angle;
            }
            else
            {
                if (dir.x != 0)
                {
                    if (dir.x == 1)
                    {
                        orientation = 0.0f;
                    }
                    if (dir.x == -1)
                    {
                        orientation = glm::pi<float>();
                    }
                }
                if (dir.y != 0)
                {
                    if (dir.y == 1)
                    {
                        orientation = glm::pi<float>() * 3 / 2;
                    }
                    if (dir.y == -1)
                    {
                        orientation = glm::pi<float>() / 2;
                    }
                }
            }
        }

        void printData()
        {
            std::cout << "Train data: \n";
            std::cout << "coordinates.x " << coordinates.x;
            std::cout << ", coordinates.y " << coordinates.y;
            std::cout << "\ntileCoords.x " << tileCoords.x;
            std::cout << ", tileCoords.y " << tileCoords.y;
            std::cout << "\npath.size() " << path.path.size();
            std::cout << ", pathIdx " << pathIdx;
            std::cout << "\nmoving " << moving << std::endl;
        }

        Path path;
        std::vector<std::vector<int>> *buildingTileMap;
        glm::vec2 coordinates;
        glm::vec2 pivotPoint;
        glm::ivec2 tileCoords;
        glm::ivec2 nextTileCoords;
        glm::ivec2 dir;
        glm::ivec2 prevDir;

        std::vector<int> passengers;
        std::vector<std::pair<int, int>> visitedStations;

        int pathIdx;
        int passengerCapacity = 45;
        int end = 0;
        float tileLength;
        float speed = 3.0f;
        float angularVelocity = speed / tileLength / 2.0f;
        float orientation = 0.0f;
        float standByTime = 2.0f;
        float waitingTime;
        float angle;
        float startAngle;
        float endAngle;
        float endX;
        float endZ;
        float tolerance;
        bool moving = false;
        bool rotating;
        bool subtractAngle;
        bool waiting;
        bool nearCenter;

        bool toBeRemoved;
        bool isLocomotive;
    };
} // namespace m1