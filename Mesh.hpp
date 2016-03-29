#pragma once
#include <array>

#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>

#include "constants.hpp"
#include "Paintable.hpp"

class Mesh : public Paintable
{
public:
	using TriMesh = OpenMesh::TriMesh_ArrayKernelT<>;
	using PolyMesh = OpenMesh::PolyMesh_ArrayKernelT<>;
	
	using MyMesh = TriMesh;
	using Point = MyMesh::Point;

	Mesh();

	void paint(Painter& p) const override final;
	void setup_buffers() override final;

	GLuint getVBO() const { return VBO; }
	GLuint getNVB1() const { return neighbour_verts_buffer; }

	MyMesh const & getOpenMesh() const { return mesh; }

	//dla ka¿dego wierzcho³ka wyznaczanie otoczenia wierzcho³ków (pierwsza i druga warstwa s¹siednich wierzcho³ków),
	void select_neighbour_vertices();

	unsigned int no_vertices;
	unsigned int no_tris;
	unsigned int no_neighbourhood1;

private:
	MyMesh mesh;

	// OpenGL
	GLuint EBO;
	GLuint neighbour_verts_buffer;
};
