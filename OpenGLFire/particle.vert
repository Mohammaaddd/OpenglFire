#version 330 core

layout (location = 0) in vec2 aPos;

uniform vec2 particlePosition;
uniform float particleSize;

out vec2 localPosition;

void main()
{
    localPosition = aPos;

    gl_Position = vec4(aPos * particleSize + particlePosition, 0.0, 1.0);
}