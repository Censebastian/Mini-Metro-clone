#pragma once

#include "core/gpu/mesh.h"
#include <utility>

#include "lab_m1/tema2/station.h"
#include "lab_m1/tema2/train.h"

namespace m1
{
    class Map 
    {
     public:
        Map()
        {
        }

        Map(int length, std::vector<std::vector<float>> shapedHeightMap, int verticesPerUnit,
            float verticesDistance, float maxAvgHeight = 0.4f, float maxAvgDepth = -0.4f)
        {
            this->length = length;
            this->shapedHeightMap = shapedHeightMap;
            this->verticesPerUnit = verticesPerUnit;
            this->maxAvgHeight = maxAvgHeight;
            this->maxAvgDepth = maxAvgDepth;
            this->verticesDistance = verticesDistance;

            startHeightMap = (shapedHeightMap.size() / verticesPerUnit - length) / 2 * verticesPerUnit;

            terrainTileMap = std::vector<std::vector<int>>(length, std::vector<int>(length, 0));
            buildingTileMap = std::vector<std::vector<int>>(length, std::vector<int>(length, 0));
            initMapTiles();
        }

        ~Map()
        {
        }

        void initMapTiles()
        {
            for (int y = 0; y < length; y++)
            {
                for (int x = 0; x < length; x++)
                {
                    float averageTileHeight = 0.0f;
                    int shapedHeightMapY = startHeightMap + y * verticesPerUnit;
                    int shapedHeightMapX = startHeightMap + x * verticesPerUnit;
                    for (int i = 0; i < verticesPerUnit; i++)
                    {
                        for (int j = 0; j < verticesPerUnit; j++)
                        {
                            averageTileHeight += shapedHeightMap[shapedHeightMapY + i][shapedHeightMapX + j];
                        }
                    }

                    averageTileHeight /= verticesPerUnit * verticesPerUnit;

                    if (averageTileHeight >= maxAvgHeight)
                    {
                        terrainTileMap[y][x] = 1;
                    }
                    else if (averageTileHeight <= maxAvgDepth)
                    {
                        terrainTileMap[y][x] = 2;
                    }
                    else
                    {
                        terrainTileMap[y][x] = 0;
                    }
                }
            }
        }

        void printTerrainTiles()
        {
            for (int y = 0; y < length; y++)
            {
                for (int x = 0; x < length; x++)
                {
                    std::cout << (int)terrainTileMap[y][x] << " ";
                }
                std::cout << std::endl;
            }
            std::cout << std::endl;
        }

        void printBuildingTiles()
        {
            for (int y = 0; y < length; y++)
            {
                for (int x = 0; x < length; x++)
                {
                    std::cout << (int)buildingTileMap[y][x] << " ";
                }
                std::cout << std::endl;
            }
            std::cout << std::endl;
        }

        void printTrains(std::vector<Train> trains)
        {
            for (int y = 0; y < length; y++)
            {
                for (int x = 0; x < length; x++)
                {
                    bool printed = false;
                    for (auto& train : trains)
                    {
                        // bad notation, intended functionality
                        if (train.coordinates.x == y && train.coordinates.y == x)
                        {
                            std::cout <<  "T";
                            printed = true;
                        }
                    }
                    if (!printed) 
                    {
                        std::cout << "0 ";
                    }
                }
                std::cout << std::endl;
            }
            std::cout << std::endl;
        }

        bool canPlaceRail(glm::vec3 cursorIntersection)
        {
            int tileX = (int)cursorIntersection.x / (verticesPerUnit * verticesDistance) - startHeightMap / verticesPerUnit;
            int tileZ = (int)cursorIntersection.z / (verticesPerUnit * verticesDistance) - startHeightMap / verticesPerUnit;

            if (tileX < 0 || tileX >= length || tileZ < 0 || tileZ >= length)
            {
                return false;
            }
            
            if (buildingTileMap[tileX][tileZ] == 0)
            {
                return true;
            }
            return false;
        }

