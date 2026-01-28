#include "lab_m1/tema2/tema2.h"

#include <vector>
#include <ctime>
#include <string>
#include <iostream>
#include <queue>
#include <set>

using namespace std;
using namespace m1;

Tema2::Tema2()
{
}

Tema2::~Tema2()
{
}

void Tema2::Init()
{
    terrain = new Terrain(10, 3.0f);
    map = new Map(10, terrain->shapedHeightMap, terrain->verticesPerUnit, terrain->verticesDistance);

    camera = new implemented::MyCamera();
    camera->Set(glm::vec3(terrain->width / 2, 35, terrain->length * 1.0f / 5.0f),
        glm::vec3(terrain->width / 2, 0, terrain->length / 2), glm::vec3(0, 1, 0));

    {
        Mesh* mesh = new Mesh("box");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("sphere");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = generateCylinder("cylinder", glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, 0.5f, glm::vec3(166.0f / 255.0f, 115.0f / 255.0f, 68.0f / 255.0f), true);
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = generateCylinder("steamCylinder", glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, 0.5f, glm::vec3(141.0f / 255.0f, 190.0f / 255.0f, 224.0f / 255.0f), true);
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = generateCylinder("cylStation", glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, 0.5f, glm::vec3(250.0f / 255.0f, 130.0f / 255.0f, 120.0f / 255.0f), true);
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = generateRectangle("rectangle", glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, glm::vec3(150.0f / 255.0f, 143.0f / 255.0f, 136.0f / 255.0f), true);
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = generateRectangle("wood", glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, glm::vec3(93.0f / 255.0f, 67.0f / 255.0f, 44.0f / 255.0f), true);
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = generateRectangle("metal", glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, glm::vec3(189.0f / 255.0f, 195.0f / 255.0f, 199.0f / 255.0f), true);
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = generateRectangle("recStation", glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, glm::vec3(47.0f / 255.0f, 123.0f / 255.0f, 245.0f / 255.0f), true);
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = generateRectangle("cabin", glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, glm::vec3(219.0f / 255.0f, 224.0f / 255.0f, 141.0f / 255.0f), true);
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = generatePyramid("pyramid", glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, glm::vec3(182.0f / 255.0f, 13.0f / 255.0f, 109.0f / 255.0f), true);
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = generateRing("tunnel", glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, 0.1f, 1.0f, glm::vec3(165.0f / 255.0f, 172.0f / 255.0f, 175.0f / 255.0f));
        meshes[mesh->GetMeshID()] = mesh;
    }

    projectionMatrix = glm::perspective(RADIANS(100), window->props.aspectRatio, 0.01f, 200.0f);

    buildingMode = false;
    canPlaceRail = false;
    canRemoveRail = false;
    pause = false;
    tutorial = true;
    gameOver = false;
    time = 0.0f;
    timeMultiplier = 1;

    std::srand(std::time({}));

    tileLength = terrain->verticesDistance * terrain->verticesPerUnit;
    trainLength = tileLength * 2 / 3;
    trainWidth = tileLength * 2 / 5;
    railLength = tileLength;
    railWidth = trainWidth;
    stationSize = 2.0f;
}


void Tema2::FrameStart()
{
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    glViewport(0, 0, resolution.x, resolution.y);
}


void Tema2::Update(float deltaTimeSeconds)
{

    if (gameOver)
    {
        std::cout << "Game Over.\n";
    }
    else
    {
        if (!pause)
        {
            UpdateTime(deltaTimeSeconds);
            UpdateStations();
            UpdateTrains(deltaTimeSeconds);
        }

        if (highlightTile != NULL)
        {
            RenderMesh(highlightTile, shaders["VertexColor"], glm::mat4(1));
        }
        RenderMesh(terrain->mesh, shaders["VertexColor"], glm::mat4(1));
        RenderRails();
    }
}


void Tema2::FrameEnd()
{
}

void Tema2::RenderMesh(Mesh * mesh, Shader * shader, const glm::mat4 & modelMatrix)
{
    if (!mesh || !shader || !shader->program)
        return;

    // Render an object using the specified shader and the specified position
    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    mesh->Render();
}

