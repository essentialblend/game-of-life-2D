#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aOffset;
layout (location = 2) in ivec2 aCellIndex;

uniform mat4 scaleMatrix;
uniform vec2 translationOffset;

flat out ivec2 vCellIndex;

void main() {
    
    vec4 scaledPos = scaleMatrix * vec4(aPos, 1.f);
    scaledPos.xy += aOffset + translationOffset;
    vCellIndex = aCellIndex;
    gl_Position = scaledPos;
}