#pragma once

#include "Grid.hpp"
#include "Mesh.hpp"


class Simulation
{
public:
	Simulation();

	void run();

	// main components and also Paintables
	Grid grid;
	Mesh mesh;

private:

};