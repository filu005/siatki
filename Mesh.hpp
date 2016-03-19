#pragma once
#include <array>

#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>

#include "constants.hpp"
#include "Paintable.hpp"

class Mesh : public Paintable
{
public:
	using MyMesh = OpenMesh::PolyMesh_ArrayKernelT<>;
	using TriMesh = OpenMesh::TriMesh_ArrayKernelT<>;
	using Point = MyMesh::Point;
	
	Mesh();

	void paint(Painter& p) const override final;
	void setup_buffers() override final;

	unsigned int no_vertices;
	unsigned int no_tris;

private:
	TriMesh mesh;

	// OpenGL
	GLuint EBO;
};
