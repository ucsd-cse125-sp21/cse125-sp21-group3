#include "Window.h"
/*
 * File Name: Window.cpp
 *
 * This file contains methods which control what is rendered to the window
 * and how to respond to input from the user. This is where a bulk of the
 * implementation for our project will be written.
 *
 * @author Part of 169 starter code
 */
 ////////////////////////////////////////////////////////////////////////////////

 // Window Properties
int Window::width;
int Window::height;
const char* Window::windowTitle = "CSE 169 Starter";

// Objects to render
Cube* Window::cube;

// Camera Properties
Camera* Cam;

// Interaction Variables
bool LeftDown, RightDown;
int MouseX, MouseY;

// The shader program id
GLuint Window::shaderProgram;


////////////////////////////////////////////////////////////////////////////////

/*
 * Initializes the shader program.
 *
 * @return true if initialization was successful, false if not
 * @author Part of 169 starter code
 */
 // Constructors and desctructors
bool Window::initializeProgram() {

	// Create a shader program with a vertex shader and a fragment shader.
	shaderProgram = LoadShaders("shaders/shader.vert", "shaders/shader.frag");

	// Check the shader program.
	if (!shaderProgram)
	{
		std::cerr << "Failed to initialize shader program" << std::endl;
		return false;
	}

	return true;
}

/*
 * Initializes objects to be rendered within the window.
 *
 * @return true if initialization was successful, false if not
 * @author Part of 169 starter code
 */
bool Window::initializeObjects()
{
	// Create a cube
	cube = new Cube();
	//cube = new Cube(glm::vec3(-1, 0, -2), glm::vec3(1, 1, 1));

	return true;
}

/*
 * Deallocates all objects within the window and deletes shader program.
 *
 * @return true if initialization was successful, false if not
 * @author Part of 169 starter code
 */
void Window::cleanUp()
{
	// Deallcoate the objects.
	delete cube;

	// Delete the shader program.
	glDeleteProgram(shaderProgram);
}

////////////////////////////////////////////////////////////////////////////////
/*
 * Creates the window and sizes it appropriately.
 *
 * @param width The width of the window
 * @param height The height of the window
 * @return pointer to the window object
 * @author Part of 169 starter code
 */
GLFWwindow* Window::createWindow(int width, int height)
{
	// Initialize GLFW.
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return NULL;
	}

	// 4x antialiasing.
	glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__
	// Apple implements its own version of OpenGL and requires special treatments
	// to make it uses modern OpenGL.

	// Ensure that minimum OpenGL version is 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Enable forward compatibility and allow a modern OpenGL context
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Create the GLFW window.
	GLFWwindow* window = glfwCreateWindow(width, height, windowTitle, NULL, NULL);

	// Check if the window could not be created.
	if (!window)
	{
		std::cerr << "Failed to open GLFW window." << std::endl;
		glfwTerminate();
		return NULL;
	}

	// Make the context of the window.
	glfwMakeContextCurrent(window);

#ifndef __APPLE__
	// On Windows and Linux, we need GLEW to provide modern OpenGL functionality.

	// Initialize GLEW.
	if (glewInit())
	{
		std::cerr << "Failed to initialize GLEW" << std::endl;
		return NULL;
	}
#endif

	// Set swap interval to 1.
	glfwSwapInterval(0);

	// set up the camera
	Cam = new Camera();
	Cam->SetAspect(float(width) / float(height));

	// initialize the interaction variables
	LeftDown = RightDown = false;
	MouseX = MouseY = 0;

	// Call the resize callback to make sure things get drawn immediately.
	Window::resizeCallback(window, width, height);

	return window;
}

/*
 * This method is called every time the window is resized. This will scale
 * objects appropriately within the window after it has been resized.
 *
 * @param window Pointer to the window being resized
 * @param width The width fo the window
 * @param height The height of the window
 * @author Part of 169 starter code
 */
