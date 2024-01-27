#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D tex_thumb;
uniform sampler2D tex_container;
uniform float   alpha;
void main()
{
    //FragColor = mix( texture(tex_container, TexCoord), texture(tex_thumb, TexCoord), 0.9);
    FragColor = texture(tex_container, TexCoord) * ( 1 - alpha ) + texture(tex_thumb, TexCoord) * alpha;
}
