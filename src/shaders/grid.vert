#version 440 core

layout (location = 0) in vec3 aPos;

uniform float gridxOffset;
uniform float gridyOffset;

uniform mat4 scaleMatrix;

void main() {

    gl_Position = scaleMatrix * vec4(aPos.x + gridxOffset, aPos.y + gridyOffset, aPos.z, 1.0f);
}