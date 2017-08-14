#pragma once

#include <windows.h>

#include "GL/glew.h"
#include <GL/gl.h>
#include <string>
#include <stack>

#include <FL/Fl_Gl_Window.h>
#include <Fl/Fl.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"

#include "Viewer.h"
#include "ModelView.h"
#include "Water.h"
#include "Skybox.h"
#include "Timing.h"

class MyGlWindow : public Fl_Gl_Window {
public:
	MyGlWindow(int x, int y, int w, int h);
	~MyGlWindow();

	Clock m_clock;
	GLfloat timePassed;

private:
	void draw();
	int handle(int);
	void initialize();
	int first;

	Model m_model;
	Viewer *m_viewer;
	Water *m_water;
	SkyBox *m_skybox;

};