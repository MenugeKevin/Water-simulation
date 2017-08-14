#version 440

vec4 fogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);
const float maxDist = 50.0f;
const float minDist = 1.0f;

in vec4 Position;
in vec3 Normal;
in vec2 TexCoord;
in vec3 Tangent;
in vec3 BitTangent;

struct LightInfo {
	vec4 Position;
	vec3 Intensity;
};
uniform LightInfo Light;

struct MaterialInfo {
  vec3 Ka;      
  vec3 Ks;       
  float Shiness;
};
uniform MaterialInfo Material;

struct TextureInfo {
	sampler2D texture_diffuse; 
	sampler2D texture_specular; 
	sampler2D texture_normal;
};
uniform TextureInfo Texture;

layout(location = 0) out vec4 FragColors;


void main()
{
	vec4 finalcolor = vec4(1.0);

	mat3 tangentMatrix = mat3(Tangent.x, BitTangent.x, Normal.x,
							  Tangent.y, BitTangent.y, Normal.y,
							  Tangent.z, BitTangent.z, Normal.z);
	
	vec3 P = vec3(Position);
	vec3 V = normalize(tangentMatrix * vec3(-Position));
	vec3 N = texture(Texture.texture_normal, TexCoord).rgb;
	N = normalize(N * 2.0 - 1.0);
	vec3 L = normalize(tangentMatrix * vec3(Light.Position - Position));
	vec4 Kd = texture(Texture.texture_diffuse, TexCoord);
	vec3 R = normalize(reflect(-L, N));

	vec3 ambient = Light.Intensity * Material.Ka;
	vec3 diffuse = Light.Intensity * Kd.xyz * max(dot(L, N), 0);
	vec3 spec = Light.Intensity * Material.Ks * pow(max(dot(R, V), 0.0), Material.Shiness);

	vec4 ambAndDif = vec4(ambient + diffuse, 1.0);

	finalcolor = vec4(ambient + diffuse + spec, 1.0f);
	
	float z = abs(Position.z);
	float f = (maxDist - z) / (maxDist - minDist);

	f = clamp(f, 0.0, 1.0);
	vec4 color = mix(fogColor, finalcolor, f);
	color.a = 0.75f;
	FragColors = color;
}