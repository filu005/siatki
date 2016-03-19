#include "Application.hpp"

Application::Application()
{
	//_paintables.push_back(&_sim.grid);
	_paintables.push_back(&_sim.mesh);

	_painter.set_camera(camera);
}

void Application::paint(void)
{
	for(auto & p : _paintables)
		p->paint(_painter);
}


void Application::tick(void)
{
	_sim.run();
}
