#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 color;
layout(location = 3) in vec2 texCoords;

out vec3 fragColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Output to fragment shader
out float noiseValue;

// 2D Random
float random(in vec2 pos) {
    return fract(sin(dot(pos.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}

// Simple noise function (placeholder)
float noise(in vec2 pos) {
    vec2 i = floor(pos);
    vec2 f = fract(pos);

    // Four corners in 2D of a tile
    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));

    // Smooth Interpolation
    vec2 u = f * f * (3.0 - 2.0 * f);

    // Mix 4 corners percentages
    return mix(a, b, u.x) +
           (c - a) * u.y * (1.0 - u.x) +
           (d - b) * u.x * u.y;
}

void main()
{
    float frequency = 5.0; // Adjust this value to change the noise frequency
    noiseValue = noise(grid_position.xz * frequency);

    // Modify the y-component based on noise
    vec3 modifiedPosition = position;
    modifiedPosition.y += noiseValue * 2.0; // Scale the height as needed

    fragColor = color;
    // Final vertex position
    gl_Position = projection * view * model * vec4(modifiedPosition, 1.0);
}