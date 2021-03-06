#ifndef _WINDOW_H_
#define _WINDOW_H_

#include "core.h"
#include "Cube.h"
#include "shader.h"
#include "Camera.h"
#include "Game.h"
#include "Particle.h"

class Game;

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
	static GLuint shaderTextureQuadProgram;
	static GLuint shaderRedTintProgram;

	// Act as Constructors and desctructors 
	static bool initializeProgram();
	static bool initializeObjects(Game* game);
	static void cleanUp();

	// for the Window
	static GLFWwindow* createWindow(int width, int height);
	static void resizeCallback(GLFWwindow* window, int width, int height);

	// update and draw functions
	static void idleCallback(Game* game);
	static void displayCallback(Game* game, GLFWwindow*);

	// helper to reset the camera
	static void resetCamera();

	// callbacks - for interaction
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void mouse_callback(GLFWwindow* window, int button, int action, int mods);
	static void cursor_callback(GLFWwindow* window, double currX, double currY);

	static void drawCrosshair();
	static void drawHealth();
	static void drawArmor();
	static void drawHealthDigit(int startingX, int startingY, vector<bool> segmentsUsed);
	static void drawArmorDigit(int startingX, int startingY, vector<bool> segmentsUsed);
	static void drawIcon();
	static void loadAbilityIcon();
	static bool debugMode;
	static vector<vector<Particle*>> bloodsplatterList;
	static void generateBloodsplatter(glm::vec3 position, glm::vec3 color);
	static int createBloodsplatter;
	static bool mouseLock;

	//Networking Stuff
	static int createOpponent;
	static vector<string> messagesToServer;
};

////////////////////////////////////////////////////////////////////////////////

#endif