void Tema2::OnInputUpdate(float deltaTime, int mods)
{
    // move the camera only if B button is pressed
    if (window->KeyHold(GLFW_KEY_B))
    {
        float cameraSpeed = 30.0f;

        if (window->KeyHold(GLFW_KEY_W)) {
            // TODO(student): Translate the camera forward
            camera->MoveForward(cameraSpeed * deltaTime);
        }

        if (window->KeyHold(GLFW_KEY_A)) {
            // TODO(student): Translate the camera to the left
            camera->TranslateRight(-cameraSpeed * deltaTime);
        }

        if (window->KeyHold(GLFW_KEY_S)) {
            // TODO(student): Translate the camera backward
            camera->MoveForward(-cameraSpeed * deltaTime);
        }

        if (window->KeyHold(GLFW_KEY_D)) {
            // TODO(student): Translate the camera to the right
            camera->TranslateRight(cameraSpeed * deltaTime);
        }

        if (window->KeyHold(GLFW_KEY_Q)) {
            // TODO(student): Translate the camera downward
            camera->TranslateUpward(-cameraSpeed * deltaTime);

        }

        if (window->KeyHold(GLFW_KEY_E)) {
            // TODO(student): Translate the camera upward
            camera->TranslateUpward(cameraSpeed * deltaTime);
        }
    }

}

void Tema2::OnKeyPress(int key, int mods)
{
    if (key == GLFW_KEY_1) 
    {
        buildingMode = 0;
    }
    if (key == GLFW_KEY_2) 
    {
        buildingMode = 1;
    }
    if (key == GLFW_KEY_3) 
    {
        buildingMode = 2;
    }
    if (key == GLFW_KEY_4)
    {
        buildingMode = 3;
    }

    if (key == GLFW_KEY_P)
    {
        pause = !pause;
    }

    if (key == GLFW_KEY_LEFT_BRACKET)
    {
        timeMultiplier -= 2;
        if (timeMultiplier < 1)
        {
            timeMultiplier = 1;
        }
    }
    if (key == GLFW_KEY_RIGHT_BRACKET)
    {
        timeMultiplier += 2;
        if (timeMultiplier > 10)
        {
            timeMultiplier = 10;
        }
    }
}


void Tema2::OnKeyRelease(int key, int mods)
{
    // Add key release event
}

void Tema2::MousePick(int mouseX, int mouseY)
{
    glm::ivec2 resolution = window->GetResolution();
    float normalizedX = 2 * (float)mouseX / resolution.x - 1.0f;
    float normalizedY = 1.0f - 2 * (float)mouseY / resolution.y;
    float normalizedZ = -1.0f;

    glm::vec4 ray_clip = glm::vec4(normalizedX, normalizedY, normalizedZ, 1.0f);
    glm::vec4 ray_eye = glm::inverse(projectionMatrix) * ray_clip;
    ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0f, 0.0f);

    glm::vec3 ray_wrld = (glm::inverse(camera->GetViewMatrix()) * ray_eye);
    ray_wrld = glm::normalize(ray_wrld);
    
    if (glm::dot(ray_wrld, glm::vec3(0.0f, 1.0f, 0.0f)) == 0.0f)
    {
        cursorIntersection = glm::vec3(-0.1f, -0.1f, -0.1f);
    }
    else
    {
        float t = -glm::dot(camera->position, glm::vec3(0.0f, 1.0f, 0.0f)) / glm::dot(ray_wrld, glm::vec3(0.0f, 1.0f, 0.0f));
        if (t < 0.0f)
        {
            cursorIntersection = glm::vec3(-0.1f, -0.1f, -0.1f);
        }
        else 
        {
            cursorIntersection = camera->position + t * ray_wrld;
        }
    }
}

