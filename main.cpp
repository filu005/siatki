#include <iostream>
#include <ctime>
#include <thread>

// GLEW
//#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// My includes
#include "Application.hpp"

// Function prototypes
void do_movement();
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void render(GLFWwindow* window);
double calcFPS(GLFWwindow* window, double theTimeInterval = 1.0, std::string theWindowTitle = "NONE");
bool secElapse(double interval = 1.0);

// Camera
bool keys[1024];
GLfloat lastX = c::screenWidth * 0.5f, lastY = c::screenHeight * 0.5f;
bool firstMouse = true;

std::unique_ptr<Application> app;

// The MAIN function, from here we start our application and run our Game loop
int main(int argc, char* argv[])
{
	// Init GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	GLFWwindow* window = glfwCreateWindow(c::screenWidth, c::screenHeight, "LearnOpenGL", nullptr, nullptr); // Windowed
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// Initialize GLEW to setup the OpenGL Function pointers
	glewExperimental = GL_TRUE;
	glewInit();
	glfwSetWindowPos(window, 100, 100);

	app = std::make_unique<Application>();

	// Define the viewport dimensions
	glViewport(0, 0, c::screenWidth, c::screenHeight);

	// Game loop
	while(!glfwWindowShouldClose(window))
	{
		// Check and call events
		glfwPollEvents();

		calcFPS(window, 1.0, "");
		do_movement();

		app->tick();

		render(window);

		//http://stackoverflow.com/questions/17138579/idle-thread-time-in-glfw-3-0-c
		std::this_thread::yield();
	}

	glfwTerminate();
	return 0;
}


void render(GLFWwindow* window)
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	app->paint();

	// Swap the buffers
	glfwSwapBuffers(window);
}

// Moves/alters the camera positions based on user input
void do_movement()
{
	GLfloat dt = 1.0f / 60.0f;
	Camera& camera = app->camera;
	// Camera controls
	if(keys[GLFW_KEY_W])
		camera.ProcessKeyboard(FORWARD, dt);
	if(keys[GLFW_KEY_S])
		camera.ProcessKeyboard(BACKWARD, dt);
	if(keys[GLFW_KEY_A])
		camera.ProcessKeyboard(LEFT, dt);
	if(keys[GLFW_KEY_D])
		camera.ProcessKeyboard(RIGHT, dt);
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if(action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		switch(key)
		{
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, GL_TRUE);
			break;
		}
	}

	if(action == GLFW_PRESS)
		keys[key] = true;
	else if(action == GLFW_RELEASE)
		keys[key] = false;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	Camera& camera = app->camera;
	GLfloat const xposf = static_cast<GLfloat>(xpos);
	GLfloat const yposf = static_cast<GLfloat>(ypos);

	if(firstMouse)
	{
		lastX = xposf;
		lastY = yposf;
		firstMouse = false;
	}

	GLfloat xoffset = xposf - lastX;
	GLfloat yoffset = lastY - yposf;  // Reversed since y-coordinates go from bottom to left

	lastX = xposf;
	lastY = yposf;

	camera.ProcessMouseMovement(xoffset*2.5f, yoffset*2.5f);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	Camera& camera = app->camera;
	camera.ProcessMouseScroll(static_cast<GLfloat>(yoffset) * 0.1f);
}

bool secElapse(double interval)
{
	static double t0 = glfwGetTime();

	double currentTime = glfwGetTime();

	if((currentTime - t0) > interval)
	{
		t0 = glfwGetTime();
		return true;
	}
	else
		return false;
}

// http://r3dux.org/2012/07/a-simple-glfw-fps-counter/
double calcFPS(GLFWwindow* window, double theTimeInterval, std::string theWindowTitle)
{
	// Static values which only get initialised the first time the function runs
	static double t0Value = glfwGetTime(); // Set the initial time to now
	static int    fpsFrameCount = 0;             // Set the initial FPS frame count to 0
	static double fps = 0.0;           // Set the initial FPS value to 0.0

	// Get the current time in seconds since the program started (non-static, so executed every time)
	double currentTime = glfwGetTime();

	// Ensure the time interval between FPS checks is sane (low cap = 0.1s, high-cap = 10.0s)
	// Negative numbers are invalid, 10 fps checks per second at most, 1 every 10 secs at least.
	if(theTimeInterval < 0.1)
	{
		theTimeInterval = 0.1;
	}
	if(theTimeInterval > 10.0)
	{
		theTimeInterval = 10.0;
	}

	// Calculate and display the FPS every specified time interval
	if((currentTime - t0Value) > theTimeInterval)
	{
		// Calculate the FPS as the number of frames divided by the interval in seconds
		fps = (double) fpsFrameCount / (currentTime - t0Value);

		// If the user specified a window title to append the FPS value to...
		if(theWindowTitle != "NONE")
		{
			// Convert the fps value into a string using an output stringstream
			std::ostringstream stream;
			stream << fps;
			std::string fpsString = stream.str();

			// Append the FPS value to the window title details
			theWindowTitle += "FPS: " + fpsString;

			// Convert the new window title to a c_str and set it
			const char* pszConstString = theWindowTitle.c_str();
			glfwSetWindowTitle(window, pszConstString);
		}
		else // If the user didn't specify a window to append the FPS to then output the FPS to the console
		{
			std::cout << "FPS: " << fps << std::endl;
		}

		// Reset the FPS frame counter and set the initial time to be now
		fpsFrameCount = 0;
		t0Value = glfwGetTime();
	}
	else // FPS calculation time interval hasn't elapsed yet? Simply increment the FPS frame counter
	{
		fpsFrameCount++;
	}

	// Return the current FPS - doesn't have to be used if you don't want it!
	return fps;
}