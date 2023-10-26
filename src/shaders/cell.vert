#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aOffset;

uniform mat4 scaleMatrix;
uniform vec2 translationOffset;

void main() {
    
    vec4 scaledPos = scaleMatrix * vec4(aPos, 1.f);
    scaledPos.xy += aOffset + translationOffset;
    gl_Position = scaledPos;
}