void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    if (buildingMode == 1)
    {
        MousePick(mouseX, mouseY);
        canPlaceRail = map->canPlaceRail(cursorIntersection);
        canRemoveRail = false;
        highlightTile = map->generateHighlightTile(cursorIntersection, buildingMode);
    }
    else if (buildingMode == 2)
    {
        MousePick(mouseX, mouseY);
        canRemoveRail = map->canRemoveRail(cursorIntersection);
        canPlaceRail = false;
        highlightTile = map->generateHighlightTile(cursorIntersection, buildingMode);
    }
    else if (buildingMode == 3)
    {
        MousePick(mouseX, mouseY);
        canPlaceRail = map->canPlaceRail(cursorIntersection);
        canRemoveRail = false;
        highlightTile = map->generateHighlightTile(cursorIntersection, buildingMode);
    }
    else
    {
        canPlaceRail = false;
        canRemoveRail = false;
        highlightTile = NULL;
    }

    if (window->KeyHold(GLFW_KEY_B))
    {
        float sensivityOX = 0.001f;
        float sensivityOY = 0.001f;

        if (window->GetSpecialKeyState() == 0) {
            camera->RotateFirstPerson_OX(-deltaY * sensivityOY);
            camera->RotateFirstPerson_OY(-deltaX * sensivityOX);
        }
    }
}

void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    if (IS_BIT_SET(button, GLFW_MOUSE_BUTTON_LEFT))
    {
        if (canPlaceRail && buildingMode == 1)
        {
            map->placeRail(cursorIntersection);
        }
        else if (canRemoveRail && buildingMode == 2)
        {
            map->removeRail(cursorIntersection);
        }
        else if (canPlaceRail && buildingMode == 3)
        {
            stations.push_back(map->placeStation(cursorIntersection, time, difficulty));
        }
        UpdatePaths();
    }
}


void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema2::OnWindowResize(int width, int height)
{
}

void Tema2::UpdatePaths()
{
    std::vector<std::set<std::pair<int, int>>> connectedComponents;

    int rows = map->buildingTileMap.size();
    int cols = map->buildingTileMap[0].size();

    std::vector<std::vector<bool>> visited(rows, std::vector<bool>(cols, false));

    int dr[] = {-1, 1, 0, 0};
    int dc[] = {0, 0, -1, 1};

    for (int r = 0; r < rows; ++r)
    {
        for (int c = 0; c < cols; ++c)
        {
            if (map->buildingTileMap[r][c] > 0 && !visited[r][c])
            {
                std::set<std::pair<int, int>> currentComponent;
                std::queue<std::pair<int, int>> q;

                q.push({r, c});
                visited[r][c] = true;

                while (!q.empty())
                {
                    std::pair<int, int> curr = q.front();
                    q.pop();
                    
                    currentComponent.insert(curr);

                    for (int i = 0; i < 4; ++i)
                    {
                        int nr = curr.first + dr[i];
                        int nc = curr.second + dc[i];

                        if (nr >= 0 && nr < rows && nc >= 0 && nc < cols &&
                            map->buildingTileMap[nr][nc] > 0 && !visited[nr][nc] &&
                            (map->buildingTileMap[nr][nc] < 4 || map->buildingTileMap[curr.first][curr.second] < 4))
                        {
                            visited[nr][nc] = true;
                            q.push({nr, nc});
                        }
                    }
                }
                connectedComponents.push_back(currentComponent);
            }
        }
    }

    std::vector<Path> deadPaths = this->paths;

    for (auto itComp = connectedComponents.begin(); itComp != connectedComponents.end(); )
    {
        bool matchFound = false;
        for (auto itPath = deadPaths.begin(); itPath != deadPaths.end(); ++itPath)
        {
            if (itPath->compareTiles(*itComp))
            {
                deadPaths.erase(itPath);
                itComp = connectedComponents.erase(itComp);
                matchFound = true;
                break;
            }
        }
        if (!matchFound) {
            ++itComp;
        }
    }

    for (const auto& pathToRemove : deadPaths)
    {
        for (int i = trains.size() - 1; i >= 0; i--) 
        {
            if (trains[i].path == pathToRemove) {
                trains.erase(trains.begin() + i);
            }
        }

        for (int i = this->paths.size() - 1; i >= 0; i--)
        {
            if (this->paths[i] == pathToRemove) {
                this->paths.erase(this->paths.begin() + i);
                break;
            }
        }
    }

    for (auto& comp : connectedComponents) 
    {
        Path newPath(comp, map->buildingTileMap);
        if (!newPath.path.empty())
        {
            this->paths.push_back(newPath);
            trains.push_back(Train(newPath, &map->buildingTileMap,
                terrain->verticesPerUnit, terrain->verticesDistance));
        }
    }
}

