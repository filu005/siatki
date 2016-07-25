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

std::vector<glm::vec3> FVMesh::edge_border()
{
	std::vector<glm::vec3> features_vector;

	// iteruje po wszystkich trójk¹tach
	for(unsigned int face_idx = 0; face_idx < face_list.size(); ++face_idx)
	{
		auto face = face_list[face_idx];

		// i po ka¿dej jego krawêdzi
		// tj. trzech parach wierzcho³ków trójk¹ta
		auto common_edge = std::pair<unsigned int, unsigned int>{ 0, 0 };

		int face_common_edges[3][3] = { 0 };

		// sprawdzam czy inny trójk¹t ma wspóln¹ krawêdŸ z 'moim' trójk¹tem
		for(unsigned int face_j_idx = 0; face_j_idx < face_list.size(); ++face_j_idx)
		{
			if(face_j_idx == face_idx)
				continue;

			auto face_j = face_list[face_j_idx];

			// macierrz kombinacji punktów przedstawiaj¹ca
			// wszystkie mo¿liwe kombinacje stworzonych krawêdzi
			// z powtórzeniami - tj. krawêdzie mog¹ biec w ka¿d¹ stronê
			int common_edges[3][3] = { 0 };
			for(int i = 0; i < 2; ++i)
			{
				if(face_j[i] == face.x || face_j[i] == face.y || face_j[i] == face.z)
				{
					for(int j = i + 1; j < 3; ++j)// bez powtórzeñ, wpisuje tylko do lewej dolnej czêœci macierzy. reszta jest 0
					{
						//if(j == i)
						//	continue;
						common_edges[i][j] = static_cast<int>(face_j[j] == face.x || face_j[j] == face.y || face_j[j] == face.z);
					}
				}
			}

			int common_edge_count = 0;
			for (int i = 0; i < 3; ++i)
				common_edge_count += common_edges[0][i] + common_edges[1][i] + common_edges[2][i];

			// je¿eli krawêdzie trójk¹ta s¹ dzielone z drugim trójk¹tem
			if (common_edge_count > 0)
			{
				for (int i = 0; i < 2; ++i)
				{
					for (int j = i + 1; j < 3; ++j)
					{
						if (common_edges[i][j] == 1)// teraz znajduje te wspólne
						{
							common_edge = { face[i], face_j[j] };
							face_common_edges[i][j] = 1;
						}
						// break. tylko jak mam wyskoczyæ z tej pêli?!
					}
				}
			}
		}

		//if (common_edge.first != 0 && common_edge.second != 0)
		//{
		//	auto vert1 = vertex_list[common_edge.first];
		//	auto vert2 = vertex_list[common_edge.second];
		//	std::cout << "istnieje wspólny brzeg dla wierzcho³ków: " << common_edge.first << ", " << common_edge.second << "\n";
		//	features_vector.push_back(vert1);
		//	features_vector.push_back(vert2);
		//}

		for (int i = 0; i < 2; ++i)
		{
			for (int j = i + 1; j < 3; ++j)
			{
				if (face_common_edges[i][j] == 0)// teraz znajduje te wolne
				{
					auto vert1 = vertex_list[face[i]];
					auto vert2 = vertex_list[face[j]];
					features_vector.push_back(vert1);
					features_vector.push_back(vert2);
				}
			}
		}
	}

	return features_vector;
}

void FVMesh::flip_edges()
{
	// pick face index(!) somewhere from the middle
	auto face_idx_0 = static_cast<unsigned int>(face_list.size() * 0.3);
	int face_edges[6][2]; // globalny indeks wierzcho³ka, lokalny (odnosz¹cy siê konkretnie do trójk¹ta) indeks wierzcho³ka
	bool flipped = false;

	face_edges[0][0] = face_list[face_idx_0][0];
	face_edges[0][1] = 0;
	face_edges[1][0] = face_list[face_idx_0][1];
	face_edges[1][1] = 1;
	face_edges[2][0] = face_list[face_idx_0][1];
	face_edges[2][1] = 1;
	face_edges[3][0] = face_list[face_idx_0][2];
	face_edges[3][1] = 2;
	face_edges[4][0] = face_list[face_idx_0][2];
	face_edges[4][1] = 2;
	face_edges[5][0] = face_list[face_idx_0][0];
	face_edges[5][1] = 0;

	for(int i = 0; (i < 3) && (flipped == false); ++i)
	{
		auto vert_idx = face_list[face_idx_0][i];
		auto p1 = face_edges[i * 2][0];
		auto p1_idx = face_edges[i * 2][1];
		auto p2 = face_edges[(i * 2) + 1][0];
		auto p2_idx = face_edges[(i * 2) + 1][1];
		auto p3_idx = (p1_idx + p2_idx == 3 ? 0 : (p1_idx + p2_idx == 2 ? 1 : 2));

		for(unsigned int face_idx = 0; (face_idx < face_list.size()) && (flipped == false); ++face_idx)
		{
			auto face = face_list[face_idx];

			auto flip = [&](int const idx1, int const idx2, int const f1_idx3, int const f2_idx3)->void
			{
				face_list[face_idx_0][idx1] = face_list[face_idx][f2_idx3];
				face_list[face_idx][idx2] = face_list[face_idx_0][f1_idx3];
			};

			if(face_idx == face_idx_0)
				continue;

			if((face.x == p1 || face.y == p1 || face.z == p1) && 
			   (face.x == p2 || face.y == p2 || face.z == p2))
			{
				if(face.x == p1)
				{
					if(face.y == p2)
						flip(p1_idx, 1, p3_idx, 2);
					else // face.z == p2
					{
						// p1_idx, face_z_idx, p3_idx, face_y_idx
						flip(p1_idx, 2, p3_idx, 1);
					}
				}
				else if(face.y == p1)
				{
					if(face.x == p2)
						flip(p1_idx, 0, p3_idx, 2);
					else // face.z == p2
						flip(p1_idx, 2, p3_idx, 0);
				}
				else // face.z == p1
				{
					if(face.x == p2)
						flip(p1_idx, 0, p3_idx, 1);
					else // face.y == p2
						flip(p1_idx, 1, p3_idx, 0);
				}

				flipped = true;
				break;
			}
			
		}
	}
}