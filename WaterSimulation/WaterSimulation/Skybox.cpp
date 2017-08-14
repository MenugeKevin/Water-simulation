#include "SOIL\SOIL.h"
#include "glm\gtc\type_ptr.hpp"
#include "Skybox.h"

SkyBox::SkyBox()
{
	float side = 64.0f;   //size of cube
	float side2 = side / 2.0f;
	float v[24 * 3] = {
		// Front
		-side2, -side2, side2,
		side2, -side2, side2,
		side2,  side2, side2,
		-side2,  side2, side2,
		// Right
		side2, -side2, side2,
		side2, -side2, -side2,
		side2,  side2, -side2,
		side2,  side2, side2,
		// Back
		-side2, -side2, -side2,
		-side2,  side2, -side2,
		side2,  side2, -side2,
		side2, -side2, -side2,
		// Left
		-side2, -side2, side2,
		-side2,  side2,  side2,
		-side2,  side2, -side2,
		-side2, -side2, -side2,
		// Bottom
		-side2, -side2, side2,
		-side2, -side2, -side2,
		side2, -side2, -side2,
		side2, -side2, side2,
		// Top
		-side2,  side2, side2,
		side2,  side2, side2,
		side2,  side2, -side2,
		-side2,  side2, -side2
	};

	unsigned int el[] = {
		0,2,1,    0,3,2,    4,6,5,     4,7,6,
		8,10,9,   8,11,10,  12,14,13,  12,15,14,
		16,18,17, 16,19,18, 20,22,21,  20,23,22
	};

	shaderProgram = new ShaderProgram();

	shaderProgram->initFromFiles("skybox.vert", "skybox.frag");

	shaderProgram->addAttribute("vertexPosition");

	shaderProgram->addUniform("mvp");
	shaderProgram->addUniform("ModelViewMatrix");  //View*Model : mat4
	shaderProgram->addUniform("CubeMapTex");  //cubemap texture

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &texID);  //set the texID as a member variable
	glBindTexture(GL_TEXTURE_CUBE_MAP, texID);

	const char * suffixes[] = { "left", "right", "top", "down", "back", "front" };

	GLuint targets[] = {
		GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
	};

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	for (int i = 0; i < 6; i++) {
		int channel;
		int width, height;
		std::string filename;
		filename = std::string(suffixes[i]) + std::string(".tga");
		unsigned char * image = SOIL_load_image(filename.c_str(), &width, &height, &channel, SOIL_LOAD_RGB);
		glTexImage2D(targets[i], 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);
		SOIL_free_image_data(image);
	}
	

	glGenVertexArrays(1, &vaoHandle);
	glBindVertexArray(vaoHandle);

	//create vbo for vertices
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * 24, v, GL_STATIC_DRAW);
	glVertexAttribPointer(
		shaderProgram->attribute("vertexPosition"), // attribute
		3,                 // number of elements per vertex, here (x,y,z,1)
		GL_FLOAT,          // the type of each element
		GL_FALSE,          // take our values as-is
		0,                 // no extra data between each position
		0                  // offset of first element
	);
	glEnableVertexAttribArray(shaderProgram->attribute("vertexPosition"));

	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * 3 * 12, el, GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void SkyBox::draw(glm::mat4 & model, glm::mat4 & view, glm::mat4 & projection) const
{
	glm::mat4 mview = view * model;
	glm::mat4 mvp = projection * view * model;

	shaderProgram->use();

	glUniformMatrix4fv(shaderProgram->uniform("mvp"), 1, GL_FALSE, glm::value_ptr(mvp));
	glUniformMatrix4fv(shaderProgram->uniform("ModelViewMatrix"), 1, GL_FALSE, glm::value_ptr(mview));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texID);
	glUniform1i(shaderProgram->uniform("CubeMapTex"), 0);

	glBindVertexArray(vaoHandle);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	shaderProgram->disable();
}