void Tema2::UpdateTime(float deltaTimeSeconds)
{
    time += deltaTimeSeconds * timeMultiplier;
    if (time / 60 < 2)
    {
        difficulty = 1;
    }
    else if (time / 60 >= 2 && time / 60 < 3)
    {
        difficulty = 2;
    }
    else if (time / 60 >= 3)
    {
        difficulty = 3;
    }
}

void Tema2::SpawnStation()
{   
    bool spawn = false;
    int chance = std::rand();

    if (difficulty == 1)
    {
        spawn = chance % 10000 < 10;
    }
    else if (difficulty == 2)
    {
        spawn = chance % 10000 < 3 * 10;
    }
    else if (difficulty == 3)
    {
        spawn = chance % 1000 < 10;
    }

    if (spawn &&
        ((difficulty == 1 && stations.size() < 5) ||
        (difficulty == 2 && stations.size() < 10) ||
        (difficulty == 3 && stations.size() < 15)))
    {
        std::pair<int, int> tileCoords = map->getRandomFreeTile();
        stations.push_back(Station(tileCoords, time, difficulty));
        map->placeStation(tileCoords, stations.back().stationType);
    }
}

void Tema2::UpdateStations()
{
    SpawnStation();
    for (auto& station : stations)
    {
        station.generatePassengers(time, difficulty, gameOver);
        if (station.stationType == 4)
        {
            RenderStationCircular(glm::vec3((station.tileCoords.first + 0.5f) * tileLength, 2.0f, (station.tileCoords.second + 0.5f) * tileLength));
        }
        else if (station.stationType == 5)
        {
            RenderStationSquare(glm::vec3((station.tileCoords.first + 0.5f) * tileLength, 2.0f, (station.tileCoords.second + 0.5f) * tileLength));
        }
        else if (station.stationType == 6)
        {
            RenderStationTriangle(glm::vec3((station.tileCoords.first + 0.5f) * tileLength, 2.0f, (station.tileCoords.second + 0.5f) * tileLength));
        }
    }
}

void Tema2::UpdateTrains(float deltaTimeSeconds)
{
    for (auto it = trains.begin(); it != trains.end(); )
    {
        if (it->toBeRemoved)
        {
            trains.erase(it);
            continue;
        }
        if (it->moving == false)
        {
            it->startTrip();
        }
        if (it->waiting)
        {
           it->waitingTime += deltaTimeSeconds;
           if (it->waitingTime > it->standByTime)
           {
                it->waitingTime = 0.0f;
                it->waiting = false;
           }
        }
        else
        {
            it->update(deltaTimeSeconds, timeMultiplier);

            int dr[] = {-1, 1, 0, 0};
            int dc[] = {0, 0, -1, 1};

            for (int i = 0; i < 4; i++)
            {   
                std::pair<int, int> stationCoords = {it->tileCoords.x + dr[i], it->tileCoords.y + dc[i]};
                if (it->tileCoords.x + dr[i] >= 0 && it->tileCoords.x + dr[i] < map->buildingTileMap.size() && 
                    it->tileCoords.y + dc[i] >= 0 && it->tileCoords.y + dc[i] < map->buildingTileMap.size() && 
                    map->buildingTileMap[it->tileCoords.x + dr[i]][it->tileCoords.y + dc[i]] >= 4 && it->nearCenter &&
                    find(it->visitedStations.begin(), it->visitedStations.end(), stationCoords) == it->visitedStations.end())
                {
                    it->waiting = true;
                    it->coordinates = glm::vec2(it->endX, it->endZ);
                    it->visitedStations.push_back(stationCoords);
                }
            }
        }
        RenderLocomotive(glm::vec3(it->coordinates.x, 1.2f, it->coordinates.y), it->orientation);
        ++it;
    }
}


