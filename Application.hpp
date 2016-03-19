#pragma once
#include <memory>
#include <vector>

#include "Simulation.hpp"
#include "Painter.hpp"
#include "Camera.hpp"

class Application
{
public:
	Application();

	void paint();
	void tick();

	Camera camera;

private:
	Simulation _sim;
	Painter _painter;

	std::vector<Paintable*> _paintables;
};
