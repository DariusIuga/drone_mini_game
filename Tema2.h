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

        updateDirection();
    }

    void updateDirection()
    {
        updateForward();
        updateRight();
        updateUp();
    }

    void updateForward()
    {
        forward = glm::normalize(glm::vec3(
            sin(yawAngle),
            0,
            cos(yawAngle)
        ));
    }

    void updateRight()
    {
        right = glm::normalize(glm::vec3(-cos(yawAngle), 0, sin(yawAngle)));
    }

    void updateUp()
    {
        up = glm::normalize(glm::cross(right, forward));
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

    glm::vec3 forward;
    glm::vec3 right;
    glm::vec3 up;

    // Gravity
    constexpr static float gravity = 10;

    // Thrust force applied in the upward direction (starts at 0)
    float thrust = 0;

    constexpr static double boundingSphereRadius = legLength;
};

// Axis aligned bounding box
class AABB
{
public:
    AABB()
    {
        min = glm::vec3(0);
        max = glm::vec3(0);
    }

    AABB(glm::vec3 min, glm::vec3 max)
    {
        this->min = min;
        this->max = max;
    }

    glm::vec3 min;
    glm::vec3 max;
};

class Tree
{
public:
    Tree()
    {
        boundingBox = AABB(position + glm::vec3(-trunkRadius, 0, -trunkRadius),
            position + glm::vec3(trunkRadius,
                trunkHeight + coneHeight * 4 /
                3,
                trunkRadius));
    }

    void calculateModelMatrix(const int nrTilesSide, const int tileLength,
        const int tileWidth)
    {
        this->modelMatrix = glm::mat4(1);

        // Translate to a random position
        float x = rand() % (nrTilesSide * tileLength);
        float z = rand() % (nrTilesSide * tileWidth);

        // Translate to get to the centered location of the ground
        x -= tileLength * nrTilesSide / 2;
        z -= tileWidth * nrTilesSide / 2;
        this->modelMatrix = glm::translate(this->modelMatrix,
            glm::vec3(x, 0, z));

        // Scale up or down randomly
        float scale = 0.5 + (rand() % 20) / 10.0;
        this->modelMatrix = glm::scale(this->modelMatrix, glm::vec3(scale));

        this->position = glm::vec3(x, 0, z);
        generateBoundingBox();
    }

    void generateBoundingBox()
    {
        boundingBox = AABB(position + glm::vec3(-trunkRadius, 0, -trunkRadius),
            position + glm::vec3(trunkRadius,
                trunkHeight + coneHeight * 4 /
                3,
                trunkRadius));
    }

    // Made of a trunk and 2 cones representing the crown
    constexpr static float trunkRadius = 0.2;
    constexpr static float trunkHeight = 1;
    constexpr static float coneRadius = 0.4;
    constexpr static float coneHeight = 1;

    // The coordinates at the center of the bottom face of the trunk
    glm::vec3 position;
    AABB boundingBox;
    glm::mat4 modelMatrix;
};

class Ground
{
public:
    Ground()
    {
        // Generate random number for the number of trees
        srand(time(NULL));
        nr_trees = rand() % 20 + 5;
        trees = new Tree[nr_trees];


        generateTreeMatrices();
    }

    // Function for generating tree matrices
    void generateTreeMatrices()
    {
        // Draw a random number of trees
        for (int i = 0; i < nr_trees; i++)
        {
            trees[i].calculateModelMatrix(nrTilesSide, tileLength, tileWidth);
        }
    }

    constexpr static int tileLength = 6;
    constexpr static int tileWidth = 4;
    constexpr static int nrTilesSide = 8;
    Mesh* mesh;

    // Meshes for a tree
    Mesh* trunk;
    Mesh* cone;

    int nr_trees;

    // Vector of trees
    Tree* trees;
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
            return { r / 256.0, g / 256.0, b / 256.0 };
        }

        // Make sure that the drone stays above the ground and doesn't go
        // outside the bounds of the ground
        void checkOutOfBounds()
        {
            // Don't let the drone go outside the ground
            if (drone.position.y < 0)
            {
                drone.position.y = 0;
            }

            // Put invisible walls around the limits of the ground
            if (drone.position.x < -ground.tileLength * ground.nrTilesSide / 2)
            {
                drone.position.x = -ground.tileLength * ground.nrTilesSide / 2;
            }
            if (drone.position.x > ground.tileLength * ground.nrTilesSide / 2)
            {
                drone.position.x = ground.tileLength * ground.nrTilesSide / 2;
            }
            if (drone.position.z < -ground.tileWidth * ground.nrTilesSide / 2)
            {
                drone.position.z = -ground.tileWidth * ground.nrTilesSide / 2;
            }
            if (drone.position.z > ground.tileWidth * ground.nrTilesSide / 2)
            {
                drone.position.z = ground.tileWidth * ground.nrTilesSide / 2;
            }

            // Put an invisible ceiling for good measure
            if (drone.position.y > 50)
            {
                drone.position.y = 50;
            }
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

        enum CameraMode { FIRST_PERSON, THIRD_PERSON };

        CameraMode cameraMode; // Tracks the current camera mode

        // Mode of drone control
        enum ControlMode { ANGLE, ACRO };
        ControlMode controlMode = ANGLE;
    };
} // namespace m1
