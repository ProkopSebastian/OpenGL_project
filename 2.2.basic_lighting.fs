#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 objectColor;

// jeszcze to powinienem dostawaæ jeœli wszystko dzia³a:
in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

void main()
{
    //
    vec4 textureColor = texture(texture_diffuse1, TexCoords);
    vec3 texCol = textureColor.rgb; // pierwsze 3 elementy tekstury

    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // specular
    float specularStrength = 1.0;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * texCol; // zamiast objectColor
    FragColor = vec4(result, 1.0);
} 