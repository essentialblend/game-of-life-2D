#version 460 core

layout (location = 0) in vec3 aPos;

uniform mat4 scaleMatrix;
uniform mat4 projMat;

void main() {

    gl_Position = projMat * scaleMatrix * vec4(aPos.x , aPos.y, aPos.z, 1.0f);
}