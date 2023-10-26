#version 460 core

layout (location = 0) in vec3 aPos;

uniform mat4 scaleMatrix;

void main() {

    gl_Position = scaleMatrix * vec4(aPos.x , aPos.y, aPos.z, 1.0f);
}