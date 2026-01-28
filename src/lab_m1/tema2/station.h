#pragma once

#include <vector>

namespace m1
{
    class Station
    {
     public:
        Station(std::pair<int, int> tileCoords, float time, int difficulty)
        {
            this->tileCoords = tileCoords;
            idOffset = 4;
            stationType = std::rand() % 3 + idOffset; // 0 - square, 1 - triangle, 2 - circle
            spawnRate = 2.0f;
            spawnTime = 10.0f;
            passTypeCapacity = 10;
            passengers = std::vector<int>(3, 0);

            prevGenTime = time;
            timeFilled = time;
            maxFullTime = 20.0f;

            full = false;

            for (int i = 0; i < passengers.size(); i++)
            {
                if (i + 4 != stationType)
                {
                    arrIdxs.push_back(i);
                }
            }
            
        }

        ~Station()
        {
        }

        void generatePassengers(float time, int difficulty, bool& gameOver)
        {
            this->difficulty = difficulty;
            if (full)
            {
                if (time - timeFilled > maxFullTime)
                {
                    gameOver = true;
                    return;
                }
            }
            else // if not full don't let timeFilled lag behind time
            {
                timeFilled = time;
            }

            for (auto& pass : passengers)
            {
                // std::cout << "printam pas ca nu se mai poate " <<  pass << " " << passTypeCapacity << std::endl;
                if (pass == passTypeCapacity && !full)
                {
                    full = true;
                }
            }

            if (prevGenTime + spawnTime <= time)
            {
                int passengerType = std::rand() % 2;

                if (passengers[passengerType] != passTypeCapacity)
                {
                    passengers[passengerType] += this->difficulty * spawnRate;
                }
                else if (passengers[(passengerType + 1) % 2] != passengerType)
                {
                    passengers[(passengerType + 1) % 2] += this->difficulty * spawnRate;
                }

                prevGenTime = time;
            }
        }

        int collectPassengers(int capacity)
        {
            auto it = std::max_element(passengers.begin(), passengers.end());

            int retVal = -1;
            if (*it > capacity)
            {
                *it = *it - capacity;
                retVal = capacity;
            }
            else
            {
                int passColl = *it;
                *it = 0;
                retVal = passColl;
            }
            it = std::max_element(passengers.begin(), passengers.end());

            if (*it < passTypeCapacity && full)
            {
                full = false;
            }
            if (retVal == -1)
            {
                std::cout << "collectPassengers: WARNING - COLLECTED -1 PASSENGERS" << std::endl;
            }
            return retVal;
        }
        
        void printData(float time)
        {
            std::cout << time << " " << prevGenTime << std::endl;
            std::cout << difficulty << " " << spawnRate << std::endl;
            std::cout << timeFilled << std::endl;
            for (auto& it : passengers)
            {
                std::cout << it << " ";
            }
            std::cout << std::endl;
        }

        std::pair<int, int> tileCoords;
        std::vector<int> passengers;
        std::vector<int> arrIdxs;
        float difficulty;
        float prevGenTime;
        float spawnTime;
        float spawnRate;
        float timeFilled;
        float maxFullTime;

        int stationType;
        int passTypeCapacity;
        int idOffset;

        bool full;
    };
} // namespace m1