#version 330 core
layout (location = 0) in vec3 aPos;   // the position variable has attribute position 0
layout (location = 1) in vec3 aColor; // the color variable has attribute position 
layout (location = 2) in vec2 aTexCoord;
out vec3 ourColor;
out vec2 TexCoord;
uniform mat4 mvp;
void main()
{
    gl_Position = mvp * vec4(aPos, 1.0);
    ourColor = aColor;
    TexCoord = aTexCoord;
}
