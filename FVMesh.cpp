#include "Painter.hpp"
#include "Mesh.hpp"
#include "FVMesh.hpp"

FVMesh::FVMesh(std::vector<glm::vec3> const & vertices, std::vector<glm::vec3> const & normals, std::vector<glm::uvec3> const & faces) :
	face_list(faces),
	normal_list(normals),
	vertex_list(vertices)
{
	no_faces = face_list.size();
}

FVMesh::FVMesh(Mesh const & m_obj)
{
	auto mesh = m_obj.getOpenMesh();
	vertex_list.reserve(mesh.n_vertices());
	normal_list.reserve(mesh.n_vertices());
	face_list.reserve(mesh.n_faces());

	for(auto const f : mesh.faces())
	{
		glm::uvec3 face_vert_idx;
		int i = 0;
		for(auto fv : mesh.fv_range(f))
		{
			assert(i < 3);
			face_vert_idx[i] = fv.idx();
			++i;
		}
		face_list.push_back(face_vert_idx);
	}
	no_faces = face_list.size();// !!!! pamiêtaj o tym jak bêdziesz przesy³aæ na GPU, ¿e to powinno byæ * sizeof(glm::uvec3)

	for(auto const v : mesh.vertices())
	{
		auto vert = mesh.point(v);
		auto norm = mesh.normal(v);
		vertex_list.push_back(glm::vec3(vert[0], vert[1], vert[2]));
		normal_list.push_back(glm::vec3(norm[0], norm[1], norm[2]));
	}
	no_vertices = vertex_list.size();

	setup_buffers();
}

void FVMesh::paint(Painter& p) const
{
	p.paint(*this);
}

void FVMesh::setup_buffers(void)
{
	glGenVertexArrays(1, &this->VAO);
	glBindVertexArray(this->VAO);

	//glGenBuffers(1, &this->VBO);
	//glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	//glBufferData(GL_ARRAY_BUFFER, vertex_list.size() * sizeof(glm::vec3), &vertex_list[0], GL_STATIC_DRAW);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	//glGenBuffers(1, &this->VBO_normals);
	//glBindBuffer(GL_ARRAY_BUFFER, this->VBO_normals);
	//glBufferData(GL_ARRAY_BUFFER, normal_list.size() * sizeof(glm::vec3), &normal_list[0], GL_STATIC_DRAW);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	////hm? czy nie tylko ficzery?
	//glGenBuffers(1, &this->EBO);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * no_faces, &face_list[0], GL_STATIC_DRAW);

	//// Vertex Positions & Normals
	//glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*) 0);
	//glEnableVertexAttribArray(0);
	//glBindBuffer(GL_ARRAY_BUFFER, this->VBO_normals);
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*) 0);
	//glEnableVertexAttribArray(1);
	////glVertexAttribDivisor(1, 0);

	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// default draw mode
	draw_mode = GL_POINTS;
}

void FVMesh::get_feature(std::function<std::vector<glm::vec3>(void)> feature)
{
	auto features_vector = feature();
	no_vertices = features_vector.size();
	draw_mode = GL_POINTS;

	glDeleteBuffers(1, &this->VBO_feature);
	
	glBindVertexArray(this->VAO);
	glGenBuffers(1, &this->VBO_feature);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO_feature);
	glBufferData(GL_ARRAY_BUFFER, features_vector.size() * sizeof(glm::vec3), &features_vector[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*) 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}


void FVMesh::get_feature(std::function<std::vector<unsigned int>(void)> feature)
{
	auto features_vector = feature();
	no_faces = features_vector.size();
	draw_mode = GL_TRIANGLES;

	std::vector<glm::vec3> faces_vertices;
	faces_vertices.reserve(no_faces * 3);

	for(auto const & face_idx : features_vector)
	{
		faces_vertices.push_back(vertex_list[face_list[face_idx].x]);
		faces_vertices.push_back(vertex_list[face_list[face_idx].y]);
		faces_vertices.push_back(vertex_list[face_list[face_idx].z]);
	}

	glDeleteBuffers(1, &this->VBO_feature);

	glBindVertexArray(this->VAO);
	glGenBuffers(1, &this->VBO_feature);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO_feature);
	glBufferData(GL_ARRAY_BUFFER, faces_vertices.size() * sizeof(glm::vec3), &faces_vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*) 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}


