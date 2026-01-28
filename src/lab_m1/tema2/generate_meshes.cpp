#include "lab_m1/tema2/generate_meshes.h"

#include <vector>

#include "core/engine.h"
#include "utils/gl_utils.h"


Mesh* generateCylinder(const std::string &name, glm::vec3 leftBottomCorner, float length, float radius, glm::vec3 color, bool fill)
{
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    int segments = 32;       
    
    for (int i = 0; i <= segments; i++)
    {
        float ratio = (float)i / segments;
        float angle = ratio * 2.0f * glm::pi<float>();
        float x = radius * std::cos(angle);
        float z = radius * std::sin(angle);

        vertices.push_back(VertexFormat(leftBottomCorner + glm::vec3(x, 0, z), color));
        vertices.push_back(VertexFormat(leftBottomCorner + glm::vec3(x, length, z), color));
    }

    for (int i = 0; i < segments; i++)
    {
        int b0 = i * 2;
        int t0 = i * 2 + 1;
        int b1 = (i + 1) * 2;
        int t1 = (i + 1) * 2 + 1;

        indices.push_back(b0); indices.push_back(t0); indices.push_back(b1);
        indices.push_back(t0); indices.push_back(t1); indices.push_back(b1);
    }

    if (fill)
    {
        int bottomCenterIdx = vertices.size();
        vertices.push_back(VertexFormat(leftBottomCorner, color));
        
        int topCenterIdx = vertices.size();
        vertices.push_back(VertexFormat(leftBottomCorner + glm::vec3(0, length, 0), color));

        int capStartIdx = vertices.size();
        for (int i = 0; i <= segments; i++)
        {
            float ratio = (float)i / segments;
            float angle = ratio * 2.0f * glm::pi<float>();
            float x = radius * std::cos(angle);
            float z = radius * std::sin(angle);

            vertices.push_back(VertexFormat(leftBottomCorner + glm::vec3(x, 0, z), color));      // Bottom ring
            vertices.push_back(VertexFormat(leftBottomCorner + glm::vec3(x, length, z), color)); // Top ring
        }

        for (int i = 0; i < segments; i++)
        {
            int bCurrent = capStartIdx + (i * 2);
            int bNext = capStartIdx + ((i + 1) * 2);
            int tCurrent = capStartIdx + (i * 2) + 1;
            int tNext = capStartIdx + ((i + 1) * 2) + 1;

            indices.push_back(bottomCenterIdx);
            indices.push_back(bNext);
            indices.push_back(bCurrent);

            indices.push_back(topCenterIdx);
            indices.push_back(tCurrent);
            indices.push_back(tNext);
        }
    }

    Mesh* cylinder = new Mesh(name);
    cylinder->InitFromData(vertices, indices);
    return cylinder;
}

Mesh* generateRectangle(const std::string &name, glm::vec3 leftBottomCorner, float length, glm::vec3 color, bool fill)
{
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;
    glm::vec3 bottomCenter = leftBottomCorner;

    float half = length / 2.0f;

    vertices.push_back(VertexFormat(bottomCenter + glm::vec3(-half, 0,  half), color));      
    vertices.push_back(VertexFormat(bottomCenter + glm::vec3( half, 0,  half), color)); 
    vertices.push_back(VertexFormat(bottomCenter + glm::vec3(-half, 0, -half), color));           
    vertices.push_back(VertexFormat(bottomCenter + glm::vec3( half, 0, -half), color));      

    vertices.push_back(VertexFormat(bottomCenter + glm::vec3(-half, length,  half), color));      
    vertices.push_back(VertexFormat(bottomCenter + glm::vec3( half, length,  half), color)); 
    vertices.push_back(VertexFormat(bottomCenter + glm::vec3(-half, length, -half), color));           
    vertices.push_back(VertexFormat(bottomCenter + glm::vec3( half, length, -half), color));      

    if (fill) {
        indices = {
            0, 1, 5,    0, 5, 4,
            2, 3, 7,    2, 7, 6,
            1, 3, 7,    1, 7, 5,
            0, 2, 6,    0, 6, 4,
            4, 5, 7,    4, 7, 6,
            0, 1, 3,    0, 3, 2 
        };
    } else {
        indices = {
            0, 1, 1, 3, 3, 2, 2, 0,
            4, 5, 5, 7, 7, 6, 6, 4,
            0, 4, 1, 5, 2, 6, 3, 7 
        };
    }

    Mesh* rectangle = new Mesh(name);
    rectangle->InitFromData(vertices, indices);
    return rectangle;
}

