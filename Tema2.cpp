#include "lab_m1/Tema2/Tema2.h"

#include <vector>
#include <string>
#include <iostream>

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema2::Tema2()
{
    Ground();
    Drone();
    Tree();
}


Tema2::~Tema2()
{
}

// Create a cylinder with the specified radius and height
Mesh* createCylinder(float radius, float height, glm::vec3 color)
{
    vector<VertexFormat> vertices;
    vector<unsigned int> indices;

    // Create the vertices
    int numSlices = 100;
    for (int i = 0; i < numSlices; i++)
    {
        float theta = 2 * M_PI * i / numSlices;
        float x = radius * cos(theta);
        float z = radius * sin(theta);

        vertices.push_back(VertexFormat(glm::vec3(x, 0, z), color));
        vertices.push_back(VertexFormat(glm::vec3(x, height, z), color));
    }

    // Add center vertices for bottom and top faces
    vertices.push_back(VertexFormat(glm::vec3(0, 0, 0), color));
    // Bottom center
    vertices.push_back(VertexFormat(glm::vec3(0, height, 0), color));
    // Top center

    int centerBottomIndex = vertices.size() - 2;
    int centerTopIndex = vertices.size() - 1;

    // Create the indices
    for (int i = 0; i <= numSlices; i++)
    {
        int current = i % numSlices;
        indices.push_back(2 * current);
        indices.push_back(2 * current + 1);
    }

    // Add indices for bottom face
    for (int i = 0; i < numSlices; i++)
    {
        indices.push_back(centerBottomIndex);
        indices.push_back(2 * i);
        indices.push_back((2 * (i + 1)) % (2 * numSlices));
    }

    // Add indices for top face
    for (int i = 0; i < numSlices; i++)
    {
        indices.push_back(centerTopIndex);
        indices.push_back((2 * i + 1));
        indices.push_back((2 * (i + 1) + 1) % (2 * numSlices));
    }

    Mesh* mesh = new Mesh("cylinder");
    mesh->SetDrawMode(GL_TRIANGLE_STRIP); // Change draw mode to triangle strip
    mesh->InitFromData(vertices, indices);
    return mesh;
}

// Create a cone with the specified radius and height
Mesh* createCone(float radius, float height, glm::vec3 color)
{
    vector<VertexFormat> vertices;
    vector<unsigned int> indices;

    // Create the vertices
    int numSlices = 100;
    for (int i = 0; i < numSlices; i++)
    {
        float theta = 2 * M_PI * i / numSlices;
        float x = radius * cos(theta);
        float z = radius * sin(theta);

        vertices.push_back(VertexFormat(glm::vec3(x, 0, z), color));
    }

    // Add center vertices for bottom face
    vertices.push_back(VertexFormat(glm::vec3(0, 0, 0), color));
    // Bottom center

    int centerBottomIndex = vertices.size() - 1;

    // Add apex vertex
    vertices.push_back(VertexFormat(glm::vec3(0, height, 0), color)); // Apex

    int apexIndex = vertices.size() - 1;

    // Create side indices using triangle fan
    for (int i = 0; i <= numSlices; i++)
    {
        int current = i % numSlices;
        indices.push_back(apexIndex);
        indices.push_back(current);
    }

    // Create the indices
    for (int i = 0; i < numSlices; i++)
    {
        indices.push_back(i);
    }

    // Add indices for bottom face
    for (int i = 0; i < numSlices; i++)
    {
        indices.push_back(centerBottomIndex);
        indices.push_back(i);
        indices.push_back((i + 1) % numSlices);
    }

    Mesh* mesh = new Mesh("cone");
    mesh->SetDrawMode(GL_TRIANGLE_FAN); // Change draw mode to triangle fan
    mesh->InitFromData(vertices, indices);
    return mesh;
}