std::vector<glm::vec3> FVMesh::neighbour_1_ring()
{
	std::vector<glm::vec3> features_vector;
	// pick vertex index(!) somewhere from the middle
	auto vert_idx = static_cast<unsigned int>(vertex_list.size() * 0.3);

	for(auto const & face : face_list)
	{
		// if triangle face is built from this vertex
		// then remaining vertices are neighbours
		if(face.x == vert_idx)
		{
			features_vector.push_back(vertex_list[face.y]);
			features_vector.push_back(vertex_list[face.z]);
		}
		else if(face.y == vert_idx)
		{
			features_vector.push_back(vertex_list[face.x]);
			features_vector.push_back(vertex_list[face.z]);
		}
		else if(face.z == vert_idx)
		{
			features_vector.push_back(vertex_list[face.x]);
			features_vector.push_back(vertex_list[face.y]);
		}
	}

	return features_vector;
}

std::vector<glm::vec3> FVMesh::neighbour_2_ring()
{
	std::vector<unsigned int> neighbour_indices_vector;
	// pick vertex index(!) somewhere from the middle
	auto vert_idx_0 = static_cast<unsigned int>(vertex_list.size() * 0.3);

	for(auto const & face : face_list)
	{
		// if triangle face is built from this vertex
		// then remaining vertices are neighbours
		if(face.x == vert_idx_0)
		{
			neighbour_indices_vector.push_back(face.y);
			neighbour_indices_vector.push_back(face.z);
		}
		else if(face.y == vert_idx_0)
		{
			neighbour_indices_vector.push_back(face.x);
			neighbour_indices_vector.push_back(face.z);
		}
		else if(face.z == vert_idx_0)
		{
			neighbour_indices_vector.push_back(face.x);
			neighbour_indices_vector.push_back(face.y);
		}
	}

	std::vector<glm::vec3> features_vector;

	for(auto const & vert_idx_1 : neighbour_indices_vector)
	{
		for(auto const & face : face_list)
		{
			if(face.x == vert_idx_0 || face.y == vert_idx_0 || face.z == vert_idx_0)
				continue;

			if(face.x == vert_idx_1)
			{
				features_vector.push_back(vertex_list[face.y]);
				features_vector.push_back(vertex_list[face.z]);
			}
			else if(face.y == vert_idx_1)
			{
				features_vector.push_back(vertex_list[face.x]);
				features_vector.push_back(vertex_list[face.z]);
			}
			else if(face.z == vert_idx_1)
			{
				features_vector.push_back(vertex_list[face.x]);
				features_vector.push_back(vertex_list[face.y]);
			}
		}
	}

	return features_vector;
}

std::vector<unsigned int> FVMesh::vertex_common_faces()
{
	std::vector<unsigned int> features_vector;

	// pick vertex index(!) somewhere from the middle
	auto vert_idx = static_cast<unsigned int>(vertex_list.size() * 0.3);

	for(unsigned int face_idx = 0; face_idx < face_list.size(); ++face_idx)
	{
		auto face = face_list[face_idx];
		// if triangle face is built from this vertex
		// then remaining vertices are neighbours
		if(face.x == vert_idx || face.y == vert_idx || face.z == vert_idx)
		{
			features_vector.push_back(face_idx);
		}
	}

	return features_vector;
}

std::vector<unsigned int> FVMesh::face_neighbour_1_ring()
{
	std::vector<unsigned int> features_vector;

	// pick vertex index(!) somewhere from the middle
	auto face_idx_0 = static_cast<unsigned int>(face_list.size() * 0.3);
	for(int i = 0; i < 3; ++i)
	{
		auto vert_idx = face_list[face_idx_0][i];

		for(unsigned int face_idx = 0; face_idx < face_list.size(); ++face_idx)
		{
			auto face = face_list[face_idx];
			// if triangle face is built from this vertex
			// then remaining vertices are neighbours
			if(face.x == vert_idx || face.y == vert_idx || face.z == vert_idx)
			{
				features_vector.push_back(face_idx);
			}
		}
	}
	return features_vector;
}

std::vector<unsigned int> FVMesh::face_neighbour_2_ring()
{
	std::vector<unsigned int> features_vector = face_neighbour_1_ring();

	auto fv_size_0 = features_vector.size();
	for(unsigned int j = 0; j < fv_size_0; ++j)
	//for(auto const & face_idx_1 : features_vector) nie dzia³a nie wiem czemu :|
	{
		auto face_idx_1 = features_vector[j];
		for(int i = 0; i < 3; ++i)
		{
			auto vert_idx = face_list[face_idx_1][i];

			for(unsigned int face_idx = 0; face_idx < face_list.size(); ++face_idx)
			{
				auto face = face_list[face_idx];
				// if triangle face is built from this vertex
				// then remaining vertices are neighbours
				if(face.x == vert_idx || face.y == vert_idx || face.z == vert_idx)
				{
					features_vector.push_back(face_idx);
				}
			}
		}
	}

	return features_vector;
}