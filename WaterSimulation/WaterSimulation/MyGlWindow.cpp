
#include <iostream>
#include "MyGlWindow.h"

static double DEFAULT_VIEW_POINT[3] = { 5, 5, 5 };
static double DEFAULT_VIEW_CENTER[3] = { 0, 0, 0 };
static double DEFAULT_UP_VECTOR[3] = { 0, 1, 0 };

MyGlWindow::MyGlWindow(int x, int y, int w, int h)
	: Fl_Gl_Window(x, y, w, h)
	//==========================================================================
{

	mode(FL_RGB | FL_ALPHA | FL_DOUBLE | FL_STENCIL);
	first = 0;
	glm::vec3 viewPoint(DEFAULT_VIEW_POINT[0], DEFAULT_VIEW_POINT[1], DEFAULT_VIEW_POINT[2]);
	glm::vec3 viewCenter(DEFAULT_VIEW_CENTER[0], DEFAULT_VIEW_CENTER[1], DEFAULT_VIEW_CENTER[2]);
	glm::vec3 upVector(DEFAULT_UP_VECTOR[0], DEFAULT_UP_VECTOR[1], DEFAULT_UP_VECTOR[2]);

	float aspect = (w / (float)h);
	m_viewer = new Viewer(viewPoint, viewCenter, upVector, 45.0f, aspect);
}

MyGlWindow::~MyGlWindow()
{

}

void MyGlWindow::draw(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);

	if (!first) {
		glClearColor(0.2f, 0.2f, .2f, 0);
		glewExperimental = TRUE;
		GLenum err = glewInit();
		if (err != GLEW_OK)
		{
			//Problem: glewInit failed, something is seriously wrong.
			std::cout << "glewInit failed, aborting." << std::endl;
		}

		first = 1;
		initialize();
	}

	glm::vec3 eye = m_viewer->getViewPoint(); // m_viewer->getViewPoint().x(), m_viewer->getViewPoint().y(), m_viewer->getViewPoint().z());
	glm::vec3 look = m_viewer->getViewCenter();   //(m_viewer->getViewCenter().x(), m_viewer->getViewCenter().y(), m_viewer->getViewCenter().z());
	glm::vec3 up = m_viewer->getUpVector(); // m_viewer->getUpVector().x(), m_viewer->getUpVector().y(), m_viewer->getUpVector().z());

	glm::mat4 view = glm::lookAt(eye, look, up);

	glm::mat4 projection = glm::perspective(45.0f, 1.0f*w() / h(), 0.1f, 500.0f);

	m_model.glPushMatrix();
	m_model.glTranslate(0, 10, 0);
	if (m_skybox)
	{
		m_skybox->draw(m_model.getMatrix(), view, projection);
	}
	m_model.glPopMatrix();
	if (m_water)
	{
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
		m_water->draw(m_model.getMatrix(), view, projection, timePassed);
		glDisable(GL_BLEND);
	}
	glViewport(0, 0, w(), h());
}

void MyGlWindow::initialize()
{
	m_water = new Water(64, 128);
	m_skybox = new SkyBox();
}

int m_pressedMouseButton;
int m_lastMouseX;
int m_lastMouseY;

int MyGlWindow::handle(int e)
//==========================================================================
{
	switch (e) {
	case FL_SHOW:		// you must handle this, or not be seen!
		show();
		return 1;
	case FL_PUSH:
	{

		m_pressedMouseButton = Fl::event_button();

		//	  m_viewer->setAspectRatio( static_cast<double>(this->w()) / static_cast<double>(this->h()) );

		m_lastMouseX = Fl::event_x();
		m_lastMouseY = Fl::event_y();
	}
	damage(1);
	return 1;
	case FL_RELEASE:
		m_pressedMouseButton = -1;
		damage(1);
		return 1;
	case FL_DRAG: // if the user drags the mouse
	{

		double fractionChangeX = static_cast<double>(Fl::event_x() - m_lastMouseX) / static_cast<double>(this->w());
		double fractionChangeY = static_cast<double>(m_lastMouseY - Fl::event_y()) / static_cast<double>(this->h());

		if (m_pressedMouseButton == 1) {
			m_viewer->rotate(fractionChangeX, fractionChangeY);
		}
		else if (m_pressedMouseButton == 2) {
			m_viewer->zoom(fractionChangeY);
		}
		else if (m_pressedMouseButton == 3) {
			m_viewer->translate(-fractionChangeX, -fractionChangeY, (Fl::event_key(FL_Shift_L) == 0) || (Fl::event_key(FL_Shift_R) == 0));
		}
		else {
			std::cout << "Warning: dragging with unknown mouse button!  Nothing will be done" << std::endl;
		}

		m_lastMouseX = Fl::event_x();
		m_lastMouseY = Fl::event_y();
		redraw();
	}

	return 1;

	case FL_KEYBOARD:
		return 0;

	default:
		return 0;
	}
}