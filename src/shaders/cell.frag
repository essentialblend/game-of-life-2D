#version 460 core

flat in ivec2 vCellIndex;

uniform isampler2D cellLifeStates;

out vec4 FragColor;


void main() {

    vec3 offColor = vec3(0.824f, 0.824f, 0.824f);
    vec3 onColor = vec3(1.f, 0.f, 0.f);

    int lifeState = texelFetch(cellLifeStates, vCellIndex.yx, 0).r;

    FragColor = vec4(lifeState == 1 ? onColor : offColor, 1.f);
    
    //FragColor = vec4(0.824f, 0.824f, 0.824f, 1.0f);
}