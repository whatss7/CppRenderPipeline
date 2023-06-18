#version 440

layout (location = 0) in vec3 aPos;
layout (location = 0) out vec4 color;

void main()
{
    gl_Position = vec4(aPos.y, aPos.x, aPos.z, 1.0);
    color = vec4((aPos.x + 1.0) / 2.0, (aPos.y + 1.0) / 2.0, (aPos.z + 1.0) / 2.0, 1.0);
}
