#pragma once
#include <vector>
#include <functional>

#include "constants.hpp"
#include "Paintable.hpp"

class Mesh;

// Face-Vertex Mesh
// https://en.wikipedia.org/wiki/Polygon_mesh#Face-vertex_meshes
//
class FVMesh : public Paintable
{
public:
	FVMesh(std::vector<glm::vec3> const & vertices, std::vector<glm::vec3> const & normals, std::vector<glm::uvec3> const & faces);
	FVMesh(Mesh const & m);

	void paint(Painter& p) const override final;
	void setup_buffers() override final;

	GLuint getVBO_feature() const { return VBO_feature; }

	GLenum draw_mode;
	unsigned int no_faces;
	unsigned int no_vertices;

	void get_feature(std::function<std::vector<unsigned int>()> feature);
	void get_feature(std::function<std::vector<glm::vec3>()> feature);

	// feature extraction
	// these methods return:
	// indexes of faces
	// OR 
	// vertices
	// which are selected to be rendered.
	// usage: pass these methods to get_feature(std::function feature)
	std::vector<glm::vec3> neighbour_1_ring();
	std::vector<glm::vec3> neighbour_2_ring();

	std::vector<unsigned int> vertex_common_faces();

	std::vector<unsigned int> face_neighbour_1_ring();
	std::vector<unsigned int> face_neighbour_2_ring();

	// TEGO JESZCZE NIE MA!
	std::vector<glm::vec3> edge_border();

	// mesh modifiers - TEGO JESZCZE NIE MA!
	void flip_edges();

private:
	std::vector<glm::uvec3> face_list;
	std::vector<glm::vec3> normal_list;
	std::vector<glm::vec3> vertex_list;

	GLuint EBO;
	GLuint VBO_normals;

	//features
	GLuint VBO_feature;
};