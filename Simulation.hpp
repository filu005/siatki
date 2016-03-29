#pragma once

#include "Grid.hpp"
#include "Mesh.hpp"
#include "FVMesh.hpp"

class Simulation
{
public:
	Simulation();

	void run();

	// main components and also Paintables
	Grid grid;
	Mesh mesh;
	FVMesh fvmesh;

private:

};