#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 objectColor;

// Jeszcze to powinienem dostawać jeśli wszystko działa:
in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

// Dodatkowe parametry mgły
float fogStart = 1.5;
float fogEnd = 8.0;

void main()
{
    // Odczytaj kolor tekstury
    vec4 textureColor = texture(texture_diffuse1, TexCoords);
    
    // Pobierz pierwsze 3 elementy koloru tekstury
    vec3 texCol = textureColor.rgb;

    // Ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    // Kombinacja efektów oświetlenia i koloru tekstury
    vec3 result = (ambient + diffuse + specular) * texCol;

    // Kalkulator mgły
    float distance = length(FragPos - viewPos);
    float fogFactor = smoothstep(fogStart, fogEnd, distance);
    vec3 fogColor = vec3(0.8, 0.8, 0.8); // Kolor mgły

    // Zastosowanie efektu mgły
    result = mix(result, fogColor, fogFactor);

    FragColor = vec4(result, 1.0);
}
