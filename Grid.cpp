#include "Painter.hpp"
#include "Grid.hpp"


GLfloat const Grid::square_vertices[6 * 2] =
{
	// First triangle
	c::dx, c::dy,
	c::dx, -c::dy,
	-c::dx, c::dy,
	// Second triangle
	c::dx, -c::dy,
	-c::dx, -c::dy,
	-c::dx, c::dy
};

Grid::Grid()
{
	setup_buffers();
	clear_grid();
}

void Grid::paint(Painter& p) const
{
	p.paint(*this);
}

void Grid::setup_buffers(void)
{
	auto index = 0u;

	for(float y = c::ymin; y < c::ymax; y += c::dy)
		for(float x = c::xmin; x < c::xmax; x += c::dx)
			translations[index++] = glm::vec2(x, y);// post-increment

	glGenVertexArrays(1, &this->VAO);
	glBindVertexArray(this->VAO);

	glGenBuffers(1, &this->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->square_vertices), &this->square_vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Store instance data in an array buffer
	glGenBuffers(1, &this->instance_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->instance_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * c::KL, &this->translations[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Vertex Positions
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*) 0);

	// Also set instance data
	glBindBuffer(GL_ARRAY_BUFFER, this->instance_VBO);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*) 0);
	glVertexAttribDivisor(1, 1); // Tell OpenGL this is an instanced vertex attribute.

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Grid::clear_grid()
{
	for(auto& c : grid)
		c = 0.0f;
}