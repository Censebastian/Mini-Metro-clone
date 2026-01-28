#pragma once

#include <string>

#include "core/gpu/mesh.h"
#include "utils/glm_utils.h"

Mesh* generateCylinder(const std::string &name, glm::vec3 leftBottomCorner, float length, float radius, glm::vec3 color, bool fill = false);
Mesh* generateRectangle(const std::string &name, glm::vec3 leftBottomCorner, float length, glm::vec3 color, bool fill = false);
Mesh* generatePyramid(const std::string &name, glm::vec3 bottomCenter, float length, glm::vec3 color, bool fill);
Mesh* generateRing(const std::string &name, glm::vec3 bottomCenter, float outerRadius, float thickness, float height, glm::vec3 color);