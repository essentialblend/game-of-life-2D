#version 460 core

flat in ivec2 vCellIndex;

uniform isampler2D cellLifeStates;

out vec4 FragColor;


void main() {

    int lifeState = texelFetch(cellLifeStates, vCellIndex.yx, 0).r;

    if (lifeState == 0)
    {
        FragColor = vec4(0.824f, 0.824f, 0.824f, 1.0f);
    }
    else if (lifeState == 1)
    {
        FragColor = vec4(1.f, 0.0f, 0.0f, 1.0f);
    }
    
    //FragColor = vec4(0.824f, 0.824f, 0.824f, 1.0f);
}