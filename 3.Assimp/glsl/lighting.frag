#version 330 core

out vec4 FragColor;

struct Material{
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct DirectLight{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

in vec3 FragPos;
in vec3 Normal;
in vec3 LightPos;   // extra in variable, since we need the light position in view space we calculate this in the vertex shader
in vec2 TexCoords;

#define NR_POINT_LIGHTS 4

uniform	Material material;
uniform	Material materialflash;
uniform DirectLight directLight;
uniform PointLight pointLights[4];
uniform SpotLight spotLight;

uniform vec3 viewPos;

vec3 calculateDirectLight()
{
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(-directLight.direction);
    // ambient
    vec3 ambient = directLight.ambient * vec3(texture(material.diffuse, TexCoords));

    // diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = directLight.diffuse * diff * vec3(texture(material.diffuse, TexCoords));

    // specular
    vec3 viewDir = normalize(viewPos - FragPos );
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = directLight.specular * spec * vec3(texture(material.specular, TexCoords));

    vec3 result = ambient + diffuse + specular;
    return result;
}

vec3 calculatePointLight(PointLight pointLight)
{
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(pointLight.position - FragPos);
    // ambient
    vec3 ambient = pointLight.ambient * vec3(texture(material.diffuse, TexCoords));

    // diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = pointLight.diffuse * diff * vec3(texture(material.diffuse, TexCoords));

    // specular
    vec3 viewDir = normalize(viewPos - FragPos );
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = pointLight.specular * spec * vec3(texture(material.specular, TexCoords));

    float distance    = length(spotLight.position - FragPos);
    float attenuation = 1.0 / (spotLight.constant + spotLight.linear * distance +
			spotLight.quadratic * (distance * distance));

    //ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    vec3 result = ambient + diffuse + specular;
    return result;
}

vec3 calculateSpotLight(Material material1)
{
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(spotLight.position - FragPos);

    // ambient
    vec3 ambient = spotLight.ambient * vec3(texture(material1.diffuse, TexCoords));

    // diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = spotLight.diffuse * diff * vec3(texture(material1.diffuse, TexCoords));

    // specular
    vec3 viewDir = normalize(viewPos-FragPos); // the viewer is always at (0,0,0) in view-space, so viewDir is (0,0,0) - Position => -Position
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material1.shininess);
    vec3 specular = spotLight.specular * spec * vec3(texture(material1.specular, TexCoords));


    float distance    = length(spotLight.position - FragPos);
    float attenuation = 1.0 / (spotLight.constant + spotLight.linear * distance +
			spotLight.quadratic * (distance * distance));

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    float costheta     = dot(lightDir, normalize(-spotLight.direction));
    float epsilon = spotLight.cutOff - spotLight.outerCutOff;
    float intensity = clamp((costheta - spotLight.outerCutOff) / epsilon, 0.0, 1.0 );

    diffuse *= intensity;
    specular *= intensity;
    vec3 result = ambient + diffuse + specular;
    return result;
}

void main()
{
    vec3 result = vec3(0,0,0);
    result += calculateSpotLight(material);
    for(int i = 0; i < 4; i++)
	result += calculatePointLight(pointLights[i]);
    result += calculateDirectLight();
    FragColor = vec4(result, 1.0);

}
