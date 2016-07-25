#pragma once
#include <vector>
#include <functional>

#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>

#include "constants.hpp"
#include "Paintable.hpp"

class Mesh;

// Face-Vertex Mesh
// https://en.wikipedia.org/wiki/Polygon_mesh#Face-vertex_meshes
//
class HEMesh : public Paintable
{
public:
	using MyMesh = OpenMesh::TriMesh_ArrayKernelT<>;
	using Point = MyMesh::Point;
	using Face = MyMesh::Face;

	HEMesh(Mesh const & m);

	void paint(Painter& p) const override final;
	void setup_buffers() override final;

	GLuint getVBO_feature() const { return VBO_feature; }

	GLenum draw_mode;
	unsigned int no_faces;
	unsigned int no_vertices;

	void get_feature(std::function<std::vector<unsigned int>()> feature);
	void get_feature(std::function<std::vector<glm::vec3>()> feature);

	std::vector<glm::vec3> neighbour_1_ring();
	std::vector<glm::vec3> neighbour_2_ring();

	std::vector<unsigned int> vertex_common_faces();

	std::vector<unsigned int> face_neighbour_1_ring();
	std::vector<unsigned int> face_neighbour_2_ring();

	std::vector<glm::vec3> edge_border();

	void flip_edges();

private:
	MyMesh mesh;

	//features
	GLuint VBO_feature;
};
