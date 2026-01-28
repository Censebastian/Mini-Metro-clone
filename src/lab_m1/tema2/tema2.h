#pragma once

#include "components/simple_scene.h"
#include "lab_m1/tema2/camera.h"
#include "lab_m1/tema2/terrain.h"
#include "lab_m1/tema2/map.h"
#include "lab_m1/tema2/transform.h"
#include "lab_m1/tema2/path.h"
#include "lab_m1/tema2/station.h"
#include "lab_m1/tema2/train.h"
#include "lab_m1/tema2/generate_meshes.h"

namespace m1
{
    class Tema2 : public gfxc::SimpleScene
    {
     public:
        Tema2();
        ~Tema2();

        void Init() override;

     private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void RenderMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix) override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

        void MousePick(int mouseX, int mouseY);
        void UpdatePaths();
        void UpdateTime(float deltaTimeSeconds);
        void UpdateStations();
        void SpawnStation();
        void UpdateTrains(float deltaTimeSeconds);

        void RenderLocomotive(glm::vec3 position, float orientation);
        void RenderWagon(glm::vec3 position, float orientation);

        void RenderStationSquare(glm::vec3 position);
        void RenderStationTriangle(glm::vec3 position);
        void RenderStationCircular(glm::vec3 position);

        void RenderRail(glm::vec3 position, int orientation);
        void RenderRailCross(glm::vec3 position, int orientation);
        void RenderTunnel(glm::vec3 position, int orientation);
        void RenderBridge(glm::vec3 position, int orientation);
        void RenderBridgeCross(glm::vec3 position, int orientation);

        void RenderRails();

     protected:
        implemented::MyCamera *camera;
        glm::mat4 projectionMatrix;

        Terrain* terrain;
        Map* map;
        int terrainSize;
        int mapSize;
        float tileLength;

        std::vector<Station> stations;

        int buildingMode;
        bool canPlaceRail;
        bool canRemoveRail;
        Mesh* highlightTile;

        glm::vec3 cursorIntersection;
        std::vector<Path> paths;

        bool pause;
        bool tutorial;
        bool gameOver;

        float time;
        int timeMultiplier;
        int difficulty;

        std::vector<Train> trains;

        float trainLength;
        float trainWidth;
        float railLength;
        float railWidth;
        float stationSize;
    };
}