#include <OpenMesh/Core/IO/MeshIO.hh>

#include "Painter.hpp"
#include "Mesh.hpp"

Mesh::Mesh()
{
	// request vertex normals, so the mesh reader can use normal information
	// if available
	mesh.request_vertex_normals();
	
	// assure we have vertex normals

	if (!mesh.has_vertex_normals())
	{
		std::cout << "ERROR: Standard vertex property 'Normals' not available!\n";
		exit(1);
	}


	OpenMesh::IO::Options iopt;
	iopt += OpenMesh::IO::Options::VertexNormal;
	if(!OpenMesh::IO::read_mesh(mesh, "./meshes/bunny_simple.obj", iopt))
	{
		std::cout << "IO read error\n";
		exit(1);
	}
	
	// If the file did not provide vertex normals, then calculate them
	if (!iopt.check(OpenMesh::IO::Options::VertexNormal))
	{
		std::cout << "calculating vertex normals from face normals\n";
		// we need face normals to update the vertex normals
		mesh.request_face_normals();

		// let the mesh update the normals
		mesh.update_normals();

		// dispose the face normals, as we don't need them anymore
		mesh.release_face_normals();
	}

	no_vertices = mesh.n_vertices();

	setup_buffers();

	//select_neighbour_vertices();

	// don't need the normals anymore? Remove them!
	//mesh.release_vertex_normals();

	// just check if it really works
	//if (mesh.has_vertex_normals())
	//{
	//	std::cerr << "Ouch! ERROR! Shouldn't have any vertex normals anymore!\n";
	//	exit(1);
	//}
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

	std::vector<Point::vector_type> verts_normals;
	verts_normals.reserve(mesh.n_vertices() * 3 * 2);

	for(auto const v : mesh.vertices())
	{
		auto vert = mesh.point(v);
		auto norm = mesh.normal(v);
		verts_normals.push_back(vert);
		verts_normals.push_back(norm);
	}

	glGenVertexArrays(1, &this->VAO);
	glBindVertexArray(this->VAO);
	
	//https://mailman.rwth-aachen.de/pipermail/openmesh/2011-April/000527.html
	glGenBuffers(1, &this->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, verts_normals.size() * sizeof(Point::value_type) * Point::size_, &verts_normals[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &this->EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), &indices[0], GL_STATIC_DRAW);

	// Vertex Positions & Normals
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*) 0);
	glEnableVertexAttribArray(0);
	glVertexAttribDivisor(0, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*) (3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glVertexAttribDivisor(1, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Mesh::select_neighbour_vertices()
{
	std::vector<Point::vector_type> neighbour_vertices;
	for(auto v_it = mesh.vertices_begin(); v_it != ++mesh.vertices_begin(); ++v_it)
	{
		std::cout << v_it->idx() << "\n";
		//neighbour_vertices.push_back(mesh.point(*v_it));
		for(MyMesh::VertexVertexIter vv_it = mesh.vv_begin(*v_it); vv_it.is_valid(); ++vv_it)
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
	glBufferData(GL_ARRAY_BUFFER, neighbour_vertices.size() * sizeof(Point::value_type) * Point::size_, &neighbour_vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}