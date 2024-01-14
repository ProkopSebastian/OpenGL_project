#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

// Dodatkowy out do przekazania koloru wierzchołka do fragment shadera
out vec3 VertexColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform sampler2D texture_diffuse1;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoords = aTexCoords;

    // Vertex color from texture
    vec4 texCol = texture(texture_diffuse1, TexCoords);
    VertexColor = texCol.rgb; // Przykładowy kolor (pomarańczowy)

    gl_Position = projection * view * vec4(FragPos, 1.0);
}