        bool canRemoveRail(glm::vec3 cursorIntersection)
        {
            int tileX = (int)cursorIntersection.x / (verticesPerUnit * verticesDistance) - startHeightMap / verticesPerUnit;
            int tileZ = (int)cursorIntersection.z / (verticesPerUnit * verticesDistance) - startHeightMap / verticesPerUnit;

            if (tileX < 0 || tileX >= length || tileZ < 0 || tileZ >= length)
            {
                return false;
            }
            
            if (buildingTileMap[tileX][tileZ] != 0)
            {
                return true;
            }
            return false;
        }

        Mesh* generateHighlightTile(glm::vec3 cursorIntersection, int buildingMode)
        {
            int tileX = (int)cursorIntersection.x / (verticesPerUnit * verticesDistance) - startHeightMap / verticesPerUnit;
            int tileZ = (int)cursorIntersection.z / (verticesPerUnit * verticesDistance) - startHeightMap / verticesPerUnit;

            if (tileX < 0 || tileX >= length || tileZ < 0 || tileZ >= length)
            {
                // std::cout << "generateHighlightTile - WARNING: OUT OF BOUNDS\n";
                return NULL;
            }
            
            glm::vec3 origin = glm::vec3(0.0f, 0.0f, 0.0f);
            glm::vec3 color;

            if (buildingMode == 1)
            {
                color = placeHighlightColor;
            }
            else if (buildingMode == 2)
            {
                color = removeHighlightColor;
            }
            else if (buildingMode == 3)
            {
                color = stationHighlightColor;
            }
            else
            {
                color = glm::vec3(1.0f, 1.0f, 1.0f);
            }


            std::vector<VertexFormat> vertices;

            int startX = tileX * verticesPerUnit + startHeightMap;
            int startZ = tileZ * verticesPerUnit + startHeightMap;

            for (int x = startX; x < startX + verticesPerUnit; x++)
            {
                for (int z = startZ; z < startZ + verticesPerUnit; z++)
                {
                    vertices.push_back(VertexFormat(origin
                        + glm::vec3(x * verticesDistance, shapedHeightMap[x][z] + 0.125f, z * verticesDistance), color));
                }
            }

            std::vector<unsigned int> indices;

            for (int x = 0; x < verticesPerUnit - 1; x++)
            {
                for (int z = 0; z < verticesPerUnit - 1; z++)
                {
                    indices.push_back(x * verticesPerUnit + z);
                    indices.push_back(x * verticesPerUnit + z + 1);
                    indices.push_back((x + 1) * verticesPerUnit + z);

                    indices.push_back(x * verticesPerUnit+ z + 1);
                    indices.push_back((x + 1) * verticesPerUnit+ z + 1);
                    indices.push_back((x + 1) * verticesPerUnit+ z);
                }
            }

            Mesh* highlightTile = new Mesh("highlightTile");
            highlightTile->InitFromData(vertices, indices);

            return highlightTile;
        }

        void placeRail(glm::vec3 cursorIntersection)
        {
            int tileX = (int)cursorIntersection.x / (verticesPerUnit * verticesDistance) - startHeightMap / verticesPerUnit;
            int tileZ = (int)cursorIntersection.z / (verticesPerUnit * verticesDistance) - startHeightMap / verticesPerUnit;

            conformRail(tileX, tileZ, 0, true); // forward to the neighbours 
        }

        Station placeStation(glm::vec3 cursorIntersection, float time, int difficulty)
        {
            int tileX = (int)cursorIntersection.x / (verticesPerUnit * verticesDistance) - startHeightMap / verticesPerUnit;
            int tileZ = (int)cursorIntersection.z / (verticesPerUnit * verticesDistance) - startHeightMap / verticesPerUnit;

            Station station({tileX, tileZ}, time, difficulty);

            buildingTileMap[tileX][tileZ] = station.stationType;
            conformRail(tileX, tileZ, 2, true);

            return station;
        }

