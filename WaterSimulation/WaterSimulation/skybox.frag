#version 440

in vec4 Position;
in vec3 ReflectDir;

uniform samplerCube CubeMapTex;

layout(location = 0) out vec4 FragColors;

void main()
{
	vec4 CubeMapColor = texture(CubeMapTex, ReflectDir);

	FragColors = CubeMapColor;
}