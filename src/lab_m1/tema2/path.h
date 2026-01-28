#pragma once

#include <utility>
#include <set>
#include <queue>

namespace m1 
{
    class Path
    {
     public:
        Path(std::set<std::pair<int, int>>& occupiedTiles, std::vector<std::vector<int>>& buildingTileMap)
        {
            this->occupiedTiles = occupiedTiles;
            std::vector<std::pair<int, int>> visitedStations;
            std::vector<std::pair<int, int>> visitedTiles;
            std::vector<std::pair<int, int>> path;
            std::vector<std::vector<std::pair<int, int>>> intermediatePaths;

            for (auto& tile : occupiedTiles)
            {
                if (buildingTileMap[tile.first][tile.second] >= 4)
                {
                    visitedStations.push_back(tile);
                    buildPathDfs(tile, intermediatePaths, path, buildingTileMap,
                        visitedStations, visitedTiles);
                    break;
                }
            }

            if (intermediatePaths.size() != 0)
            {   
                this->path.push_back(visitedStations.front());
                while (intermediatePaths.size() != 0)
                {
                    std::vector<std::pair<int, int>> minPath;
                    for (auto& currPath : intermediatePaths)
                    {
                        if (minPath.size() == 0 || currPath.size() < minPath.size())
                        {
                            minPath = currPath;
                        }
                    }
                    visitedStations.push_back(minPath.back());
                    this->path.insert(this->path.end(), minPath.begin() + 1, minPath.end()); // + bcs a path begins with the last station
                    intermediatePaths.clear();

                    // search for min path from the current node to the next 
                    // unvisited node
                    buildPathDfs(this->path.back(), intermediatePaths, path, buildingTileMap,
                        visitedStations, visitedTiles);

                    // if no such path has been found break
                }
                this->end1 = this->path.front();
                this->end2 = this->path.back();
            }
        }

        ~Path()
        {
        }

        void buildPathDfs(std::pair<int, int> curr, std::vector<std::vector<std::pair<int, int>>>& intermediatePaths,
            std::vector<std::pair<int, int>> path, std::vector<std::vector<int>>& buildingTileMap,
            std::vector<std::pair<int, int>>& visitedStations, std::vector<std::pair<int, int>> visitedTiles)
        {
            path.push_back(curr);
            visitedTiles.push_back(curr);

            if (buildingTileMap[curr.first][curr.second] >= 4 &&
                std::find(visitedStations.begin(), visitedStations.end(), curr) == visitedStations.end())
            {
                intermediatePaths.push_back(path);
                return;
            }   
            
            int dr[] = {-1, 1, 0, 0};
            int dc[] = {0, 0, -1, 1};

            for (int i = 0; i < 4; i++) 
            {
                int x = curr.first + dr[i];
                int z = curr.second + dc[i];
                std::pair<int, int> next = {x, z};

                if (0 <= x && x < buildingTileMap.size() && 0 <= z && z < buildingTileMap[0].size() &&
                    std::find(visitedTiles.begin(), visitedTiles.end(), next) == visitedTiles.end() &&
                    !(buildingTileMap[curr.first][curr.second] >= 4 && buildingTileMap[x][z] >= 4) &&
                    buildingTileMap[x][z] != 0)
                {
                    buildPathDfs(next, intermediatePaths, path, buildingTileMap,
                        visitedStations, visitedTiles);
                }
            }
        }

        bool operator == (const Path &p1)
        {
           if(this->occupiedTiles == p1.occupiedTiles)
             return true;
           else
             return false;
        }

        bool isOnPath(int x, int z)
        {
            std::pair<int, int> coordinates;
            for (auto& stone : path)
            {
                if (stone == coordinates)
                {
                    return true;
                }
            }
            return false;
        }

        bool isEnd(int x, int z)
        {
            return (end1.first == x && end1.second == z)
                || (end2.first == x && end2.second == z);
        }

        bool compareTiles(std::set<std::pair<int, int>> occupiedTiles)
        {
            return occupiedTiles == this->occupiedTiles;
        }

        void printOccupiedTiles()
        {   
            std::cout << "newPath\n";
            for (auto& tile : occupiedTiles)
            {
                std::cout << tile.first << " " << tile.second << std::endl;
            }
            std::cout << std::endl;
        }

        void printPath()
        {   
            std::cout << "newPath\n";
            for (auto& tile : path)
            {
                std::cout << tile.first << " " << tile.second << " | ";
            }
            std::cout << std::endl;
        }

        std::pair<int, int> end1;
        std::pair<int, int> end2;
        std::vector<std::pair<int, int>> path;
        std::set<std::pair<int, int>> occupiedTiles; // path signature
    };
} // namespace m1