void Tema2::RenderLocomotive(glm::vec3 position, float orientation)
{
    int numberOfWheels = 5;
    float wheelDiameter = trainLength / numberOfWheels;
    float wheelLengh = trainWidth * 1 / 5;

    glm::mat4 modelMatrix;

    for (int i = 0; i < numberOfWheels; i++)
    {
        modelMatrix = glm::mat4(1) * 
                        Translate(position.x, position.y, position.z) * 
                        RotateOY(orientation) *
                        Translate(-trainLength / 2, 0.0f, -wheelLengh) * 
                        Translate(wheelDiameter / 2 + i * wheelDiameter, 0.0f, -(trainWidth / 2 - wheelLengh)) * 
                        Translate(0.0f, wheelDiameter / 2, 0.0f) * 
                        RotateOX(glm::pi<float>() / 2) *
                        Scale(wheelDiameter, wheelLengh, wheelDiameter);
        RenderMesh(meshes["cylinder"], shaders["VertexColor"], modelMatrix);
    }
    for (int i = 0; i < numberOfWheels; i++)
    {
        modelMatrix = glm::mat4(1) * 
                        Translate(position.x, position.y, position.z) * 
                        RotateOY(orientation) *
                        Translate(-trainLength / 2, 0.0f, -wheelLengh) * 
                        Translate(wheelDiameter / 2 + i * wheelDiameter, 0.0f, trainWidth / 2) * 
                        Translate(0.0f, wheelDiameter / 2, 0.0f) * 
                        RotateOX(glm::pi<float>() / 2) *
                        Scale(wheelDiameter, wheelLengh, wheelDiameter);
        RenderMesh(meshes["cylinder"], shaders["VertexColor"], modelMatrix);
    }

    float baseThickness = 0.2f;
    modelMatrix = glm::mat4(1) *
                    Translate(position.x, position.y, position.z) * 
                    Translate(0.0f, wheelDiameter, 0.0f) * 
                    RotateOY(orientation) *
                    Scale(trainLength, baseThickness, trainWidth);
    RenderMesh(meshes["rectangle"], shaders["VertexColor"], modelMatrix);


    float steamCylinderLength = trainLength * 5 / 8;
    float steamCylinderDiameter = trainWidth * 2 / 3;
    float cabinLength = trainLength * 3 / 8;
    float cabinWidth = trainWidth * 4 / 5;

    modelMatrix = glm::mat4(1) *
                    Translate(position.x, position.y, position.z) * 
                    RotateOY(orientation) *
                    Translate(-(trainLength / 8), 0.0f, 0.0f) * 
                    Translate(0.0f, steamCylinderDiameter / 2 + wheelDiameter + baseThickness, 0.0f) * 
                    RotateOY(glm::pi<float>() / 2) *
                    RotateOX(glm::pi<float>() / 2) *
                    Scale(steamCylinderDiameter, steamCylinderLength, steamCylinderDiameter);
    RenderMesh(meshes["steamCylinder"], shaders["VertexColor"], modelMatrix);

    modelMatrix = glm::mat4(1) *
                    Translate(position.x, position.y, position.z) * 
                    RotateOY(orientation) *
                    Translate(-(tileLength / 2) + cabinLength, wheelDiameter + baseThickness, 0.0f) * 
                    Scale(cabinLength, steamCylinderDiameter * 1.2f, cabinWidth);
    RenderMesh(meshes["cabin"], shaders["VertexColor"], modelMatrix);
}