void Tema2::Init()
{
    renderCameraTarget = false;
    orthoProjection = false;
    projectionFov = 60.0f;
    projectionWidth = 16.0f;
    projectionHeight = 9.0f;

    camera = new implemented::Camera();
    camera->Set(glm::vec3(0, 2, 3.5f), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));

    // Create ground mesh

    vector<VertexFormat> groundVertices;
    glm::vec3 groundColor = rgbToVec3(40, 160, 35);
    vector<unsigned int> groundIndices;
    for (int i = 0; i <= ground.nrTilesSide; i++)
    {
        for (int j = 0; j <= ground.nrTilesSide; j++)
        {
            groundVertices.push_back(
                VertexFormat(glm::vec3(ground.tileLength * i, 0,
                                       ground.tileWidth * j),
                             groundColor));
        }
    }

    // Add indices for tile triangles

    for (int i = 0; i < ground.nrTilesSide; i++)
    {
        for (int j = 0; j < ground.nrTilesSide; j++)
        {
            int topLeft = i * (ground.nrTilesSide + 1) + j;
            int topRight = topLeft + 1;
            int bottomLeft = topLeft + ground.nrTilesSide + 1;
            int bottomRight = bottomLeft + 1;

            // First triangle
            groundIndices.push_back(topLeft);
            groundIndices.push_back(bottomLeft);
            groundIndices.push_back(topRight);

            // Second triangle
            groundIndices.push_back(topRight);
            groundIndices.push_back(bottomLeft);
            groundIndices.push_back(bottomRight);
        }
    }

    ground.mesh = new Mesh("ground");
    ground.mesh->SetDrawMode(GL_TRIANGLES);
    ground.mesh->InitFromData(groundVertices, groundIndices);
    meshes[ground.mesh->GetMeshID()] = ground.mesh;


    // Create a mesh for the drone body and one for the propeller

    // Body
    vector<VertexFormat> bodyVertices;
    glm::vec3 bodyColor = rgbToVec3(100, 100, 100);
    vector<unsigned int> bodyIndices;

    // Add points for the body
    float halfLength = Drone::legLength;
    float halfWidth = Drone::legWidth;

    bodyVertices = {
        // First parallelepiped
        VertexFormat(glm::vec3(-halfLength, -halfWidth, -halfWidth), bodyColor),
        VertexFormat(glm::vec3(halfLength, -halfWidth, -halfWidth), bodyColor),
        VertexFormat(glm::vec3(halfLength, -halfWidth, halfWidth), bodyColor),
        VertexFormat(glm::vec3(-halfLength, -halfWidth, halfWidth), bodyColor),
        VertexFormat(glm::vec3(-halfLength, halfWidth, -halfWidth), bodyColor),
        VertexFormat(glm::vec3(halfLength, halfWidth, -halfWidth), bodyColor),
        VertexFormat(glm::vec3(halfLength, halfWidth, halfWidth), bodyColor),
        VertexFormat(glm::vec3(-halfLength, halfWidth, halfWidth), bodyColor),

        // Second parallelepiped
        VertexFormat(glm::vec3(-halfWidth, -halfWidth, -halfLength), bodyColor),
        VertexFormat(glm::vec3(halfWidth, -halfWidth, -halfLength), bodyColor),
        VertexFormat(glm::vec3(halfWidth, -halfWidth, halfLength), bodyColor),
        VertexFormat(glm::vec3(-halfWidth, -halfWidth, halfLength), bodyColor),
        VertexFormat(glm::vec3(-halfWidth, halfWidth, -halfLength), bodyColor),
        VertexFormat(glm::vec3(halfWidth, halfWidth, -halfLength), bodyColor),
        VertexFormat(glm::vec3(halfWidth, halfWidth, halfLength), bodyColor),
        VertexFormat(glm::vec3(-halfWidth, halfWidth, halfLength), bodyColor),

        // +X arm top cube (vertices [48..55])
        VertexFormat(
            glm::vec3(halfLength - 2 * halfWidth, halfWidth, -halfWidth),
            bodyColor),
        VertexFormat(glm::vec3(halfLength, halfWidth, -halfWidth), bodyColor),
        VertexFormat(glm::vec3(halfLength, halfWidth, halfWidth), bodyColor),
        VertexFormat(
            glm::vec3(halfLength - 2 * halfWidth, halfWidth, halfWidth),
            bodyColor),
        VertexFormat(glm::vec3(halfLength - 2 * halfWidth, 3 * halfWidth,
                               -halfWidth), bodyColor),
        VertexFormat(glm::vec3(halfLength, 3 * halfWidth, -halfWidth),
                     bodyColor),
        VertexFormat(glm::vec3(halfLength, 3 * halfWidth, halfWidth),
                     bodyColor),
        VertexFormat(glm::vec3(halfLength - 2 * halfWidth, 3 * halfWidth,
                               halfWidth), bodyColor),

        // -X arm top cube (vertices [56..63])
        VertexFormat(glm::vec3(-(halfLength - 2 * halfWidth), halfWidth,
                               -halfWidth), bodyColor),
        VertexFormat(glm::vec3(-halfLength, halfWidth, -halfWidth), bodyColor),
        VertexFormat(glm::vec3(-halfLength, halfWidth, halfWidth), bodyColor),
        VertexFormat(
            glm::vec3(-(halfLength - 2 * halfWidth), halfWidth, halfWidth),
            bodyColor),
        VertexFormat(glm::vec3(-(halfLength - 2 * halfWidth), 3 * halfWidth,
                               -halfWidth), bodyColor),
        VertexFormat(glm::vec3(-halfLength, 3 * halfWidth, -halfWidth),
                     bodyColor),
        VertexFormat(glm::vec3(-halfLength, 3 * halfWidth, halfWidth),
                     bodyColor),
        VertexFormat(glm::vec3(-(halfLength - 2 * halfWidth), 3 * halfWidth,
                               halfWidth), bodyColor),

        // +Z arm top cube (vertices [64..71])
        VertexFormat(
            glm::vec3(-halfWidth, halfWidth, halfLength - 2 * halfWidth),
            bodyColor),
        VertexFormat(
            glm::vec3(halfWidth, halfWidth, halfLength - 2 * halfWidth),
            bodyColor),
        VertexFormat(glm::vec3(halfWidth, halfWidth, halfLength), bodyColor),
        VertexFormat(glm::vec3(-halfWidth, halfWidth, halfLength), bodyColor),
        VertexFormat(glm::vec3(-halfWidth, 3 * halfWidth,
                               halfLength - 2 * halfWidth), bodyColor),
        VertexFormat(glm::vec3(halfWidth, 3 * halfWidth,
                               halfLength - 2 * halfWidth), bodyColor),
        VertexFormat(glm::vec3(halfWidth, 3 * halfWidth, halfLength),
                     bodyColor),
        VertexFormat(glm::vec3(-halfWidth, 3 * halfWidth, halfLength),
                     bodyColor),

        // -Z arm top cube (vertices [72..79])
        VertexFormat(glm::vec3(-halfWidth, halfWidth,
                               -(halfLength - 2 * halfWidth)), bodyColor),
        VertexFormat(
            glm::vec3(halfWidth, halfWidth, -(halfLength - 2 * halfWidth)),
            bodyColor),
        VertexFormat(glm::vec3(halfWidth, halfWidth, -halfLength), bodyColor),
        VertexFormat(glm::vec3(-halfWidth, halfWidth, -halfLength), bodyColor),
        VertexFormat(glm::vec3(-halfWidth, 3 * halfWidth,
                               -(halfLength - 2 * halfWidth)), bodyColor),
        VertexFormat(glm::vec3(halfWidth, 3 * halfWidth,
                               -(halfLength - 2 * halfWidth)), bodyColor),
        VertexFormat(glm::vec3(halfWidth, 3 * halfWidth, -halfLength),
                     bodyColor),
        VertexFormat(glm::vec3(-halfWidth, 3 * halfWidth, -halfLength),
                     bodyColor)
    };

    // Indices for the body
    bodyIndices = {
        // First parallelepiped
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4,
        0, 1, 5, 5, 4, 0,
        2, 3, 7, 7, 6, 2,
        0, 3, 7, 7, 4, 0,
        1, 2, 6, 6, 5, 1,

        // Second parallelepiped
        8, 9, 10, 10, 11, 8,
        12, 13, 14, 14, 15, 12,
        8, 9, 13, 13, 12, 8,
        10, 11, 15, 15, 14, 10,
        8, 11, 15, 15, 12, 8,
        9, 10, 14, 14, 13, 9,

        // Indices for the +X arm cube (correspond to vertices [16..23])
        16, 17, 18, 18, 19, 16,
        20, 21, 22, 22, 23, 20,
        16, 17, 21, 21, 20, 16,
        18, 19, 23, 23, 22, 18,
        16, 19, 23, 23, 20, 16,
        17, 18, 22, 22, 21, 17,

        // Indices for the -X arm cube (correspond to vertices [24..31])
        24, 25, 26, 26, 27, 24,
        28, 29, 30, 30, 31, 28,
        24, 25, 29, 29, 28, 24,
        26, 27, 31, 31, 30, 26,
        24, 27, 31, 31, 28, 24,
        25, 26, 30, 30, 29, 25,

        // Indices for the +Z arm cube (correspond to vertices [32..39])
        32, 33, 34, 34, 35, 32,
        36, 37, 38, 38, 39, 36,
        32, 33, 37, 37, 36, 32,
        34, 35, 39, 39, 38, 34,
        32, 35, 39, 39, 36, 32,
        33, 34, 38, 38, 37, 33,

        // Indices for the -Z arm cube (correspond to vertices [40..47])
        40, 41, 42, 42, 43, 40,
        44, 45, 46, 46, 47, 44,
        40, 41, 45, 45, 44, 40,
        42, 43, 47, 47, 46, 42,
        40, 43, 47, 47, 44, 40,
        41, 42, 46, 46, 45, 41,

        // Indices for the 4 new cubes
        // +X top cube ([48..55])
        48, 49, 50, 50, 51, 48, 52, 53, 54, 54, 55, 52, 48, 49, 53, 53, 52, 48,
        50, 51, 55, 55, 54, 50, 48, 51, 55, 55, 52, 48, 49, 50, 54, 54, 53, 49,

        // -X top cube ([56..63])
        56, 57, 58, 58, 59, 56, 60, 61, 62, 62, 63, 60, 56, 57, 61, 61, 60, 56,
        58, 59, 63, 63, 62, 58, 56, 59, 63, 63, 60, 56, 57, 58, 62, 62, 61, 57,

        // +Z top cube ([64..71])
        64, 65, 66, 66, 67, 64, 68, 69, 70, 70, 71, 68, 64, 65, 69, 69, 68, 64,
        66, 67, 71, 71, 70, 66, 64, 67, 71, 71, 68, 64, 65, 66, 70, 70, 69, 65,

        // -Z top cube ([72..79])
        72, 73, 74, 74, 75, 72, 76, 77, 78, 78, 79, 76, 72, 73, 77, 77, 76, 72,
        74, 75, 79, 79, 78, 74, 72, 75, 79, 79, 76, 72, 73, 74, 78, 78, 77, 73
    };

    drone.body = new Mesh("body");
    drone.body->SetDrawMode(GL_TRIANGLES);
    drone.body->InitFromData(bodyVertices, bodyIndices);
    meshes[drone.body->GetMeshID()] = drone.body;

    {
        glm::vec3 propellerColor = rgbToVec3(0, 0, 0);
        double halfLength = Drone::propellerLength;
        double halfWidth = Drone::propellerWidth;
        // Black color for propellers
        vector<VertexFormat> propellerVertices = {
            // Propeller vertices
            VertexFormat(glm::vec3(-halfWidth, 0, -halfLength), propellerColor),
            VertexFormat(glm::vec3(halfWidth, 0, -halfLength), propellerColor),
            VertexFormat(glm::vec3(halfWidth, 0, halfLength), propellerColor),
            VertexFormat(glm::vec3(-halfWidth, 0, halfLength), propellerColor),
            VertexFormat(glm::vec3(-halfWidth, halfWidth, -halfLength),
                         propellerColor),
            VertexFormat(glm::vec3(halfWidth, halfWidth, -halfLength),
                         propellerColor),
            VertexFormat(glm::vec3(halfWidth, halfWidth, halfLength),
                         propellerColor),
            VertexFormat(glm::vec3(-halfWidth, halfWidth, halfLength),
                         propellerColor)
        };

        vector<unsigned int> propellerIndices = {
            0, 1, 2, 2, 3, 0,
            4, 5, 6, 6, 7, 4,
            0, 1, 5, 5, 4, 0,
            2, 3, 7, 7, 6, 2,
            0, 3, 7, 7, 4, 0,
            1, 2, 6, 6, 5, 1
        };

        drone.propeller = new Mesh("propeller");
        drone.propeller->SetDrawMode(GL_TRIANGLES);
        drone.propeller->InitFromData(propellerVertices, propellerIndices);
        meshes[drone.propeller->GetMeshID()] = drone.propeller;
    }

    // Mesh for a tree
    {
        glm::vec3 trunkColor = rgbToVec3(139, 69, 19);
        float trunkRadius = Tree::trunkRadius;
        float trunkHeight = Tree::trunkHeight;
        glm::vec3 crownColor = rgbToVec3(9, 74, 7);

        tree.trunk = createCylinder(tree.trunkRadius, tree.trunkHeight,
                                    trunkColor);
        meshes["trunk"] = tree.trunk;
        tree.cone = createCone(tree.coneRadius, tree.coneHeight, crownColor);
        meshes["cone"] = tree.cone;
    }
}


