#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

struct DirectLight{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform DirectLight directLight;
uniform vec3 viewPos;
void main(void)
{
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(-directLight.direction);

    vec3 ambient = directLight.ambient * vec3(texture(texture_diffuse1, TexCoords));

    float diff = max(dot(lightDir, norm), 0.0);
    vec3 diffuse = directLight.diffuse * diff *	vec3(texture(texture_diffuse1, TexCoords));

    vec3 viewDir = normalize( viewPos - FragPos );
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(reflectDir, viewDir), 0.0), 32);
    vec3 specular = directLight.specular * spec * vec3(texture(texture_diffuse1, TexCoords));

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0f);
}