void Tema2::RenderWagon(glm::vec3 position, float orientation)
{
    int numberOfWheels = 5;
    float wheelDiameter = trainLength / numberOfWheels;
    float wheelLengh = trainWidth * 1 / 5;

    glm::mat4 modelMatrix;

    for (int i = 0; i < numberOfWheels; i++)
    {
        modelMatrix = glm::mat4(1) * 
                        Translate(position.x, position.y, position.z) * 
                        RotateOY(orientation) *
                        Translate(-trainLength / 2, 0.0f, -wheelLengh) * 
                        Translate(wheelDiameter / 2 + i * wheelDiameter, 0.0f, -(trainWidth / 2 - wheelLengh)) * 
                        Translate(0.0f, wheelDiameter / 2, 0.0f) * 
                        RotateOX(glm::pi<float>() / 2) *
                        Scale(wheelDiameter, wheelLengh, wheelDiameter);
        RenderMesh(meshes["cylinder"], shaders["VertexColor"], modelMatrix);
    }
    for (int i = 0; i < numberOfWheels; i++)
    {
        modelMatrix = glm::mat4(1) * 
                        Translate(position.x, position.y, position.z) * 
                        RotateOY(orientation) *
                        Translate(-trainLength / 2, 0.0f, -wheelLengh) * 
                        Translate(wheelDiameter / 2 + i * wheelDiameter, 0.0f, trainWidth / 2) * 
                        Translate(0.0f, wheelDiameter / 2, 0.0f) * 
                        RotateOX(glm::pi<float>() / 2) *
                        Scale(wheelDiameter, wheelLengh, wheelDiameter);
        RenderMesh(meshes["cylinder"], shaders["VertexColor"], modelMatrix);
    }

    float baseThickness = 0.2f;
    modelMatrix = glm::mat4(1) *
                    Translate(position.x, position.y, position.z) * 
                    Translate(0.0f, wheelDiameter, 0.0f) * 
                    RotateOY(orientation) *
                    Scale(trainLength, baseThickness, trainWidth);
    RenderMesh(meshes["rectangle"], shaders["VertexColor"], modelMatrix);

    modelMatrix = glm::mat4(1) *
                    Translate(position.x, position.y, position.z) * 
                    Translate(0.0f, wheelDiameter + baseThickness, 0.0f) * 
                    RotateOY(orientation) *
                    Scale(trainLength, trainWidth, trainWidth);
    RenderMesh(meshes["cabin"], shaders["VertexColor"], modelMatrix);
}

void Tema2::RenderStationSquare(glm::vec3 position)
{
    glm::mat4 modelMatrix = glm::mat4(1) *
                    Translate(position.x, position.y, position.z) * 
                    Scale(stationSize, stationSize, stationSize);
    RenderMesh(meshes["recStation"], shaders["VertexColor"], modelMatrix);
}

void Tema2::RenderStationTriangle(glm::vec3 position)
{
    glm::mat4 modelMatrix = glm::mat4(1) *
                    Translate(position.x, position.y, position.z) * 
                    Scale(stationSize, stationSize, stationSize);
    RenderMesh(meshes["pyramid"], shaders["VertexColor"], modelMatrix);
}

void Tema2::RenderStationCircular(glm::vec3 position)
{
    glm::mat4 modelMatrix = glm::mat4(1) *
                    Translate(position.x, position.y, position.z) * 
                    Scale(stationSize, stationSize, stationSize);
    RenderMesh(meshes["cylStation"], shaders["VertexColor"], modelMatrix);
}

void Tema2::RenderRail(glm::vec3 position, int orientation)
{
    int noPlanks = 6;
    float plankWidth = railLength / 2 / noPlanks;
    float plankheight = plankWidth / 2;

    float angle = orientation == 1 ? glm::pi<float>() / 2 : 0.0f;

    glm::mat4 modelMatrix;
    for (int i = 0; i < noPlanks; i++)
    {
        modelMatrix = glm::mat4(1) *
                        Translate(position.x, position.y, position.z) * 
                        RotateOY(angle) * 
                        Translate(-railLength / 2, 0.0f, 0.0f) * 
                        Translate(0.5f * plankWidth + 2 * i * plankWidth, 0.0f, 0.0f) * 
                        Scale(plankWidth, plankheight, railWidth);
        RenderMesh(meshes["wood"], shaders["VertexColor"], modelMatrix);
    }

    float metalside = plankheight;
    modelMatrix = glm::mat4(1) *
                    Translate(position.x, position.y, position.z) * 
                    RotateOY(angle) * 
                    Translate(0.0f, plankheight, railWidth * 3.0f / 10.0f) *
                    Scale(railLength, metalside, metalside);
    RenderMesh(meshes["metal"], shaders["VertexColor"], modelMatrix);

    modelMatrix = glm::mat4(1) *
                    Translate(position.x, position.y, position.z) * 
                    RotateOY(angle) * 
                    Translate(0.0f, plankheight, -railWidth * 3.0f / 10.0f) *
                    Scale(railLength, metalside, metalside);
    RenderMesh(meshes["metal"], shaders["VertexColor"], modelMatrix);
}