        void placeStation(std::pair<int, int> tileCoords, int stationType)
        {
            if (stationType < 4 || stationType > 6)
            {
                std::cout << "placeStation: WARNING - stationType CANNOT BE OUTSIDE [4, 6] INTERVAL" << std::endl;
                return;
            }
            buildingTileMap[tileCoords.first][tileCoords.second] = stationType;
            conformRail(tileCoords.first, tileCoords.second, 2, true);
        }

        void conformRail(int tileX, int tileZ, int remove = 0, bool forward = false)
        {
            std::vector<std::pair<int, int>> neighbours;
            if (tileX != 0 && buildingTileMap[tileX - 1][tileZ] != 0)
            {
                neighbours.push_back({tileX - 1, tileZ});
            }
            if (tileX < length - 1 && buildingTileMap[tileX + 1][tileZ] != 0)
            {
                neighbours.push_back({tileX + 1, tileZ});
            }
            if (tileZ != 0 && buildingTileMap[tileX][tileZ - 1] != 0)
            {
                neighbours.push_back({tileX, tileZ - 1});
            }
            if (tileZ < length - 1 && buildingTileMap[tileX][tileZ + 1] != 0)
            {
                neighbours.push_back({tileX, tileZ + 1});
            }
            
            if (remove == 0)
            {
                if (neighbours.size() >= 3)
                {
                    buildingTileMap[tileX][tileZ] = 3;
                }
                else if (neighbours.size() == 2
                    && neighbours[0].first != neighbours[1].first
                    && neighbours[0].second != neighbours[1].second)
                {
                    buildingTileMap[tileX][tileZ] = 3;
                }
                else if (neighbours.size() == 2 && neighbours[0].second == neighbours[1].second)
                {
                    buildingTileMap[tileX][tileZ] = 2;
                }
                else if (neighbours.size() == 1 && neighbours[0].second == tileZ)
                {
                    buildingTileMap[tileX][tileZ] = 2;
                }
                else 
                {
                    buildingTileMap[tileX][tileZ] = 1;
                }
            }
            else if (remove == 1)
            {
                buildingTileMap[tileX][tileZ] = 0;
            }

            if (forward)
            {
                for (auto& neigh : neighbours)
                {   
                    if (buildingTileMap[neigh.first][neigh.second] <= 3)
                    {
                        conformRail(neigh.first, neigh.second);
                    }
                }
            }

        }

        void removeRail(glm::vec3 cursorIntersection)
        {
            int tileX = (int)cursorIntersection.x / (verticesPerUnit * verticesDistance) - startHeightMap / verticesPerUnit;
            int tileZ = (int)cursorIntersection.z / (verticesPerUnit * verticesDistance) - startHeightMap / verticesPerUnit;
            conformRail(tileX, tileZ, 1, true);
        }

        std::pair<int, int> getRandomFreeTile()
        {
            int x = std::rand() % length;
            int z = std::rand() % length;
            int counter = 0;

            while (buildingTileMap[x][z] != 0 || terrainTileMap[x][z] != 0)
            {
                x = std::rand() % length;
                z = std::rand() % length;

                if (counter++ > maxGenerationTries)
                {
                    return {-1, -1};
                }
            }
            return {x, z};
        }

        std::vector<std::vector<int>> terrainTileMap;
        std::vector<std::vector<int>> buildingTileMap;
        int length;
        int verticesPerUnit;
        int startHeightMap;

     private:
        float maxAvgHeight;
        float maxAvgDepth;
        float verticesDistance;
        glm::vec3 placeHighlightColor = glm::vec3(0.0f / 255.0f, 204.0f / 255.0f, 0.0f / 255.0f);
        glm::vec3 removeHighlightColor = glm::vec3(255.0f / 255.0f, 102.0f / 255.0f, 102.0f / 255.0f);
        glm::vec3 stationHighlightColor = glm::vec3(151.0f / 255.0f, 85.0f / 255.0f, 227.0f / 255.0f);

        std::vector<std::vector<float>> shapedHeightMap;

        int maxGenerationTries = 200;
    };
}