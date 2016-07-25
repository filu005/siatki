#include "Simulation.hpp"
#include <functional>
#include <iostream>

Simulation::Simulation() :
	mesh_filename("bunny"), // nazwa pliku z modelem (bez rozszerzenia)
	mesh(mesh_filename + std::string(".obj")),
	fvmesh(mesh),
	hemesh(mesh),
	stats_file("./../stats/" + mesh_filename + ".txt") // œcie¿ka do pliku ze statystykami
{
	auto feature_func = &FVMesh::edge_border; // wywo³ywana funkcja

	//https://kjellkod.wordpress.com/2014/04/30/c11-template-tricks-finding-the-return-value-type-for-member-function/
	using func_return_type = std::result_of<decltype(feature_func)(FVMesh)>::type;
	// std::function musi byæ explicit bo argument std::function
	// w FVMesh::get_feature(std::function) jest szablonem
	// i podczepia siê pod wszystko co mo¿liwe
	auto func = std::function<func_return_type()>(std::bind(feature_func, fvmesh));
	
	start = std::chrono::system_clock::now();
	//hemesh.flip_edges();
	fvmesh.get_feature(func); // wywo³anie funkcji
	end = std::chrono::system_clock::now();

	std::chrono::duration<double> elapsed_seconds = end - start;
	stats_file << elapsed_seconds.count() << std::endl; // wypisanie do pliku; w sekundach
}

Simulation::~Simulation()
{
	stats_file.close();
}

void Simulation::run()
{
	
}