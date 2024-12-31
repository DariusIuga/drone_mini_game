#version 330 core

in vec3 fragColor;
out vec4 color;

void main()
{
    // Use the interpolated vertex color
    color = vec4(fragColor, 1.0);
}