#include <OpenMesh/Core/IO/MeshIO.hh>

#include "Painter.hpp"
#include "Mesh.hpp"

Mesh::Mesh()
{
	OpenMesh::IO::Options iopt;
	iopt += OpenMesh::IO::Options::VertexNormal;
	//mesh.request_vertex_normals();
	if(!OpenMesh::IO::read_mesh(mesh, "./meshes/teapot.obj", iopt))
	{
		std::cout << "IO read error\n";
		exit(1);
	}

	if(!mesh.has_vertex_normals())
	{
		std::cout << "angle weighted vertex normal is required";
		exit(1);
	}

	no_vertices = mesh.n_vertices();

	setup_buffers();

	select_neighbour_vertices();
}


void Mesh::paint(Painter& p) const
{
	p.paint(*this);
}

void Mesh::setup_buffers(void)
{
	//https://github.com/guozhou/voxelizer/blob/master/Mesh.cpp
	std::vector<GLuint> indices(mesh.n_faces() * 3, 0);

	for(auto f : mesh.faces())
	{
		GLuint i = 0;
		for(auto fv : mesh.fv_range(f))
		{
			indices[f.idx() * 3 + i] = fv.idx();
			i++;
		}
	}
	no_tris = indices.size();

	std::vector<Point::vector_type> verts_normals(mesh.n_vertices() * 3 * 2);

	for(auto v : mesh.vertices())
	{
		verts_normals.push_back(mesh.point(v));
		verts_normals.push_back(mesh.normal(v));
	}

	glGenVertexArrays(1, &this->VAO);
	glBindVertexArray(this->VAO);
	
	//https://mailman.rwth-aachen.de/pipermail/openmesh/2011-April/000527.html
	glGenBuffers(1, &this->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, 3 * verts_normals.size() * sizeof(Point::value_type), &verts_normals[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &this->EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), &indices[0], GL_STATIC_DRAW);

	// Vertex Positions & Normals
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*) 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*) (3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Mesh::select_neighbour_vertices()
{
	std::vector<Point::vector_type> neighbour_vertices;
	MyMesh::VertexIter v_it, v_end(mesh.vertices_end());
	for(v_it = mesh.vertices_begin(); v_it != ++mesh.vertices_begin(); ++v_it)
	{
		std::cout << v_it->idx() << "\n";
		neighbour_vertices.push_back(mesh.point(*v_it));
		MyMesh::VertexVertexIter vv_it;
		for(vv_it = mesh.vv_iter(*v_it); vv_it.is_valid(); ++vv_it)
		{
			auto vertex = mesh.point(*vv_it);
			neighbour_vertices.push_back(vertex);
			std::cout << "\t" << vv_it->idx() << "\n";
		}
	}

	no_neighbourhood1 = neighbour_vertices.size();

	// update found vertices to GPU
	glBindVertexArray(this->VAO);

	glGenBuffers(1, &this->neighbour_verts_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, this->neighbour_verts_buffer);
	glBufferData(GL_ARRAY_BUFFER, 3 * neighbour_vertices.size() * sizeof(Point::value_type), &neighbour_vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}