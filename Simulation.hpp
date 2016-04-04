#pragma once
#include <chrono>
#include <fstream>
#include <sstream>

#include "Grid.hpp"
#include "Mesh.hpp"
#include "FVMesh.hpp"
#include "HEMesh.hpp"

class Simulation
{
public:
	Simulation();
	~Simulation();

	void run();
	//tutaj z powodu kolejnoœci na liœcie inicjalizacyjnej konstruktora
private:
	std::string mesh_filename;

public:
	// main components and also Paintables
	Grid grid;
	Mesh mesh;
	FVMesh fvmesh;
	HEMesh hemesh;

private:
	std::chrono::time_point<std::chrono::system_clock> start, end;
	std::ofstream stats_file;
};


// only for stats output
namespace
{
	std::string statsToString(const std::pair<std::string, float>& data)
	{
		std::ostringstream str;
		str << data.first << "\t" << data.second;
		return str.str();
	}
} // namespace anonymous