void Tema2::FrameStart()
{
    projectionMatrix = orthoProjection
                           ? glm::ortho(-projectionWidth / 2,
                                        projectionWidth / 2,
                                        -projectionHeight / 2,
                                        projectionHeight / 2, .01f, 200.0f)
                           : glm::perspective(
                               RADIANS(projectionFov),
                               window->props.aspectRatio, 0.01f, 200.0f);
    // Clears the color buffer (using the previously set color) and depth buffer

    // Sky color
    glm::vec3 skyColor = rgbToVec3(135, 206, 235);
    glClearColor(skyColor.r, skyColor.g, skyColor.b, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}


void Tema2::Update(float deltaTimeSeconds)
{
    // TODO(student): Draw more objects with different model matrices.
    // Attention! The `RenderMesh()` function overrides the usual
    // `RenderMesh()` that we've been using up until now. This new
    // function uses the view matrix from the camera that you just
    // implemented, and the local projection matrix.

    // The roll, pitch and yaw angles should get closer to 0 on every frame
    // (the drone should stabilize itself).

    float stabilizationSpeed = 5.0f;
    drone.rollAngle = glm::mix(drone.rollAngle, 0.0f,
                               stabilizationSpeed * deltaTimeSeconds);
    drone.pitchAngle = glm::mix(drone.pitchAngle, 0.0f,
                                stabilizationSpeed * deltaTimeSeconds);
    drone.yawAngle = glm::mix(drone.yawAngle, 0.0f,
                              stabilizationSpeed * deltaTimeSeconds);

    {
        glm::mat4 groundModelMatrix = glm::mat4(1);
        // Move the ground's center to (0, 0, 0)
        groundModelMatrix = glm::translate(groundModelMatrix,
                                           glm::vec3(-ground.tileLength *
                                               ground.nrTilesSide / 2,
                                               0,
                                               -ground.tileWidth *
                                               ground.nrTilesSide / 2));
        RenderMesh(meshes["ground"], shaders["VertexColor"], groundModelMatrix);
    }

    glm::mat4 droneModelMatrix = glm::mat4(1);
    droneModelMatrix = glm::translate(droneModelMatrix, drone.position);
    droneModelMatrix = glm::rotate(droneModelMatrix, drone.rollAngle,
                                   glm::vec3(0, 0, 1));
    droneModelMatrix = glm::rotate(droneModelMatrix, drone.pitchAngle,
                                   glm::vec3(1, 0, 0));
    droneModelMatrix = glm::rotate(droneModelMatrix, drone.yawAngle,
                                   glm::vec3(0, 1, 0));

    {
        RenderMesh(meshes["body"], shaders["VertexColor"], droneModelMatrix);
    }

    // Update the angle of the propellers
    constexpr double propellerSpeed = 120;
    drone.propellerAngle += deltaTimeSeconds * propellerSpeed;

    // Draw a propeller on top of each arm
    // For arm +X
    {
        glm::mat4 modelMatrix = droneModelMatrix;
        modelMatrix = glm::translate(modelMatrix,
                                     glm::vec3(
                                         (Drone::legLength - Drone::legWidth),
                                         3 * Drone::legWidth, 0));
        modelMatrix = glm::rotate(modelMatrix, drone.propellerAngle,
                                  glm::vec3(0, 1, 0));
        RenderMesh(meshes["propeller"], shaders["VertexColor"], modelMatrix);
    }

    // For arm -X
    {
        glm::mat4 modelMatrix = droneModelMatrix;
        modelMatrix = glm::translate(modelMatrix,
                                     glm::vec3(
                                         -(Drone::legLength - Drone::legWidth),
                                         3 * Drone::legWidth, 0));
        modelMatrix = glm::rotate(modelMatrix, drone.propellerAngle,
                                  glm::vec3(0, 1, 0));
        RenderMesh(meshes["propeller"], shaders["VertexColor"], modelMatrix);
    }

    // For arm +Z
    {
        glm::mat4 modelMatrix = droneModelMatrix;
        modelMatrix = glm::translate(modelMatrix,
                                     glm::vec3(0, 3 * Drone::legWidth,
                                               (Drone::legLength -
                                                   Drone::legWidth)));
        modelMatrix = glm::rotate(modelMatrix, drone.propellerAngle,
                                  glm::vec3(0, 1, 0));
        RenderMesh(meshes["propeller"], shaders["VertexColor"], modelMatrix);
    }

    // For arm -Z
    {
        glm::mat4 modelMatrix = droneModelMatrix;
        modelMatrix = glm::translate(modelMatrix,
                                     glm::vec3(0, 3 * Drone::legWidth,
                                               -(Drone::legLength -
                                                   Drone::legWidth)));
        modelMatrix = glm::rotate(modelMatrix, drone.propellerAngle,
                                  glm::vec3(0, 1, 0));
        RenderMesh(meshes["propeller"], shaders["VertexColor"], modelMatrix);
    }

    // Render the camera target. This is useful for understanding where
    // the rotation point is, when moving in third-person camera mode.
    if (renderCameraTarget)
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition());
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));
        RenderMesh(meshes["sphere"], shaders["VertexNormal"], modelMatrix);
    }

    // Draw trees using the saved model matrices
    for (int i = 0; i < ground.nr_trees; i++)
    {
        glm::mat4 currentModelMatrix = ground.treeModelMatrices[i];
        // Draw the shapes that make up the tree
        RenderMesh(meshes["trunk"], shaders["VertexColor"], currentModelMatrix);
        currentModelMatrix = glm::translate(currentModelMatrix,
                                            glm::vec3(0, Tree::trunkHeight, 0));
        RenderMesh(meshes["cone"], shaders["VertexColor"], currentModelMatrix);
        currentModelMatrix = glm::translate(currentModelMatrix,
                                            glm::vec3(
                                                0, Tree::coneHeight / 3, 0));
        RenderMesh(meshes["cone"], shaders["VertexColor"], currentModelMatrix);
    }
}


