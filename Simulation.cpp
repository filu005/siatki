#include "Simulation.hpp"
#include <functional>

Simulation::Simulation() :
	mesh(),
	fvmesh(mesh)
{
	auto feature_func = &FVMesh::face_neighbour_2_ring;

	//https://kjellkod.wordpress.com/2014/04/30/c11-template-tricks-finding-the-return-value-type-for-member-function/
	using func_return_type = std::result_of<decltype(feature_func)(FVMesh)>::type;
	// std::function musi byæ explicit bo argument std::function
	// w FVMesh::get_feature(std::function) jest szablonem
	// i podczepia siê pod wszystko co mo¿liwe
	auto func = std::function<func_return_type()>(std::bind(feature_func, fvmesh));
	fvmesh.get_feature(func);
}

void Simulation::run()
{
	
}