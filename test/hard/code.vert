#version 440

layout (location = 0) in vec3 aPos;
layout (location = 0) out vec4 color;
layout (location = 1) out vec4 texCoord;

void main()
{
    vec3 temp = (aPos + 1.0) / 2.0;
    color = vec4(temp.x, temp.y, (temp.x + temp.y) / 2, 1.0);

    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
    texCoord = gl_Position;
}
