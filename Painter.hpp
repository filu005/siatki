#pragma once
#include "Shader.hpp"

class Grid;
class Mesh;
class FVMesh;
class HEMesh;

class Camera;

class Painter
{
public:
	Painter();

	void paint(Grid const & grid);
	void paint(Mesh const & mesh);
	void paint(FVMesh const & fvmesh);
	void paint(HEMesh const & fvmesh);

	void set_camera(Camera const & camera) { camera_ref = &camera; }

private:
	Shader grid_shader;
	Shader flat_shader;
	Shader point_shader;

	Camera const * camera_ref;// Reference from Application class
};
