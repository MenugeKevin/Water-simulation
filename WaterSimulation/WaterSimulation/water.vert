#version 440

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoord;
layout (location = 3) in vec3 VertexTangent;
layout (location = 4) in vec3 VertexBitTangent;

out vec4 Position;
out vec3 Normal;
out vec2 TexCoord;
out vec3 Tangent;
out vec3 BitTangent;

uniform mat4 mvp;
uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;

// Water parameters

const float PI = 3.14159265358979323846;
uniform float WaterPlaneLength;

uniform float PassedTime;

uniform vec4 WaveParameters[8];
uniform vec2 WaveDirections[8];


float wave(int i, float x, float y) {
    float frequency = 2*PI/WaveParameters[i].x;
    float phase = WaveParameters[i].z * frequency;
    float theta = dot(WaveDirections[i], vec2(x, y));
    return WaveParameters[i].y * sin(theta * frequency + PassedTime * phase);
}

float waveHeight(float x, float y) {
    float height = 0.0;
    for (int i = 0; i < 8; ++i)
        height += wave(i, x, y);
    return height;
}

float dWavedx(int i, float x, float y) {
    float frequency = 2*PI/WaveParameters[i].x;
    float phase = WaveParameters[i].z * frequency;
    float theta = dot(WaveDirections[i], vec2(x, y));
    float A = WaveParameters[i].y * WaveDirections[i].x * frequency;
    return A * cos(theta * frequency + PassedTime * phase);
}

float dWavedy(int i, float x, float y) {
    float frequency = 2*PI/WaveParameters[i].x;
    float phase = WaveParameters[i].z * frequency;
    float theta = dot(WaveDirections[i], vec2(x, y));
    float A = WaveParameters[i].y * WaveDirections[i].y * frequency;
    return A * cos(theta * frequency + PassedTime * phase);
}

vec3 waveNormal(float x, float y) {
    float dx = 0.0;
    float dy = 0.0;
    for (int i = 0; i < 8; ++i) {
        dx += dWavedx(i, x, y);
        dy += dWavedy(i, x, y);
    }
    vec3 n = vec3(-dx, 1.0, -dy);
    return normalize(n);
}

void main(void)
{
	vec4 pos = vec4(VertexPosition, 1.0f);
    pos.y = 0.05f + waveHeight(pos.x, pos.z);
	vec3 position = pos.xyz / pos.w;
	vec3 worldNormal = waveNormal(pos.x, pos.z);
	vec3 eyeNormal = NormalMatrix * worldNormal;

	TexCoord = VertexTexCoord;
	Position = vec4(position, 1.0f);
	Normal = normalize(eyeNormal);
	Tangent = normalize(NormalMatrix * VertexTangent);
	BitTangent = normalize(NormalMatrix * VertexBitTangent);
	gl_Position = mvp * pos;
}