void Tema2::FrameEnd()
{
    DrawCoordinateSystem(camera->GetViewMatrix(), projectionMatrix);
}


void Tema2::RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix)
{
    if (!mesh || !shader || !shader->program)
        return;

    // Render an object using the specified shader and the specified position
    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE,
                       glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE,
                       glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE,
                       glm::value_ptr(modelMatrix));

    mesh->Render();
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Tema2::OnInputUpdate(float deltaTime, int mods)
{
    constexpr double speed = 5;

    // move the camera only if MOUSE_RIGHT button is pressed
    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
    {
        float cameraSpeed = 2.0f;

        if (window->KeyHold(GLFW_KEY_W))
        {
            camera->TranslateForward(deltaTime * cameraSpeed);
        }

        if (window->KeyHold(GLFW_KEY_A))
        {
            camera->TranslateRight(-deltaTime * cameraSpeed);
        }

        if (window->KeyHold(GLFW_KEY_S))
        {
            camera->TranslateForward(-deltaTime * cameraSpeed);
        }

        if (window->KeyHold(GLFW_KEY_D))
        {
            camera->TranslateRight(deltaTime * cameraSpeed);
        }

        if (window->KeyHold(GLFW_KEY_Q))
        {
            camera->TranslateUpward(-deltaTime * cameraSpeed);
        }

        if (window->KeyHold(GLFW_KEY_E))
        {
            camera->TranslateUpward(deltaTime * cameraSpeed);
        }
    }
    else
    {
        // Move the drone

        if (window->KeyHold(GLFW_KEY_W))
        {
            drone.position.z -= deltaTime * speed;
            drone.pitchAngle -= deltaTime * speed;
        }
        if (window->KeyHold(GLFW_KEY_S))
        {
            drone.position.z += deltaTime * speed;
            drone.pitchAngle += deltaTime * speed;
        }
        if (window->KeyHold(GLFW_KEY_A))
        {
            drone.position.x -= deltaTime * speed;
            drone.rollAngle += deltaTime * speed;
        }
        if (window->KeyHold(GLFW_KEY_D))
        {
            drone.position.x += deltaTime * speed;
            drone.rollAngle -= deltaTime * speed;
        }
        if (window->KeyHold(GLFW_KEY_E))
        {
            drone.position.y += deltaTime * speed;
        }
        if (window->KeyHold(GLFW_KEY_Q))
        {
            drone.position.y -= deltaTime * speed;
        }
        // Just change direction/yaw
        if (window->KeyHold(GLFW_KEY_Z))
        {
            drone.yawAngle += deltaTime * speed;
        }
        if (window->KeyHold(GLFW_KEY_X))
        {
            drone.yawAngle -= deltaTime * speed;
        }
    }

    if (window->KeyHold(GLFW_KEY_1))
    {
        projectionFov -= deltaTime * 10;
    }
    if (window->KeyHold(GLFW_KEY_2))
    {
        projectionFov += deltaTime * 10;
    }

    if (window->KeyHold(GLFW_KEY_3))
    {
        projectionWidth += 5 * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_4))
    {
        projectionWidth -= 5 * deltaTime;
    }

    if (window->KeyHold(GLFW_KEY_5))
    {
        projectionHeight += 5 * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_6))
    {
        projectionHeight -= 5 * deltaTime;
    }

    // TODO(student): Change projection parameters. Declare any extra
    // variables you might need in the class header. Inspect this file
    // for any hardcoded projection arguments (can you find any?) and
    // replace them with those extra variables.
}


