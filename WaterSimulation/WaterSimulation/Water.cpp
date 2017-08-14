
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SOIL\SOIL.h>

#include "Water.h"
#include "WaterValue.h"

float WaterValue::amplitude = 0.5f;
float WaterValue::speed = 2.0f;
float WaterValue::waveLength = 32.0f;

Water::Water(float size, int nSquares)
{
	////////////////////////////////////////////////
	setup(size, nSquares);
}

glm::vec3 Water::computeNormal(glm::vec3 &p1, glm::vec3 &p2, glm::vec3 &p3)
{
	glm::vec3 v1;
	glm::vec3 v2;

	v1 = p2 - p1;
	v2 = p3 - p1;
	return (glm::normalize(glm::cross(v1, v2)));
}

void Water::setup(float size, int nSquares)
{
	std::vector<glm::vec3> vlists;
	std::vector<glm::vec3> nlists;
	std::vector<glm::vec2> tlists;

	std::vector<glm::vec3> tangentsLists;
	std::vector<glm::vec3> bitangentsLists;

	m_size = size;

	// parameters:
	float maxX = size / 2, maxY = size / 2;
	float minX = -size / 2, minY = -size / 2;

	int x, y, v[3], i;
	float xp, yp, xd, yd;
	v[2] = 0;
	xd = (maxX - minX) / ((float)nSquares);
	yd = (maxY - minY) / ((float)nSquares);


	// Loop creating the plane
	for (x = 0, xp = minX; x < nSquares; x++, xp += xd) {
		for (y = 0, yp = minY, i = x; y < nSquares; y++, i++, yp += yd) {
			vlists.push_back(glm::vec3(xp, 0, yp));
			vlists.push_back(glm::vec3(xp, 0, yp + yd));
			vlists.push_back(glm::vec3(xp + xd, 0, yp + yd));
			

			vlists.push_back(glm::vec3(xp, 0, yp));
			vlists.push_back(glm::vec3(xp + xd, 0, yp + yd));
			vlists.push_back(glm::vec3(xp + xd, 0, yp));
		} // end of for j
	}// end of for i

	for (int i = 0; i < vlists.size(); i += 3)
	{
		nlists.push_back(computeNormal(vlists[i], vlists[i + 1], vlists[i + 2]));
		nlists.push_back(computeNormal(vlists[i], vlists[i + 1], vlists[i + 2]));
		nlists.push_back(computeNormal(vlists[i], vlists[i + 1], vlists[i + 2]));
	}
	for (int i = 0; i < vlists.size(); ++i)
	{
		tlists.push_back(glm::vec2(vlists[i].r, vlists[i].b));
	}
	nvert = vlists.size();

	for (int i = 0; i < nvert; i += 3)
	{
		glm::vec3 & v0 = vlists[i + 0];
		glm::vec3 & v1 = vlists[i + 1];
		glm::vec3 & v2 = vlists[i + 2];

		// Shortcuts for UVs
		glm::vec2 & uv0 = tlists[i + 0];
		glm::vec2 & uv1 = tlists[i + 1];
		glm::vec2 & uv2 = tlists[i + 2];

		// Edges of the triangle : postion delta
		glm::vec3 deltaPos1 = v1 - v0;
		glm::vec3 deltaPos2 = v2 - v0;

		// UV delta
		glm::vec2 deltaUV1 = uv1 - uv0;
		glm::vec2 deltaUV2 = uv2 - uv0;

		float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
		glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y)*r;
		glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x)*r;

		tangentsLists.push_back(tangent);
		tangentsLists.push_back(tangent);
		tangentsLists.push_back(tangent);

		bitangentsLists.push_back(bitangent);
		bitangentsLists.push_back(bitangent);
		bitangentsLists.push_back(bitangent);
	}

	shaderProgram = new ShaderProgram();

	//load shaders
	shaderProgram->initFromFiles("water.vert", "water.frag");

	// create texture object
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(2, tex_2d);  //tex_2d is a member variable
	glBindTexture(GL_TEXTURE_2D, tex_2d[0]);

	// set parameters
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


	// Read an image
	int width, height;
	unsigned char * diffuseTex = SOIL_load_image("water.png", &width, &height, 0,
		SOIL_LOAD_RGBA);

	// Link the image with the tex obj
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
		GL_UNSIGNED_BYTE, diffuseTex);
	glGenerateMipmap(GL_TEXTURE_2D);

	// release the tex
	SOIL_free_image_data(diffuseTex);

	glBindTexture(GL_TEXTURE_2D, tex_2d[1]);

	// set parameters
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Read an image
	unsigned char * normalTex = SOIL_load_image("normal.png", &width, &height, 0,
		SOIL_LOAD_RGBA);


	// Link the image with the tex obj
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
		GL_UNSIGNED_BYTE, normalTex);
	glGenerateMipmap(GL_TEXTURE_2D);

	// release the tex
	SOIL_free_image_data(normalTex);

	glBindTexture(GL_TEXTURE_2D, 0);


	//add attributes and uniform vars
	shaderProgram->addAttribute("VertexPosition");
	shaderProgram->addAttribute("VertexNormal");
	shaderProgram->addAttribute("VertexTexCoord");
	shaderProgram->addAttribute("VertexTangent");
	shaderProgram->addAttribute("VertexBitTangent");

	shaderProgram->addUniform("mvp");
	shaderProgram->addUniform("NormalMatrix");

	shaderProgram->addUniform("Light.Position");
	shaderProgram->addUniform("Light.Intensity");

	shaderProgram->addUniform("Material.Ka");
	shaderProgram->addUniform("Material.Ks");
	shaderProgram->addUniform("Material.Shiness");

	shaderProgram->addUniform("Texture.texture_diffuse");
	shaderProgram->addUniform("Texture.texture_normal");

	shaderProgram->addUniform("PassedTime");
	shaderProgram->addUniform("WaveParameters");
	shaderProgram->addUniform("WaveDirections");

	//create vao
	glGenVertexArrays(1, &vaoHandle);
	glBindVertexArray(vaoHandle);

	//create vbo for vertices
	glGenBuffers(1, &vbo_water_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_water_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vlists.size() * 3, vlists.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(
		shaderProgram->attribute("VertexPosition"), // attribute
		3,                 // number of elements per vertex, here (x,y,z,1)
		GL_FLOAT,          // the type of each element
		GL_FALSE,          // take our values as-is
		0,                 // no extra data between each position
		0                  // offset of first element
	);
	glEnableVertexAttribArray(shaderProgram->attribute("VertexPosition"));

	glGenBuffers(1, &vbo_water_normals);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_water_normals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * nlists.size(), nlists.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(shaderProgram->attribute("VertexNormal"), 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(shaderProgram->attribute("VertexNormal"));

	glGenBuffers(1, &vbo_water_textures);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_water_textures);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * tlists.size(), tlists.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(
		shaderProgram->attribute("VertexTexCoord"),
		2,
		GL_FLOAT,
		GL_FALSE,
		0,
		0
	);
	glEnableVertexAttribArray(shaderProgram->attribute("VertexTexCoord"));


	glGenBuffers(1, &vbo_water_tangents);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_water_tangents);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * tangentsLists.size(), tangentsLists.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(
		shaderProgram->attribute("VertexTangent"),
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		0
	);
	glEnableVertexAttribArray(shaderProgram->attribute("VertexTangent"));

	glGenBuffers(1, &vbo_water_bittangents);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_water_bittangents);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * bitangentsLists.size(), bitangentsLists.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(
		shaderProgram->attribute("VertexBitTangent"),
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		0
	);
	glEnableVertexAttribArray(shaderProgram->attribute("VertexBitTangent"));

	vlists.clear();

	glBindVertexArray(0);
}

