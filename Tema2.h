#pragma once

#include <iostream>

#include "components/simple_scene.h"
#include "lab_m1/Tema2/lab_camera.h"

class Drone
{
public:
    Drone()
    {
        this->position = glm::vec3(0, 0, 0);
        this->propellerAngle = 0;
        this->pitchAngle = 0;
        this->rollAngle = 0;
        this->yawAngle = 0;
    }

    // Position of the origin
    glm::vec3 position;

    constexpr static double legLength = 2;
    constexpr static double legWidth = 0.1;
    constexpr static double propellerLength = 1;
    constexpr static double propellerWidth = 0.02;
    Mesh* body;
    Mesh* propeller;

    float propellerAngle;

    float pitchAngle;
    float rollAngle;
    float yawAngle;
};

class Ground
{
public:
    Ground()
    {
        // Generate random number for the number of trees
        srand(time(NULL));
        nr_trees = rand() % 50 + 5;
        treeModelMatrices = new glm::mat4[nr_trees];
        generateTreeMatrices();
    }

    // Function for generating tree matrices
    void generateTreeMatrices()
    {
        // Draw a random number of trees
        for (int i = 0; i < nr_trees; i++)
        {
            glm::mat4 treeModelMatrix = glm::mat4(1);

            // Translate to a random position
            float x = rand() % (nrTilesSide * tileLength);
            float z = rand() % (nrTilesSide * tileWidth);
            treeModelMatrix = glm::translate(treeModelMatrix,
                                             glm::vec3(x, 0, z));
            // Translate to get to the centered location of the ground
            treeModelMatrix = glm::translate(
                treeModelMatrix,
                glm::vec3(-tileLength * nrTilesSide / 2, 0,
                          -tileWidth * nrTilesSide / 2));

            // Scale up or down randomly
            float scale = 0.5 + (rand() % 20) / 10.0;
            treeModelMatrix = glm::scale(treeModelMatrix, glm::vec3(scale));

            treeModelMatrices[i] = treeModelMatrix;
        }
    }

    constexpr static int tileLength = 5;
    constexpr static int tileWidth = 3;
    constexpr static int nrTilesSide = 6;
    Mesh* mesh;

    int nr_trees;

    // Matrices used for rendering a bunch of trees
    glm::mat4* treeModelMatrices;
};

class Tree
{
public:
    Tree()
    {
    }

    // Made of a trunk and 2 cones representing the crown
    constexpr static float trunkRadius = 0.2;
    constexpr static float trunkHeight = 1;
    constexpr static float coneRadius = 0.4;
    constexpr static float coneHeight = 1;

    Mesh* trunk;
    Mesh* cone;
};

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

        void RenderMesh(Mesh* mesh, Shader* shader,
                        const glm::mat4& modelMatrix) override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX,
                         int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button,
                             int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button,
                               int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX,
                           int offsetY) override;
        void OnWindowResize(int width, int height) override;

        // Function that takes 3 color arguments and returns a glm::vec3
        // glm::vec3 groundColor(1.0 / 255 * 120, 1.0 / 255 * 150, 1.0 / 255 * 100);
        static glm::vec3 rgbToVec3(const int r, const int g, const int b)
        {
            return {r / 256.0, g / 256.0, b / 256.0};
        }

    protected:
        implemented::Camera* camera;
        glm::mat4 projectionMatrix;
        bool renderCameraTarget;

        // TODO(student): If you need any other class variables, define them here.
        bool orthoProjection;
        float projectionFov, projectionWidth, projectionHeight;

        Ground ground;
        Drone drone;
        Tree tree;
    };
} // namespace m1
