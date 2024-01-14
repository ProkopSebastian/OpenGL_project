#version 330 core
out vec4 FragColor;

in vec3 LightingColor;
in vec2 TexCoords;

uniform vec3 objectColor;
uniform sampler2D texture1;

void main()
{
   vec4 objectColorFromTexture = texture(texture1, TexCoords);
   vec3 objectColorFromTextureVec3 = objectColorFromTexture.rgb;
   FragColor = vec4(LightingColor * objectColorFromTextureVec3 , 1.0); // zamiast objectColor ściągam z tekstoru (powinno tak być?)
}