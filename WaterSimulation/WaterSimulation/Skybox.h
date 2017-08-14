#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <GL/glew.h> // Contains all the necessery OpenGL #include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include "Loader.h"

class SkyBox
{
private:
	unsigned int vaoHandle;
	ShaderProgram *shaderProgram;
	GLuint texID;

	GLuint vbo, ibo;
public:
	SkyBox();
	void draw(glm::mat4 & model, glm::mat4 & view, glm::mat4 & projection) const;
};
