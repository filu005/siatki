#include <OpenMesh/Core/IO/MeshIO.hh>

#include "Painter.hpp"
#include "Mesh.hpp"

Mesh::Mesh()
{
	if(!OpenMesh::IO::read_mesh(mesh, "./meshes/teapot.obj"))
	{
		std::cerr << "read error\n";
		exit(1);
	}

	no_vertices = mesh.n_vertices();

	setup_buffers();
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

	glGenVertexArrays(1, &this->VAO);
	glBindVertexArray(this->VAO);
	
	//https://mailman.rwth-aachen.de/pipermail/openmesh/2011-April/000527.html
	glGenBuffers(1, &this->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, 3 * mesh.n_vertices() * sizeof(Point::value_type), mesh.points(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &this->EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), &indices[0], GL_STATIC_DRAW);

	// Vertex Positions
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*) 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