void Tema2::RenderTunnel(glm::vec3 position, int orientation)
{
    RenderRail(position, orientation);
    float tunnelDiameter = trainWidth * 1.3f;
    float tunnelLength = tileLength;
    float angle = orientation == 1 ? 0.0f : glm::pi<float>() / 2;

    glm::mat4 modelMatrix = glm::mat4(1) *
                                Translate(position.x, position.y, position.z) * 
                                RotateOY(angle) * 
                                Translate(0.0f, tunnelDiameter / 2, -tunnelLength / 2) * 
                                RotateOX(glm::pi<float>() / 2) * 
                                Scale(tunnelDiameter, tunnelLength, tunnelDiameter);
    RenderMesh(meshes["tunnel"], shaders["VertexColor"], modelMatrix);
}

void Tema2::RenderBridge(glm::vec3 position, int orientation)
{
    RenderRail(position, orientation);
    float stumpHeight = 20.0f;
    float stumpSide = railLength / 12;
    float angle = orientation == 1 ? glm::pi<float>() / 2 : 0.0f;

    glm::mat4 modelMatrix = glm::mat4(1) *
                                Translate(position.x, position.y, position.z) * 
                                RotateOY(angle) * 
                                Translate(-railLength * 1 / 3, -stumpHeight, -railWidth) *
                                Scale(stumpSide, stumpHeight, stumpSide);
    RenderMesh(meshes["wood"], shaders["VertexColor"], modelMatrix);

    modelMatrix = glm::mat4(1) *
                    Translate(position.x, position.y, position.z) * 
                    RotateOY(angle) * 
                    Translate(-railLength * 1 / 3, -stumpHeight, railWidth) *
                    Scale(stumpSide, stumpHeight, stumpSide);
    RenderMesh(meshes["wood"], shaders["VertexColor"], modelMatrix);

    modelMatrix = glm::mat4(1) *
                    Translate(position.x, position.y, position.z) * 
                    RotateOY(angle) * 
                    Translate(railLength * 1 / 3, -stumpHeight, railWidth) *
                    Scale(stumpSide, stumpHeight, stumpSide);
    RenderMesh(meshes["wood"], shaders["VertexColor"], modelMatrix);

    modelMatrix = glm::mat4(1) *
                    Translate(position.x, position.y, position.z) * 
                    RotateOY(angle) * 
                    Translate(railLength * 1 / 3, -stumpHeight, railWidth) *
                    Scale(stumpSide, stumpHeight, stumpSide);
    RenderMesh(meshes["wood"], shaders["VertexColor"], modelMatrix);
}

void Tema2::RenderRails()
{
    for (int i = 0; i < map->buildingTileMap.size(); i++)
    {
        for (int j = 0; j < map->buildingTileMap.size(); j++)
        {
            glm::vec3 position = glm::vec3(((float)i + 0.5f) * tileLength, 1.0f, ((float)j + 0.5f) * tileLength);
            if (map->terrainTileMap[i][j] == 0) // flat
            {
                if (map->buildingTileMap[i][j] == 1 || map->buildingTileMap[i][j] == 2)
                {
                    RenderRail(position, map->buildingTileMap[i][j]);
                }
                else if (map->buildingTileMap[i][j] == 3)
                {
                    RenderRail(position, 1);
                    RenderRail(position, 2);
                }
            }
            if (map->terrainTileMap[i][j] == 1) // hill
            {
                if (map->buildingTileMap[i][j] == 1 || map->buildingTileMap[i][j] == 2)
                {
                    RenderTunnel(position, map->buildingTileMap[i][j]);
                }
                else if (map->buildingTileMap[i][j] == 3)
                {
                    RenderTunnel(position, 1);
                    RenderTunnel(position, 2);
                }
            }
            if (map->terrainTileMap[i][j] == 2) // water
            {
                if (map->buildingTileMap[i][j] == 1 || map->buildingTileMap[i][j] == 2)
                {
                    RenderBridge(position, map->buildingTileMap[i][j]);
                }
                else if (map->buildingTileMap[i][j] == 3)
                {
                    RenderBridge(position, 1);
                    RenderBridge(position, 2);
                }
            }
        }
    }
}