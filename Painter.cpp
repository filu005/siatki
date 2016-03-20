// GL Includes
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Grid.hpp"
#include "Mesh.hpp"
#include "Camera.hpp"
#include "Painter.hpp"

Painter::Painter() : grid_shader("./shaders/grid_2d.vert", "./shaders/grid_2d.frag"),
flat_shader("./shaders/flat_shade.vert", "./shaders/flat_shade.frag"),
point_shader("./shaders/point.vert", "./shaders/point.frag")
{

}


void Painter::paint(Grid const & grid)
{
	grid_shader.Use();

	// Create transformations
	glm::mat4 view;
	glm::mat4 model;
	glm::mat4 projection;
	assert(camera_ref != nullptr);
	auto const camera = *camera_ref;

	view = camera.GetViewMatrix();
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	projection = glm::perspective(camera.Zoom, c::aspectRatio, 0.1f, 1000.0f);

	// Get their uniform location
	GLint viewLoc = glGetUniformLocation(grid_shader.Program, "view");
	GLint modelLoc = glGetUniformLocation(grid_shader.Program, "model");
	GLint projLoc = glGetUniformLocation(grid_shader.Program, "projection");
	// Pass them to the shaders
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	auto const & VAO = grid.getVAO();

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBindVertexArray(VAO);
	glDrawArraysInstanced(GL_TRIANGLES, 0, 6 * 2, grid.bin_count);
	glBindVertexArray(0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}


void Painter::paint(Mesh const & mesh)
{
	// Create transformations
	glm::mat4 view;
	glm::mat4 model;
	glm::mat4 projection;
	assert(camera_ref != nullptr);
	auto const camera = *camera_ref;

	view = camera.GetViewMatrix();
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	projection = glm::perspective(camera.Zoom, c::aspectRatio, 0.1f, 1000.0f);

	auto const & VAO = mesh.getVAO();

	
	glBindVertexArray(VAO);
	

	flat_shader.Use();
	{
		// Get their uniform location
		GLint viewLoc = glGetUniformLocation(flat_shader.Program, "view");
		GLint modelLoc = glGetUniformLocation(flat_shader.Program, "model");
		GLint projLoc = glGetUniformLocation(flat_shader.Program, "projection");
		// Pass them to the shaders
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));


		glBindBuffer(GL_ARRAY_BUFFER, mesh.getVBO());
		glDrawElements(GL_TRIANGLES, mesh.no_tris, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glBindBuffer(GL_ARRAY_BUFFER, mesh.getVBO());
		glDrawElements(GL_TRIANGLES, mesh.no_tris, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	point_shader.Use();
	{
		// Get their uniform location
		GLint viewLoc = glGetUniformLocation(grid_shader.Program, "view");
		GLint modelLoc = glGetUniformLocation(grid_shader.Program, "model");
		GLint projLoc = glGetUniformLocation(grid_shader.Program, "projection");
		// Pass them to the shaders
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glBindBuffer(GL_ARRAY_BUFFER, mesh.getNVB1());
		glDrawArrays(GL_POINTS, 0, mesh.no_neighbourhood1);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	glBindVertexArray(0);
}
