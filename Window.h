#ifndef _WINDOW_H_
#define _WINDOW_H_

#include "core.h"
#include "Cube.h"
#include "shader.h"
#include "Opponent.h"

/*
 * File Name: Window.h
 *
 * Contains Window class which defines the properties of the window and 
 * handles event control. This class also handles setup for what is rendered
 * within the window and some OpenGL setup.
 *
 * @author Part of 169 starter code
 */
////////////////////////////////////////////////////////////////////////////////

class Window
{
public:
	// Window Properties
	static int width;
	static int height;
	static const char* windowTitle;

	// Objects to render
	static Cube* cube;

	// Shader Program 
	static GLuint shaderProgram;

	// Act as Constructors and desctructors 
	static bool initializeProgram();
	static bool initializeObjects();
	static void cleanUp();

	// for the Window
	static GLFWwindow* createWindow(int width, int height);
	static void resizeCallback(GLFWwindow* window, int width, int height);

	// update and draw functions
	static void idleCallback();
	static void displayCallback(GLFWwindow*);

	// helper to reset the camera
	static void resetCamera();

	// callbacks - for interaction
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void mouse_callback(GLFWwindow* window, int button, int action, int mods);
	static void cursor_callback(GLFWwindow* window, double currX, double currY);

	static void drawCrosshair();
	static bool debugMode;

	//Networking Stuff
	static void constructPlayerInputString();
	static std::string playerInputString;
	static bool isForwardPressed;
	static bool isRightPressed;
	static bool isLeftPressed;
	static bool isBackwardPressed;
	static bool isCrouched;
	static bool isSprinting;
	static glm::vec3 playerDirection;
	static bool hasFired;
	static map<int, Opponent*> opponentMap;
	static void updateOpponent(int id, glm::vec3 position, glm::vec3 direction, int moving);
};

////////////////////////////////////////////////////////////////////////////////

#endif