#pragma once
#include <array>

#include "constants.hpp"
#include "Paintable.hpp"

class Grid : public Paintable
{
public:
	Grid();

	void paint(Painter& p) const override final;
	void setup_buffers() override final;

	void clear_grid();

	GLsizei const bin_count = c::KL;

private:
	// Hot stuff
	std::array<float, c::KL> grid;// grid of all cells (containing all Particles)

	// Geometry, instance offset array
	GLfloat const static square_vertices[6 * 2];
	std::array<glm::vec2, c::KL> translations;

	// OpenGL
	GLuint instance_VBO;
};
