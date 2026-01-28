#pragma once

#include "utils/glm_utils.h"
#include "utils/math_utils.h"
#include "core/gpu/mesh.h"
#include <iostream>
#include <numeric>
#include <random>
#include <algorithm>

namespace m1
{
    class Terrain
    {
     public:
        Terrain()
        {
            this->terrainSize = 16;
            resolution = terrainSize * verticesPerUnit;

            heightMap = generatePerlinNoise(resolution, resolution, 0.009f);
            mesh = generateMesh(4.0f);

            width = resolution * verticesDistance;
            length = resolution * verticesDistance;
        }

        Terrain(int terrainSize, float flatnessPower=4.0f, float scale = 0.009f)
        {
            resolution = terrainSize * verticesPerUnit;

            heightMap = generatePerlinNoise(resolution, resolution, scale);
            shapedHeightMap = std::vector<std::vector<float>>(resolution, std::vector<float>(resolution, 0.0f));
            mesh = generateMesh(flatnessPower);

            width = resolution * verticesDistance;
            length = resolution * verticesDistance;
        }

        ~Terrain()
        {
        }

        Mesh* mesh;
        int terrainSize; // size of terrain units
        int resolution; // vertices number
        int verticesPerUnit = 50;
        float verticesDistance = 0.125f;
        float width;
        float length;
        float maxHeight = 10.0f;
        float maxDepth = 20.0f;
        std::vector<std::vector<float>> heightMap;
        std::vector<std::vector<float>> shapedHeightMap;


     private:
        Mesh* generateMesh(float flatnessPower)
        {
            glm::vec3 origin = glm::vec3(0.0f, 0.0f, 0.0f);
            std::vector<VertexFormat> vertices;

            for (int i = 0; i < resolution; i++)
            {
                for (int j = 0; j < resolution; j++)
                {
                    float shapedHeight = std::pow(std::abs(heightMap[i][j]), flatnessPower);
                    if (heightMap[i][j] < 0)
                    {
                        shapedHeight *= -1;
                    }

                    glm::vec3 color;
                    if (shapedHeight >= 0.0f)
                    {
                        color = glm::mix(groundColor, hillColor, shapedHeight);
                    }
                    else if (shapedHeight < 0.0f)
                    {
                        color = glm::mix(groundColor, waterColor, -shapedHeight);
                    }
                    shapedHeightMap[i][j] = shapedHeight >= 0 ? shapedHeight * maxHeight : shapedHeight * maxDepth;
                    vertices.push_back(VertexFormat(origin +
                        glm::vec3(i * verticesDistance, shapedHeightMap[i][j], j * verticesDistance), color));
                }
            }

            std::vector<unsigned int> indices;
            for (int i = 0; i < resolution - 1; i++)
            {
                for (int j = 0; j < resolution - 1; j++)
                {
                    indices.push_back(i * resolution + j);
                    indices.push_back(i * resolution + j + 1);
                    indices.push_back((i + 1) * resolution + j);

                    indices.push_back(i * resolution + j + 1);
                    indices.push_back((i + 1) * resolution + j + 1);
                    indices.push_back((i + 1) * resolution + j);
                }
            }

            Mesh* terrain = new Mesh("terrain");
            terrain->InitFromData(vertices, indices);

            return terrain;
        }

        std::vector<std::vector<float>> generatePerlinNoise(int width, int height, float scale = 0.1f)
        {
            std::vector<int> p(512);
            std::iota(p.begin(), p.begin() + 256, 0); // Fill 0..255

            std::default_random_engine engine(std::random_device{}());
            std::shuffle(p.begin(), p.begin() + 256, engine);

            for (int i = 0; i < 256; ++i) p[256 + i] = p[i];

            auto fade = [](float t) { return t * t * t * (t * (t * 6 - 15) + 10); };

            auto lerp = [](float t, float a, float b) { return a + t * (b - a); };

            auto grad = [](int hash, float x, float y) {
                int h = hash & 15;
                float u = h < 8 ? x : y;
                float v = h < 4 ? y : h == 12 || h == 14 ? x : 0;
                return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
            };

            std::vector<std::vector<float>> map(width, std::vector<float>(height));

            for (int x = 0; x < width; x++) {
                for (int y = 0; y < height; y++) {
                    float fx = x * scale;
                    float fy = y * scale;

                    int X = (int)floor(fx) & 255;
                    int Y = (int)floor(fy) & 255;

                    fx -= floor(fx);
                    fy -= floor(fy);

                    float u = fade(fx);
                    float v = fade(fy);

                    int A = p[X] + Y, AA = p[A], AB = p[A + 1];
                    int B = p[X + 1] + Y, BA = p[B], BB = p[B + 1];

                    float res = lerp(v, lerp(u, grad(p[AA], fx, fy), grad(p[BA], fx - 1, fy)),
                                        lerp(u, grad(p[AB], fx, fy - 1), grad(p[BB], fx - 1, fy - 1)));

                    map[x][y] = res * 1.4142f; 
                }
            }

            return map;
        }    

        glm::vec3 groundColor = glm::vec3(222.0f / 255.0f, 243.0f / 255.0f, 220.0f / 255.0f);
        glm::vec3 hillColor = glm::vec3(130.0f / 255.0f, 104.0f / 255.0f, 32.0f / 255.0f);
        glm::vec3 waterColor = glm::vec3(94.0f / 255.0f, 145.0f / 255.0f, 246.0f / 255.0f);
    };
}   // namespace implemented
