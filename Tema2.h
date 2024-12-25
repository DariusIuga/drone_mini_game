#pragma once

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
    }

    constexpr static int tileLength = 10;
    constexpr static int tileWidth = 10;
    constexpr static int nrTilesSide = 3;
    Mesh* mesh;
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

        void RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix) override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

        // Function that takes 3 color arguments and returns a glm::vec3
        // glm::vec3 groundColor(1.0 / 255 * 120, 1.0 / 255 * 150, 1.0 / 255 * 100);
        static glm::vec3 rgbToVec3(const int r, const int g, const int b)
        {
            return { r / 256.0, g / 256.0, b / 256.0 };
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

    };
}   // namespace m1