void Tema2::OnKeyPress(int key, int mods)
{
    // Add key press event
    if (key == GLFW_KEY_T)
    {
        renderCameraTarget = !renderCameraTarget;
    }
    // TODO(student): Switch projections
    if (key == GLFW_KEY_O) orthoProjection = true;
    else if (key == GLFW_KEY_P) orthoProjection = false;
}


void Tema2::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event

    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
    {
        float sensivityOX = 0.001f;
        float sensivityOY = 0.001f;

        if (window->GetSpecialKeyState() == 0)
        {
            renderCameraTarget = false;
            // TODO(student): Rotate the camera in first-person mode around
            // OX and OY using `deltaX` and `deltaY`. Use the sensitivity
            // variables for setting up the rotation speed.
            camera->RotateFirstPerson_OX(-deltaY * sensivityOX);
            camera->RotateFirstPerson_OY(-deltaX * sensivityOY);
        }

        if (window->GetSpecialKeyState() & GLFW_MOD_CONTROL)
        {
            renderCameraTarget = true;
            // TODO(student): Rotate the camera in third-person mode around
            // OX and OY using `deltaX` and `deltaY`. Use the sensitivity
            // variables for setting up the rotation speed.
            camera->RotateThirdPerson_OX(-deltaY * sensivityOX);
            camera->RotateThirdPerson_OY(-deltaX * sensivityOY);
        }
    }
}


void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
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
