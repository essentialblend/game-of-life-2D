#version 460 core
out vec4 FragColor;

uniform bool isCellAlive;

void main() {
    if(isCellAlive)
    {
        FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
    }
    else
    {
        FragColor = vec4(0.824f, 0.824f, 0.824f, 1.0f);
    }
}