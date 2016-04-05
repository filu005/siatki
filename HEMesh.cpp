#include "Painter.hpp"
#include "Mesh.hpp"
#include "HEMesh.hpp"


HEMesh::HEMesh(Mesh const & m_obj) : mesh(m_obj.getOpenMesh())
{
	setup_buffers();
}

void HEMesh::paint(Painter& p) const
{
	p.paint(*this);
}

void HEMesh::setup_buffers(void)
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

void HEMesh::get_feature(std::function<std::vector<glm::vec3>(void)> feature)
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


void HEMesh::get_feature(std::function<std::vector<unsigned int>(void)> feature)
{
	auto features_vector = feature();
	no_faces = features_vector.size();
	draw_mode = GL_TRIANGLES;

	std::vector<glm::vec3> faces_vertices;
	faces_vertices.reserve(no_faces * 3);

	for(auto const & face_idx : features_vector)
	{
		MyMesh::FaceHandle f_it(face_idx);

		for(MyMesh::FaceVertexIter fv_it = mesh.fv_iter(f_it); fv_it.is_valid(); ++fv_it)
		{
			auto vert = mesh.point(*fv_it);
			faces_vertices.push_back(glm::vec3(vert[0], vert[1], vert[2]));
		}
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

std::vector<glm::vec3> HEMesh::neighbour_1_ring()
{
	std::vector<glm::vec3> neighbour_vertices;

	auto vert_idx = static_cast<unsigned int>(mesh.n_vertices() * 0.3);
	MyMesh::VertexHandle v_it(vert_idx);

	for(MyMesh::VertexVertexIter vv_it = mesh.vv_iter(v_it); vv_it.is_valid(); ++vv_it)
	{
		auto vertex = mesh.point(*vv_it);		
		neighbour_vertices.push_back(glm::vec3(vertex[0], vertex[1], vertex[2]));
	}

	return neighbour_vertices;
}

std::vector<glm::vec3> HEMesh::neighbour_2_ring()
{
	std::vector<glm::vec3> neighbour_vertices;
	std::vector<MyMesh::VertexHandle> neighbour_vertex_handles;

	auto vert_idx_0 = static_cast<unsigned int>(mesh.n_vertices() * 0.25);
	MyMesh::VertexHandle v_it_0(vert_idx_0);

	for(MyMesh::VertexVertexIter vv_it_1 = mesh.vv_iter(v_it_0); vv_it_1.is_valid(); ++vv_it_1)
		neighbour_vertex_handles.push_back(*vv_it_1);

	for(auto const v_it_1 : neighbour_vertex_handles)
	{
		for(MyMesh::VertexVertexIter vv_it_2 = mesh.vv_iter(v_it_1); vv_it_2.is_valid(); ++vv_it_2)
		{
			if(vv_it_2->idx() == vert_idx_0)
				continue;

			auto vertex = mesh.point(*vv_it_2);
			neighbour_vertices.push_back(glm::vec3(vertex[0], vertex[1], vertex[2]));
		}
	}

	return neighbour_vertices;
}

std::vector<unsigned int> HEMesh::vertex_common_faces()
{
	std::vector<unsigned int> neighbour_faces;

	auto vert_idx = static_cast<unsigned int>(mesh.n_vertices() * 0.3);
	MyMesh::VertexHandle v_it(vert_idx);

	for(MyMesh::VertexFaceIter vf_it = mesh.vf_iter(v_it); vf_it.is_valid(); ++vf_it)
	{
		auto face_idx = vf_it->idx();
		neighbour_faces.push_back(face_idx);
	}

	return neighbour_faces;
}

std::vector<unsigned int> HEMesh::face_neighbour_1_ring()
{
	std::vector<unsigned int> features_vector;

	auto face_idx_0 = static_cast<unsigned int>(mesh.n_faces() * 0.3);
	MyMesh::FaceHandle f_it_0(face_idx_0);

	for(MyMesh::FaceFaceIter ff_it_1 = mesh.ff_iter(f_it_0); ff_it_1.is_valid(); ++ff_it_1)
	{
		auto face_idx = ff_it_1->idx();
		features_vector.push_back(face_idx);
	}

	return features_vector;
}

std::vector<unsigned int> HEMesh::face_neighbour_2_ring()
{
	auto face_idx_0 = static_cast<unsigned int>(mesh.n_faces() * 0.3);
	std::vector<unsigned int> features_vector = face_neighbour_1_ring();

	auto features_vector_1_size = features_vector.size();
	for(unsigned int i = 0; i < features_vector_1_size; ++i)
	{
		auto face_idx_1 = features_vector[i];
		MyMesh::FaceHandle f_it_1(face_idx_1);

		for(MyMesh::FaceFaceIter ff_it_2 = mesh.ff_iter(f_it_1); ff_it_2.is_valid(); ++ff_it_2)
		{
			auto face_idx = ff_it_2->idx();
			if(face_idx == face_idx_0)
				continue;

			features_vector.push_back(face_idx);
		}
	}

	return features_vector;
}

std::vector<glm::vec3> HEMesh::edge_border()
{
	std::vector<glm::vec3> features_vector;

	// Find this edge and then flip it
	for (MyMesh::HalfedgeIter he_it = mesh.halfedges_begin(); he_it != mesh.halfedges_end(); ++he_it)
	{
		MyMesh::HalfedgeHandle he_h(*he_it);
		if(mesh.is_boundary(he_h))
		{
			auto vertex1 = mesh.point(mesh.to_vertex_handle(he_h));
			auto vertex2 = mesh.point(mesh.from_vertex_handle(he_h));
			features_vector.push_back(glm::vec3(vertex1[0], vertex1[1], vertex1[2]));
			features_vector.push_back(glm::vec3(vertex2[0], vertex2[1], vertex2[2]));
		}
	}

	return features_vector;
}

void HEMesh::flip_edges()
{
	auto edge_idx = static_cast<unsigned int>(mesh.n_edges() * 0.3);
	// Find this edge and then flip it
	MyMesh::EdgeHandle e_h(edge_idx);
	if (!mesh.is_boundary(e_h))
	{
		// Flip edge
		mesh.flip(e_h);
	}
}