void Water::initRenderWater()
{
	GLfloat overallSteepness = 0.2f;

	memset(m_waves, 0, sizeof(m_waves));
	memset(m_directions, 0, sizeof(m_directions));

	for (int i = 0; i < MAX_WAVES; ++i)
	{
		m_waves[i].Speed = 1.0f + WaterValue::speed * i;
		m_waves[i].Amplitude = WaterValue::amplitude / (i + 1);
		m_waves[i].WaveLength = WaterValue::waveLength * M_PI / (i + 1);
		m_waves[i].Steepness = overallSteepness / (m_waves[i].WaveLength * m_waves[i].Amplitude * (GLfloat)MAX_WAVES);
		m_directions[i].x = +1.0f;
		m_directions[i].z = +1.5f;
	}
}

void Water::draw(glm::mat4 & model, glm::mat4 & view, glm::mat4 & projection, GLfloat _deltaTime)
{
	glm::mat4 mview = view * model;
	glm::mat4 mvp = projection * view * model;

	glm::vec4 lightPos(10, 40, -10, 1);
	glm::vec3 Ka(0, 0.25, 0.45);
	glm::vec3 Ks(0.95, 0.94, 0.95);

	GLfloat Shiness = 32;

	glm::vec3 lightIntensity(0.7f, 0.7f, 0.7f);
	glm::mat4 imvp = glm::inverse(mview);
	glm::mat3 nmat = glm::mat3(glm::transpose(imvp));  //normal matrix

	shaderProgram->use();
	initRenderWater();

	glUniform4fv(shaderProgram->uniform("Light.Position"), 1, glm::value_ptr(view * lightPos));
	glUniform3fv(shaderProgram->uniform("Light.Intensity"), 1, glm::value_ptr(lightIntensity));

	glUniform3fv(shaderProgram->uniform("Material.Ka"), 1, glm::value_ptr(Ka));
	glUniform3fv(shaderProgram->uniform("Material.Ks"), 1, glm::value_ptr(Ks));
	glUniform1fv(shaderProgram->uniform("Material.Shiness"), 1, &Shiness);

	glUniformMatrix3fv(shaderProgram->uniform("NormalMatrix"), 1, GL_FALSE, glm::value_ptr(nmat));  //normalMatrix
	glUniformMatrix4fv(shaderProgram->uniform("mvp"), 1, GL_FALSE, glm::value_ptr(mvp));

	glUniform1f(shaderProgram->uniform("PassedTime"), _deltaTime);
	glUniform4fv(shaderProgram->uniform("WaveParameters"), 4 * MAX_WAVES, (GLfloat*)m_waves);
	glUniform2fv(shaderProgram->uniform("WaveDirections"), 2 * MAX_WAVES, (GLfloat*)m_directions);

	glEnable(GL_TEXTURE_2D);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_2d[0]);
	glUniform1i(shaderProgram->uniform("Texture.texture_diffuse"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex_2d[1]);
	glUniform1i(shaderProgram->uniform("Texture.texture_normal"), 1);

	glBindVertexArray(vaoHandle);
	glDrawArrays(GL_TRIANGLES, 0, nvert * 3);
	glBindTexture(GL_TEXTURE_2D, 0);

	shaderProgram->disable();

}