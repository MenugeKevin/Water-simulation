#include <iostream>
#include <windows.h>


#include <gl/glew.h>
#include <FL/Fl.H>
#include <FL\Fl_File_Chooser.H>
#include <FL\Fl_Output.H>
#include <FL\Fl_Slider.H>
#include <FL\Fl_Hor_Slider.H>
#include <FL/Fl_Double_Window.H>
#include <Fl/Fl_Gl_Window.H>
#include "Timing.h"

#include "MyGlWindow.h"
#include "WaterValue.h"

void idler(void *w)
{
	MyGlWindow *win = (MyGlWindow*)w;


	_int64 ctime;
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER *>(&ctime));

	int r = 60;  //FrameRate=60frames/sec
	if ((ctime - win->m_clock.etime) < (win->m_clock.freq / r)) return;
	win->m_clock.stop();

	//do something
	win->timePassed += win->m_clock.get();

	win->m_clock.start();
	win->damage(1); //redraw
}

void inputAmplitude_cb(Fl_Widget *widget, void *w)
{
	Fl_Hor_Slider *input = (Fl_Hor_Slider*)widget;
	MyGlWindow *win = (MyGlWindow*)w;

	WaterValue::amplitude = input->value();
	win->damage(1);
}

void inputSpeed_cb(Fl_Widget *widget, void *w)
{
	Fl_Hor_Slider *input = (Fl_Hor_Slider*)widget;
	MyGlWindow *win = (MyGlWindow*)w;

	WaterValue::speed = input->value();
	win->damage(1);
}

void inputWaveLength_cb(Fl_Widget *widget, void *w)
{
	Fl_Hor_Slider *input = (Fl_Hor_Slider*)widget;
	MyGlWindow *win = (MyGlWindow*)w;

	WaterValue::waveLength = input->value();
	win->damage(1);
}

int main()
{
	Fl::scheme("plastic");// plastic
	int width = 800;
	int height = 800;
	Fl_Double_Window* wind = new Fl_Double_Window(100, 100, width, height, "Model Loader");

	wind->begin();		// put widgets inside of the window
	MyGlWindow *gl = new MyGlWindow(10, 10, width - 20, height - 50);

	Fl_Hor_Slider *AmplitudeSlider = new Fl_Hor_Slider(600, height - 35, 100, 10, "Amplitude");
	Fl_Hor_Slider *SpeedSlider = new Fl_Hor_Slider(400, height - 35, 100, 10, "Speed");
	Fl_Hor_Slider *WaveLengthSlider = new Fl_Hor_Slider(200, height - 35, 100, 10, "WaveLength");

	wind->resizable(gl);
	wind->show();	// this actually opens the window

	Fl::add_idle(idler, gl);
	
	// Slider
	AmplitudeSlider->range(0.1f, 10.0f);
	SpeedSlider->range(0.1f, 20.0f);
	WaveLengthSlider->range(1.0f, 64.0f);

	AmplitudeSlider->value(WaterValue::amplitude);
	SpeedSlider->value(WaterValue::speed);
	WaveLengthSlider->value(WaterValue::waveLength);

	AmplitudeSlider->callback(inputAmplitude_cb, gl);
	SpeedSlider->callback(inputSpeed_cb, gl);
	WaveLengthSlider->callback(inputWaveLength_cb, gl);

	Fl::run();
	delete wind;

	return 1;
}