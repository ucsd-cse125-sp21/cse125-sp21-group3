#include "main.h"
#include <iostream>
#include <boost/asio.hpp>
using namespace std;
using namespace boost::asio;
using ip::tcp;
/*
 * File Name: main.cpp
 *
 * This file contains the starting point for program execution. Also peforms
 * some general setup about the program so that execution is interactive.
 *
 * @author Part of 169 starter code
 */
////////////////////////////////////////////////////////////////////////////////
/*
 * Called every time an error is encountered while the program is running.
 *
 * @param error Code representing the error encountered
 * @param description Description of the error
 * @author Part of 169 starter code
 */
void error_callback(int error, const char* description)
{
	// Print error.
	std::cerr << description << std::endl;
}

/*
 * Helper method which sets callbacks related to the
 * execution of the program. Most of these callbacks
 * are methods which execute in response to user input.
 *
 * @param window Pointer to the window which the program executes in
 * @author Part of 169 starter code
 */
void setup_callbacks(GLFWwindow* window)
{
	// Set the error callback.
	glfwSetErrorCallback(error_callback);
	// Set the window resize callback.
	glfwSetWindowSizeCallback(window, Window::resizeCallback);
	
	// Set the key callback.
	glfwSetKeyCallback(window, Window::keyCallback);

	// Set the mouse and cursor callbacks
	glfwSetMouseButtonCallback(window, Window::mouse_callback);
	glfwSetCursorPosCallback(window, Window::cursor_callback);
}

/*
 * Helper method which sets up OpenGL settings related to the
 * execution of the program.
 *
 * @author Part of 169 starter code
 */
void setup_opengl_settings()
{
	// Enable depth buffering.
	glEnable(GL_DEPTH_TEST);
	// Related to shaders and z value comparisons for the depth buffer.
	glDepthFunc(GL_LEQUAL);
	// Set polygon drawing mode to fill front and back of each polygon.
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	// Set clear color to black.
	glClearColor(0.0, 0.0, 0.0, 0.0);
}

/*
 * Prints system specific information that the program is running on.
 *
 * @author Part of 169 starter code
 */
void print_versions()
{
	// Get info of GPU and supported OpenGL version.
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "OpenGL version supported: " << glGetString(GL_VERSION) 
		<< std::endl;

	//If the shading language symbol is defined.
#ifdef GL_SHADING_LANGUAGE_VERSION
	std::cout << "Supported GLSL version is: " << 
		glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
#endif
}


void clientTest() {

    string message = "Default message from client.";
    message = message + "\n";

    boost::asio::io_service io_service;

    //socket creation
    tcp::socket socket(io_service);

    //connection
    socket.connect(tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 1234));

    // request/message from client
    boost::system::error_code error;
    mutable_buffer testBuffer = boost::asio::buffer(message);
    boost::asio::write(socket, testBuffer, error);
    
    if (!error) {
        cout << "Client sent message." << endl;
    }
    else {
        cout << "send failed: " << error.message() << endl;
    }

    // getting response from server
    boost::asio::streambuf receive_buffer;
    boost::asio::read(socket, receive_buffer, boost::asio::transfer_all(), error);
    if (error && error != boost::asio::error::eof) {
        cout << "receive failed: " << error.message() << endl;
    }
    else {
        const char* data = boost::asio::buffer_cast<const char*>(receive_buffer.data());
        cout << "Server Echoed: ";
        cout << data << endl;
    }
}
////////////////////////////////////////////////////////////////////////////////
/*
 * Starting point for program execution. This method contains a while loop
 * which will continue to run the program until the user terminates it. 
 *
 * @return Returns the exit code, whether or not the program exited successfully
 * @author Part of 169 starter code
 */
int main(void)
{
    clientTest();

	// Create the GLFW window.
	GLFWwindow* window = Window::createWindow(800, 600);
	if (!window) exit(EXIT_FAILURE);

	// Print OpenGL and GLSL versions.
	print_versions();
	// Setup callbacks.
	setup_callbacks(window);
	// Setup OpenGL settings.
	setup_opengl_settings();

	// Initialize the shader program; exit if initialization fails.
	if (!Window::initializeProgram()) exit(EXIT_FAILURE);
	
	// Initialize objects/pointers for rendering; exit if initialization fails.
	if (!Window::initializeObjects()) exit(EXIT_FAILURE);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	
	// Loop while GLFW window should stay open.
	while (!glfwWindowShouldClose(window))
	{
		// Main render display callback. Rendering of objects is done here.
		Window::displayCallback(window);


		// Idle callback. Updating objects, etc. can be done here.
		Window::idleCallback();
	}

	Window::cleanUp();
	// Destroy the window.
	glfwDestroyWindow(window);
	// Terminate GLFW.
	glfwTerminate();

	exit(EXIT_SUCCESS);
}

////////////////////////////////////////////////////////////////////////////////
