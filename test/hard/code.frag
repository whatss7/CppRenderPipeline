#version 440

layout (location = 0) in vec4 color;
layout (location = 1) in vec4 texCoord;
layout (location = 0) out vec4 FragColor;

void main()
{
    vec4 temp = color;
    if (texCoord.x > 0) {
        temp.z = temp.x;
    } else {
        temp.z = temp.y;
    }
    FragColor = temp;
}