Mesh* generatePyramid(const std::string &name, glm::vec3 bottomCenter, float length, glm::vec3 color, bool fill)
{
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    float half = length / 2.0f;

    vertices.push_back(VertexFormat(bottomCenter + glm::vec3(0, length, 0), color));
    
    vertices.push_back(VertexFormat(bottomCenter + glm::vec3(-half, 0,  half), color)); 
    vertices.push_back(VertexFormat(bottomCenter + glm::vec3( half, 0,  half), color));
    vertices.push_back(VertexFormat(bottomCenter + glm::vec3( half, 0, -half), color)); 
    vertices.push_back(VertexFormat(bottomCenter + glm::vec3(-half, 0, -half), color)); 

    if (fill) {
        indices = {
            0, 1, 2, 
            0, 2, 3, 
            0, 3, 4, 
            0, 4, 1, 

            1, 3, 2,
            1, 4, 3
        };
    } else {
        indices = {
            0, 1, 0, 2, 0, 3, 0, 4, 
            1, 2, 2, 3, 3, 4, 4, 1 
        };
    }

    Mesh* pyramid = new Mesh(name);
    pyramid->InitFromData(vertices, indices);
    return pyramid;
} 

Mesh* generateRing(const std::string &name, glm::vec3 bottomCenter, float outerRadius, float thickness, float height, glm::vec3 color)
{
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    int segments = 32;
    float innerRadius = outerRadius - thickness;

    for (int i = 0; i <= segments; i++)
    {
        float ratio = (float)i / segments;
        float angle = ratio * 2.0f * glm::pi<float>();
        float cosA = std::cos(angle);
        float sinA = std::sin(angle);

        vertices.push_back(VertexFormat(bottomCenter + glm::vec3(outerRadius * cosA, 0, outerRadius * sinA), color));
        vertices.push_back(VertexFormat(bottomCenter + glm::vec3(outerRadius * cosA, height, outerRadius * sinA), color));

        vertices.push_back(VertexFormat(bottomCenter + glm::vec3(innerRadius * cosA, 0, innerRadius * sinA), color));
        vertices.push_back(VertexFormat(bottomCenter + glm::vec3(innerRadius * cosA, height, innerRadius * sinA), color));
    }

    for (int i = 0; i < segments; i++)
    {
        int curr = i * 4;
        int next = (i + 1) * 4;

        indices.push_back(curr + 0); indices.push_back(curr + 1); indices.push_back(next + 1);
        indices.push_back(curr + 0); indices.push_back(next + 1); indices.push_back(next + 0);

        indices.push_back(curr + 2); indices.push_back(next + 3); indices.push_back(curr + 3);
        indices.push_back(curr + 2); indices.push_back(next + 2); indices.push_back(next + 3);

        indices.push_back(curr + 1); indices.push_back(curr + 3); indices.push_back(next + 3);
        indices.push_back(curr + 1); indices.push_back(next + 3); indices.push_back(next + 1);

        indices.push_back(curr + 0); indices.push_back(next + 2); indices.push_back(curr + 2);
        indices.push_back(curr + 0); indices.push_back(next + 0); indices.push_back(next + 2);
    }

    Mesh* ring = new Mesh(name);
    ring->InitFromData(vertices, indices);
    return ring;
}