void Window::resizeCallback(GLFWwindow* window, int width, int height)
{
#ifdef __APPLE__
	// In case your Mac has a retina display.
	glfwGetFramebufferSize(window, &width, &height);
#endif
	Window::width = width;
	Window::height = height;
	// Set the viewport size.
	glViewport(0, 0, width, height);

	Cam->SetAspect(float(width) / float(height));
}

////////////////////////////////////////////////////////////////////////////////

/*
 * This method is called every frame and performs any operations needed to update
 * the game state before everything is drawn.
 *
 * @author Part of 169 starter code
 */
void Window::idleCallback()
{
	// Perform any updates as necessary.
	Cam->Update();

	//cube->update();
}

/*
 * This method is called every frame and renders all objects based on their current
 * game state.
 *
 * @param window Pointer to the window object
 * @author Part of 169 starter code
 */
void Window::displayCallback(GLFWwindow* window)
{
	// Clear the color and depth buffers.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Render the object.
	cube->draw(Cam->GetViewProjectMtx(), Window::shaderProgram);

	// Gets events, including input such as keyboard and mouse or window resizing.
	glfwPollEvents();
	// Swap buffers.
	glfwSwapBuffers(window);
}

////////////////////////////////////////////////////////////////////////////////

/*
 * Helper method to reset the camera back to its default position.
 *
 * @author Part of 169 starter code
 */
void Window::resetCamera()
{
	Cam->Reset();
	Cam->SetAspect(float(Window::width) / float(Window::height));
}

////////////////////////////////////////////////////////////////////////////////

/*
 * This method is called every time the user presses a key. Responsible for implementing
 * responses to key press events.
 *
 * @param window Pointer to the window object
 * @param key Keycode associated with the key pressed
 * @param scancode Reports which keys have been pressed? (not sure on this one)
 * @param action Action associated with the key event (could be press, hold, release, etc.)
 * @param mods Any modifiers associated with key press? (not sure on this one)
 * @author Part of 169 starter code
 */
void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	/*
	 * TODO: Modify below to add your key callbacks.
	 */

	 // Check for a key press.
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_ESCAPE:
			// Close the window. This causes the program to also terminate.
			glfwSetWindowShouldClose(window, GL_TRUE);
			break;

		case GLFW_KEY_R:
			resetCamera();
			break;

		default:
			break;
		}
	}
}

/*
 * This method is called every time the user clicks with the mouse.
 *
 * @param window Pointer to the window object
 * @param button The button that the user pressed on the mouse (left-click, right-click, etc.)
 * @param action Action associated with the key event (could be press, hold, release, etc.)
 * @param mods Any modifiers associated with mouse click? (not sure on this one)
 * @author Part of 169 starter code
 */
void Window::mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		LeftDown = (action == GLFW_PRESS);
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		RightDown = (action == GLFW_PRESS);
	}
}

/*
 * This method is called every time the cursor changes position.
 *
 * @param window Pointer to the window object
 * @param currX Current x position of the cursor within the window
 * @param currY Current y position of the cursor within the window
 * @author Part of 169 starter code
 */
void Window::cursor_callback(GLFWwindow* window, double currX, double currY) {

	int maxDelta = 100;
	int dx = glm::clamp((int)currX - MouseX, -maxDelta, maxDelta);
	int dy = glm::clamp(-((int)currY - MouseY), -maxDelta, maxDelta);

	MouseX = (int)currX;
	MouseY = (int)currY;

	// Move camera
	// NOTE: this should really be part of Camera::Update()
	if (LeftDown) {
		const float rate = 1.0f;
		Cam->SetAzimuth(Cam->GetAzimuth() + dx * rate);
		Cam->SetIncline(glm::clamp(Cam->GetIncline() - dy * rate, -90.0f, 90.0f));
	}
	if (RightDown) {
		const float rate = 0.005f;
		float dist = glm::clamp(Cam->GetDistance() * (1.0f - dx * rate), 0.01f, 1000.0f);
		Cam->SetDistance(dist);
	}
}

////////////////////////////////////////////////////////////////////////////////
