#version 330 core
out vec4 FragColor;

void main()
{
    //FragColor = mix( texture(tex_container, TexCoord), texture(tex_thumb, TexCoord), 0.9);
    FragColor = vec4(1,1,1,1);
}
