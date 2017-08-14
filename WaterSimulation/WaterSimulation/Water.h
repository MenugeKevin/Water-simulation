#ifndef FLOOR_H_
#define FLOOR_H_

#define M_PI 3.14159265358979323846

#include <GL/glew.h>
#include <glm/mat4x4.hpp>

#include "Loader.h"

const int MAX_WAVES = 8;

struct WaveParameters
{
	GLfloat WaveLength;
	GLfloat Amplitude;
	GLfloat Speed;
	GLfloat Steepness;
} typedef Wave_t;

struct WaveDirections
{
	GLfloat x;
	GLfloat z;
} typedef Dir_t;

class Water {
public:
	Water(float size, int nSquares);
	void setup(float size, int nSquares);
	void draw(glm::mat4 & model, glm::mat4 & view, glm::mat4 &projection, GLfloat _deltaTime);
	glm::vec3 computeNormal(glm::vec3 &p1, glm::vec3 &p2, glm::vec3 &p3);
	void initRenderWater();
private:
	GLuint vaoHandle;
	GLuint vbo_water_vertices, vbo_water_normals, vbo_water_textures, vbo_water_tangents, vbo_water_bittangents;
	ShaderProgram *shaderProgram;
	int nvert;
	glm::vec3 waterColor1;
	glm::vec3 waterColor2;
	GLuint tex_2d[2];
	Wave_t m_waves[MAX_WAVES];
	Dir_t m_directions[MAX_WAVES];
	int m_size;
};

#endif // !1
