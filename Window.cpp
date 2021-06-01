#include "Window.h"
#include "Cube.h"
#include "Player.h"
#include "Maze.h"
#include "Model.h"
#include <windows.h>
#include <glm/gtx/string_cast.hpp>
#include "Player.h"
#include "Camera.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


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
const char* Window::windowTitle = "Labyrinth of Doom";
const int healthDigitSegmentLength = 20;
const int healthDigitSegmentThickness = 3;
float healthVerticalDigitSegment[healthDigitSegmentLength][healthDigitSegmentThickness][3];
float healthHorizontalDigitSegment[healthDigitSegmentThickness][healthDigitSegmentLength][3];
const int armorDigitSegmentLength = 15;
const int armorDigitSegmentThickness = 2;
float armorVerticalDigitSegment[armorDigitSegmentLength][armorDigitSegmentThickness][3];
float armorHorizontalDigitSegment[armorDigitSegmentThickness][armorDigitSegmentLength][3];


// Camera Properties
Camera* Cam;

irrklang::ISoundEngine* soundEngine = irrklang::createIrrKlangDevice();

// Interaction Variables
bool LeftDown, RightDown;
int MouseX, MouseY;

// The shader program id
GLuint Window::shaderProgram;
GLuint Window::shaderTextureQuadProgram;
GLuint Window::shaderRedTintProgram;

//toggle to see bounding boxes
bool Window::debugMode;
Model* chest;
Model* gun;
Model* character;

Game* gm;
Player* player;

//Networking Stuff
Cube* Window::cube;
int Window::createOpponent;
vector<string> Window::messagesToServer;

int loadedAbility;

//rendering icons
unsigned int abilityQuadVAO, abilityQuadVBO;
float abilityQuadVertices[] = {
	// positions        // texture Coords
	-1.0f,  1.0f, 0.0f, 1.0f, 0.0f,
	-1.0f, -1.0f, 0.0f, 1.0f, 1.0f,
	 1.0f,  1.0f, 0.0f, 0.0f, 0.0f,
	 1.0f, -1.0f, 0.0f, 0.0f, 1.0f,
};
GLuint abilityTexture;
bool abilityLoaded;
int hurtCounter = 0;
vector<vector<Particle*>> Window::bloodsplatterList;
int Window::createBloodsplatter = -1;

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
	shaderTextureQuadProgram = LoadShaders("shaders/shaderTexture_quad.vert", "shaders/shaderTexture_quad.frag");
	shaderRedTintProgram = LoadShaders("shaders/shaderRedTint.vert", "shaders/shaderRedTint.frag");

	// Check the shader program.
	if (!shaderProgram)
	{
		std::cerr << "Failed to initialize shader program" << std::endl;
		return false;
	}

	debugMode = false;
	
	return true;
}

/*
 * Initializes objects to be rendered within the window.
 *
 * @return true if initialization was successful, false if not
 * @author Part of 169 starter code
 */
bool Window::initializeObjects(Game* game)
{
	cout << "Initializing game" << endl;
	gm = game;

	//player setup
	player = new Player(glm::vec3(3.0f, 3.5f, 3.0f), game, true);
	game->myPlayer = player;

	game->allPlayers.push_back(player);

	Cam = player->getPlayerCamera();
	Cam->SetAspect(float(width) / float(height));

	player->setSoundEngine(soundEngine);
	player->resetInputDirections();

	loadedAbility = 0;

	//initializing digit segments to represent health
	for (int y = 0; y < healthDigitSegmentLength; y++)
	{
		for (int x = 0; x < healthDigitSegmentThickness; x++)
		{
			healthVerticalDigitSegment[y][x][0] = 1.0f;
			healthVerticalDigitSegment[y][x][1] = 0.0f;
			healthVerticalDigitSegment[y][x][2] = 0.0f;
		}
	}

	for (int y = 0; y < healthDigitSegmentThickness; y++)
	{
		for (int x = 0; x < healthDigitSegmentLength; x++)
		{
			healthHorizontalDigitSegment[y][x][0] = 1.0f;
			healthHorizontalDigitSegment[y][x][1] = 0.0f;
			healthHorizontalDigitSegment[y][x][2] = 0.0f;
		}
	}

	//initializing digit segments to represent armor
	for (int y = 0; y < armorDigitSegmentLength; y++)
	{
		for (int x = 0; x < armorDigitSegmentThickness; x++)
		{
			armorVerticalDigitSegment[y][x][0] = 0.0f;
			armorVerticalDigitSegment[y][x][1] = 0.0f;
			armorVerticalDigitSegment[y][x][2] = 1.0f;
		}
	}

	for (int y = 0; y < armorDigitSegmentThickness; y++)
	{
		for (int x = 0; x < armorDigitSegmentLength; x++)
		{
			armorHorizontalDigitSegment[y][x][0] = 0.0f;
			armorHorizontalDigitSegment[y][x][1] = 0.0f;
			armorHorizontalDigitSegment[y][x][2] = 1.0f;
		}
	}

	// setup plane VAO
	glGenVertexArrays(1, &abilityQuadVAO);
	glGenBuffers(1, &abilityQuadVBO);
	glBindVertexArray(abilityQuadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, abilityQuadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(abilityQuadVertices), &abilityQuadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	glGenTextures(1, &abilityTexture);
	glBindTexture(GL_TEXTURE_2D, abilityTexture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	abilityLoaded = false;
	//game->initiateGame();
	cube = new Cube(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 0, true);
	return true;
}

/*
 * Deallocates all objects within the window and deletes shader program.
 *
 * @author Part of 169 starter code
 */
void Window::cleanUp()
{
	// Deallcoate the objects.
	//for (BoundingBox* bound : maze->getBoundingBox())
	//{
	//	delete bound;
	//}

	//delete chest;
	//delete gun;
	//delete player;
	//delete Cam;

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
	Window::createOpponent = -1;
	Window::messagesToServer = {};
	

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

	// initialize the interaction variables
	LeftDown = RightDown = false;
	MouseX = MouseY = 0;


	// Call the resize callback to make sure things get drawn immediately.
	Window::resizeCallback(window, width, height);
	
	//disable cursor
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	return window;
}

/*
 * This method is called every time the window is resized. This will scale
 * objects appropriately within the window after it has been resized.
 *
 * @param window Pointer to the window being resized
 * @param width The width of the window
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

	if (Cam)
	{ 
		Cam->SetAspect(float(width) / float(height));
	}
	glfwSetCursorPos(window, width / 2, height / 2);
}

////////////////////////////////////////////////////////////////////////////////




/*
 * This method is called every frame and performs any operations needed to update
 * the game state before everything is drawn.
 *
 * @author Part of 169 starter code
 */
void Window::idleCallback(Game* game)
{
	//update all players in the game	
	if (game->gameBegun)
	{
		for (int i = 0; i < game->allPlayers.size(); i++) {
			if (game->allPlayers.at(i)->getId() != -1) {
				game->allPlayers.at(i)->update(0.02f, game);
				game->allPlayers.at(i)->createFootPrint(game->allPlayers.at(i)->getPosition());
			}
		}
	}

	//Networking Stuff
	//------------------------------------------------------------------------
	if (Window::createOpponent != -1) {
		cout << "creating player: " << Window::createOpponent << endl;
		Player* p = new Player(glm::vec3(3.0f, 3.5f, 3.0f), game, true);

		//changing position for testing purposes
		/*p->getPlayerModel()->rootModel[3][2] -= 5.0f;
		p->getPlayerGunModel()->rootModel[3][2] -= 5.0f;*/
		////////////////////////////////////////

		p->setId(Window::createOpponent);
		game->allPlayers.push_back(p);
		game->allBoundingBoxes.push_back(p->getBoundingBox());
		Window::createOpponent = -1;
	}
	//------------------------------------------------------------------------

	//Bloodsplatter Stuff
	float time = glfwGetTime();
	for (int i = 0; i < bloodsplatterList.size(); i++) {
		vector<Particle*> bloodsplatter = bloodsplatterList.at(i);
		if (bloodsplatter.size() > 0 && bloodsplatter.at(0)->lifetime - bloodsplatter.at(0)->spawnTime > bloodsplatter.at(0)->lifespan) {
			for (Particle* p : bloodsplatter) {
				delete p;
			}

			/*cout << "lifetime: " << bloodsplatter.at(0)->lifetime << endl;
			cout << "spawnTime: " << bloodsplatter.at(0)->spawnTime << endl;
			cout << "lifetime - spawntime: " << bloodsplatter.at(0)->lifetime - bloodsplatter.at(0)->spawnTime << endl;
			cout << "deleting bloodsplatter" << endl;*/
			bloodsplatterList.erase(bloodsplatterList.begin() + i);
		}
		else {
			for (Particle* p : bloodsplatter) {
				p->update(time - p->lifetime);
			}
		}
	}
	if (Window::createBloodsplatter != -1) {
		glm::vec3 position;
		glm::vec3 color;
		for (int i = 0; i < game->allPlayers.size(); i++) {
			if (game->allPlayers.at(i)->getId() == Window::createBloodsplatter) {
				position = game->allPlayers.at(i)->getPosition();
				position.y -= 1.0f;
				color = glm::vec3(game->allPlayers.at(i)->getPlayerModel()->meshes.at(0)->baseColor.x,
					game->allPlayers.at(i)->getPlayerModel()->meshes.at(0)->baseColor.y,
					game->allPlayers.at(i)->getPlayerModel()->meshes.at(0)->baseColor.z);
			}
		}
		generateBloodsplatter(position, color);
		Window::createBloodsplatter = -1;
	}
}

/*
 * Draws crosshair in middle of screen.
 *
 * @author Lucas Hwang
 */
void Window::drawCrosshair() {

	const int crosshairLength = 31;
	const int crosshairThickness = 3;
	glm::vec3 crosshairColor(1.0f, 0.0f, 0.0f);
	float verticalBar[crosshairLength][crosshairThickness][3];

	glWindowPos2i(width / 2, height / 2 - crosshairLength / 2);
	for (int y = 0; y < crosshairLength; y++)
	{
		for (int x = 0; x < crosshairThickness; x++)
		{
			verticalBar[y][x][0] = crosshairColor.x;
			verticalBar[y][x][1] = crosshairColor.y;
			verticalBar[y][x][2] = crosshairColor.z;
		}
	}

	glDrawPixels(crosshairThickness, crosshairLength, GL_RGB, GL_FLOAT, verticalBar);

	float horizontalBar[crosshairThickness][crosshairLength][3];
	glWindowPos2i(width / 2 - crosshairLength / 2 + 1, height / 2 - 1);
	for (int y = 0; y < crosshairThickness; y++)
	{
		for (int x = 0; x < crosshairLength; x++)
		{
			horizontalBar[y][x][0] = crosshairColor.x;
			horizontalBar[y][x][1] = crosshairColor.y;
			horizontalBar[y][x][2] = crosshairColor.z;
		}
	}
	
	//cube->update();
	glDrawPixels(crosshairLength, crosshairThickness, GL_RGB, GL_FLOAT, horizontalBar);
}


/*
 * Draws digit in seven segment form.
 *
 * @author Lucas Hwang
 */
void Window::drawHealthDigit(int startingX, int startingY, vector<bool> segmentsUsed) {

	
	glWindowPos2i(startingX, startingY);
	if (segmentsUsed.at(0)) {
		glDrawPixels(healthDigitSegmentLength, healthDigitSegmentThickness, GL_RGB, GL_FLOAT, healthHorizontalDigitSegment);
	}
	if (segmentsUsed.at(1)) {
		glDrawPixels(healthDigitSegmentThickness, healthDigitSegmentLength, GL_RGB, GL_FLOAT, healthVerticalDigitSegment);
	}
	glWindowPos2i(startingX + healthDigitSegmentLength - healthDigitSegmentThickness, startingY);
	if (segmentsUsed.at(2)) {
		glDrawPixels(healthDigitSegmentThickness, healthDigitSegmentLength, GL_RGB, GL_FLOAT, healthVerticalDigitSegment);
	}
	glWindowPos2i(startingX, startingY + healthDigitSegmentLength - healthDigitSegmentThickness);
	if (segmentsUsed.at(3)) {
		glDrawPixels(healthDigitSegmentLength, healthDigitSegmentThickness, GL_RGB, GL_FLOAT, healthHorizontalDigitSegment);
	}
	if (segmentsUsed.at(4)) {
		glDrawPixels(healthDigitSegmentThickness, healthDigitSegmentLength, GL_RGB, GL_FLOAT, healthVerticalDigitSegment);
	}
	glWindowPos2i(startingX + healthDigitSegmentLength - healthDigitSegmentThickness, startingY + healthDigitSegmentLength - healthDigitSegmentThickness);
	if (segmentsUsed.at(5)) {
		glDrawPixels(healthDigitSegmentThickness, healthDigitSegmentLength, GL_RGB, GL_FLOAT, healthVerticalDigitSegment);
	}
	glWindowPos2i(startingX, startingY + 2 * (healthDigitSegmentLength - healthDigitSegmentThickness));
	if (segmentsUsed.at(6)) {
		glDrawPixels(healthDigitSegmentLength, healthDigitSegmentThickness, GL_RGB, GL_FLOAT, healthHorizontalDigitSegment);
	}
}

/*
 * Draws digit in seven segment form.
 *
 * @author Lucas Hwang
 */
void Window::drawArmorDigit(int startingX, int startingY, vector<bool> segmentsUsed) {


	glWindowPos2i(startingX, startingY);
	if (segmentsUsed.at(0)) {
		glDrawPixels(armorDigitSegmentLength, armorDigitSegmentThickness, GL_RGB, GL_FLOAT, armorHorizontalDigitSegment);
	}
	if (segmentsUsed.at(1)) {
		glDrawPixels(armorDigitSegmentThickness, armorDigitSegmentLength, GL_RGB, GL_FLOAT, armorVerticalDigitSegment);
	}
	glWindowPos2i(startingX + armorDigitSegmentLength - armorDigitSegmentThickness, startingY);
	if (segmentsUsed.at(2)) {
		glDrawPixels(armorDigitSegmentThickness, armorDigitSegmentLength, GL_RGB, GL_FLOAT, armorVerticalDigitSegment);
	}
	glWindowPos2i(startingX, startingY + armorDigitSegmentLength - armorDigitSegmentThickness);
	if (segmentsUsed.at(3)) {
		glDrawPixels(armorDigitSegmentLength, armorDigitSegmentThickness, GL_RGB, GL_FLOAT, armorHorizontalDigitSegment);
	}
	if (segmentsUsed.at(4)) {
		glDrawPixels(armorDigitSegmentThickness, armorDigitSegmentLength, GL_RGB, GL_FLOAT, armorVerticalDigitSegment);
	}
	glWindowPos2i(startingX + armorDigitSegmentLength - armorDigitSegmentThickness, startingY + armorDigitSegmentLength - armorDigitSegmentThickness);
	if (segmentsUsed.at(5)) {
		glDrawPixels(armorDigitSegmentThickness, armorDigitSegmentLength, GL_RGB, GL_FLOAT, armorVerticalDigitSegment);
	}
	glWindowPos2i(startingX, startingY + 2 * (armorDigitSegmentLength - armorDigitSegmentThickness));
	if (segmentsUsed.at(6)) {
		glDrawPixels(armorDigitSegmentLength, armorDigitSegmentThickness, GL_RGB, GL_FLOAT, armorHorizontalDigitSegment);
	}
}


/*
 * Draws health in the form of digits
 *
 * @author Lucas Hwang
 */
void Window::drawHealth() {

	vector<int> digits;
	int temp = player->getHealth();
	while (temp > 0) {
		digits.push_back(temp % 10);
		temp = temp / 10;
	}

	int startingX = 10;
	for (int i = digits.size() - 1; i >= 0; i--) {
		int digit = digits.at(i);
		vector<bool> segmentsUsed;
		switch(digit){
		case 0:
			segmentsUsed = { true, true, true, false, true, true, true };
			break;
		case 1:
			segmentsUsed = { false, false, true, false, false, true, false };
			break;
		case 2:
			segmentsUsed = { true, true, false, true, false, true, true };
			break;
		case 3:
			segmentsUsed = { true, false, true, true, false, true, true };
			break;
		case 4:
			segmentsUsed = { false, false, true, true, true, true, false };
			break;
		case 5:
			segmentsUsed = { true, false, true, true, true, false, true };
			break;
		case 6:
			segmentsUsed = { true, true, true, true, true, false, true };
			break;
		case 7:
			segmentsUsed = { false, false, true, false, false, true, true };
			break;
		case 8:
			segmentsUsed = { true, true, true, true, true, true, true };
			break;
		case 9:
			segmentsUsed = { true, false, true, true, true, true, true };
			break;
		default:
			segmentsUsed = { false, false, false, false, false, false, false };
			break;
		}

		drawHealthDigit(startingX, height - 50, segmentsUsed);
		startingX += 30;
	}	
}

/*
 * Draws health in the form of digits
 *
 * @author Lucas Hwang
 */
void Window::drawArmor() {

	vector<int> digits;
	int temp = player->getArmor();
	while (temp > 0) {
		digits.push_back(temp % 10);
		temp = temp / 10;
	}

	int startingX = 20;
	for (int i = digits.size() - 1; i >= 0; i--) {
		int digit = digits.at(i);
		vector<bool> segmentsUsed;
		switch (digit) {
		case 0:
			segmentsUsed = { true, true, true, false, true, true, true };
			break;
		case 1:
			segmentsUsed = { false, false, true, false, false, true, false };
			break;
		case 2:
			segmentsUsed = { true, true, false, true, false, true, true };
			break;
		case 3:
			segmentsUsed = { true, false, true, true, false, true, true };
			break;
		case 4:
			segmentsUsed = { false, false, true, true, true, true, false };
			break;
		case 5:
			segmentsUsed = { true, false, true, true, true, false, true };
			break;
		case 6:
			segmentsUsed = { true, true, true, true, true, false, true };
			break;
		case 7:
			segmentsUsed = { false, false, true, false, false, true, true };
			break;
		case 8:
			segmentsUsed = { true, true, true, true, true, true, true };
			break;
		case 9:
			segmentsUsed = { true, false, true, true, true, true, true };
			break;
		default:
			segmentsUsed = { false, false, false, false, false, false, false };
			break;
		}

		drawArmorDigit(startingX, height - 100, segmentsUsed);
		startingX += 20;
	}
}


void Window::loadAbilityIcon() {

	int width, height, nrChannels;
	unsigned char* data;
	switch (player->getAbility()) {
	case 1:
		data = stbi_load("Assets/icons/removeWall.jpg", &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(data);
		break;
	case 2:
		data = stbi_load("Assets/icons/trackPlayer.jpg", &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(data);
		break;
	case 3:
		data = stbi_load("Assets/icons/seeMap.jpg", &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(data);
		break;
	case 4:
		data = stbi_load("Assets/icons/healPlayer.jpg", &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(data);
		break;
	case 5:
		data = stbi_load("Assets/icons/increasePlayerMaxHealth.jpg", &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(data);
		break;
	case 6:
		data = stbi_load("Assets/icons/armorPlayer.jpg", &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(data);
		break;
	case 7:
		data = stbi_load("Assets/icons/damageBoost.jpg", &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(data);
		break;
	default:
		cout << "no icon for ability: " << player->getAbility() << endl;
		break;
	}
	loadedAbility = player->getAbility();
}

void Window::drawIcon() {

	glViewport(0, 0, width / 6, height / 6);
	glUseProgram(Window::shaderTextureQuadProgram);
	glBindVertexArray(abilityQuadVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, abilityTexture);
	// draw the points using triangles, indexed with the EBO
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

/*
 * This method is called every frame and renders all objects based on their current
 * game state.
 *
 * @param window Pointer to the window object
 * @author Part of 169 starter code
 */
void Window::displayCallback(Game* game, GLFWwindow* window)
{	

	GLuint shaderProgramToUse = Window::shaderProgram;
	if (player->getIsHurt()) {
		shaderProgramToUse = Window::shaderRedTintProgram;
		hurtCounter++;
		if (hurtCounter > 10) {
			hurtCounter = 0;
			player->setIsHurt(false);
		}
	}
	if (gm->gameBegun)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, width, height);
		// Clear the color and depth buffers.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//for (Cube* footprint : player->getFootprints()) {
			//footprint->draw(Cam->GetViewProjectMtx(), Window::shaderProgram);
		//}
		for (int i = 0; i < game->allPlayers.size(); i++) {
			if (game->allPlayers.at(i)->getState() != Player::dead) {
				game->allPlayers.at(i)->draw(Cam->GetViewProjectMtx(), Window::shaderProgram);
			}
			//game->allPlayers.at(i)->createFootPrint(glm::vec3(0.0f, 0.0f, 0.0f));
			//game->allPlayers.at(i)->getFootprints().at(0)->draw(Cam->GetViewProjectMtx(), Window::shaderProgram);
		}

		for (int i = 0; i < game->allPlayers.size(); i++) {
			for (int j = 0; j < game->allPlayers.at(i)->getFootprints().size(); j++) {

				if (game->allPlayers.at(i)->getState() != Player::dead) {
					game->allPlayers.at(i)->getFootprints().at(j)->draw(Cam->GetViewProjectMtx(), Window::shaderProgram);
				}
				else {
					while (game->allPlayers.at(i)->getFootprints().size() > 0) {
						game->allPlayers.at(i)->getFootprints().pop_front();
					}
				}
				
			}
		}


		Camera* playCam = player->getPlayerCamera();
		irrklang::vec3df position(player->getPosition().x, player->getPosition().y, player->getPosition().z);        // position of the listener
		irrklang::vec3df lookDirection(playCam->getDirection().x, playCam->getDirection().y, playCam->getDirection().z); // the direction the listener looks into
		irrklang::vec3df velPerSecond(player->getVelocity().x, player->getVelocity().y, player->getVelocity().z);    // only relevant for doppler effects
		irrklang::vec3df upVector(0, 1, 0);        // where 'up' is in your 3D scene
		player->getSoundEngine()->setListenerPosition(position, lookDirection, velPerSecond, upVector);

		for (Cube* wall : game->maze->getWalls()) {
			wall->draw(Cam->GetViewProjectMtx(), shaderProgramToUse);
		}

		for (Model* abilityChest : gm -> maze->getChests()) {
			wallInfo** mazeArray = gm -> maze->getMazeArray();
			glm::vec3 abilityChestLocation(abilityChest->rootModel[3][0], abilityChest->rootModel[3][1], abilityChest->rootModel[3][2]);
			int* abilityChestPos = gm -> maze->getCoordinates(abilityChestLocation);
			if (abilityChest->opening) { //if client is in the process of opening the chest
				//cout << "ability chest is opening" << endl;
				if (abilityChest->animationClipList.at(0)->prevTime + 0.1f > abilityChest->animationClipList.at(0)->duration) {
					abilityChest->opening = false;
				}
				else {
					abilityChest->playAnimation(abilityChest->animationClipList.at(0), 0.1f, false);
				}
			}
			else if (mazeArray[abilityChestPos[0]][abilityChestPos[1]].ability == Player::opened) { //if the server has said this chest is open
				abilityChest->animationClipList.at(0)->prevTime = abilityChest->animationClipList.at(0)->duration;
				abilityChest->playAnimation(abilityChest->animationClipList.at(0), 0.0f, false);
			}

			abilityChest->draw(Cam->GetViewProjectMtx(), shaderProgramToUse);
		}


		gm->maze->getGround()->draw(Cam->GetViewProjectMtx(), shaderProgramToUse);
		for (int i = 0; i < bloodsplatterList.size(); i++) {
			//cout << "bloodsplatter: " << i << endl;
			vector<Particle*> bloodsplatter = bloodsplatterList.at(i);
			for (Particle* p : bloodsplatter) {
				p->draw(Cam->GetViewProjectMtx(), shaderProgramToUse);
			}
		}
		drawCrosshair();
		drawHealth();
		drawArmor();

		if (player->getAbility() != Player::none)
		{
			if (loadedAbility != player->getAbility())
			{
				loadAbilityIcon();
			}
			drawIcon();
		}
		
		// Gets events, including input such as keyboard and mouse or window resizing.
		glfwPollEvents();

		// Swap buffers.
		glfwSwapBuffers(window);
	}
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

void Window::generateBloodsplatter(glm::vec3 position, glm::vec3 color) {

	vector<Particle*> bloodsplatter;
	float time = glfwGetTime();
	bloodsplatter.push_back(new Particle(position, glm::vec3(30.0f, 0.0f, 0.0f), time, color));
	bloodsplatter.push_back(new Particle(position, glm::vec3(-30.0f, 0.0f, 0.0f), time, color));
	bloodsplatter.push_back(new Particle(position, glm::vec3(0.0f, 30.0f, 0.0f), time, color));
	bloodsplatter.push_back(new Particle(position, glm::vec3(0.0f, -30.0f, 0.0f), time, color));
	bloodsplatter.push_back(new Particle(position, glm::vec3(0.0f, 0.0f, 30.0f), time, color));
	bloodsplatter.push_back(new Particle(position, glm::vec3(0.0f, 0.0f, -30.0f), time, color));
	bloodsplatter.push_back(new Particle(position, glm::vec3(30.0f, 30.0f, 0.0f), time, color));
	bloodsplatter.push_back(new Particle(position, glm::vec3(-30.0f, -30.0f, 0.0f), time, color));
	bloodsplatter.push_back(new Particle(position, glm::vec3(-30.0f, 30.0f, 0.0f), time, color));
	bloodsplatter.push_back(new Particle(position, glm::vec3(30.0f, -30.0f, 0.0f), time, color));
	bloodsplatterList.push_back(bloodsplatter);
}

/*
 * This method is called every time the user presses a key. Responsible for implementing
 * responses to key press events.
 *
 * @param window Pointer to the window object
 * @param key Keycode associated with the key pressed
 * @param scancode Reports which keys have been pressed? (not sure on this one)
 * @param action Action associated with the key event (could be press, hold, release, etc.)
 * @param mods Any modifiers associated with key press (shift, etc.)
 * @author Part of 169 starter code
 */
void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (GetActiveWindow() == NULL) {
		return;
	}
	/*
	 * TODO: Modify below to add your key callbacks.
	 */
	float speed = 0.25f;
	glm::vec3 forward = glm::normalize(Cam->getDirection()) * speed;
	forward.y = 0.0f;
	glm::vec3 backward = -forward;
	glm::vec3 right = glm::normalize(glm::cross(Cam->getDirection(), glm::vec3(0.0f, 1.0f, 0.0f))) * speed;
	right.y = 0.0f;
	glm::vec3 left = -right;
	
	// Check for a key press.
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_ESCAPE:
			// Close the window. This causes the program to also terminate.
			//glfwSetWindowShouldClose(window, GL_TRUE);
			break;
		case GLFW_KEY_LEFT_CONTROL:
			player->setState(player->crouch);

			break;
		case GLFW_KEY_LEFT_SHIFT:
			player->setState(player->sprint);
	
			break;
		case GLFW_KEY_F:
			player->setPickUpAbilityKey(true);
			break;
		case GLFW_KEY_E:
			player->setUseAbilityKey(true);
			break;
		case GLFW_KEY_W:
			player->setMoving(1);
			player->setInput(player->forward, 1);
			break;
		case GLFW_KEY_A:
			player->setMoving(1);
			player->setInput(player->left, 1);	
			break;
		case GLFW_KEY_S:
			player->setMoving(-1);
			player->setInput(player->backward, 1);
			break;
		case GLFW_KEY_D:
			player->setMoving(1);
			player->setInput(player->right, 1);
			break;
		case GLFW_KEY_Z:
			player->setInput(player->up, 1);
			break;
		case GLFW_KEY_X:
			player->setInput(player->down, 1);
			break;
		default:
			break;
		}
	}
	if (action == GLFW_RELEASE)
	{
		switch (key)
		{
		case GLFW_KEY_LEFT_CONTROL:
			player->setState(player->stand);
			break;
		case GLFW_KEY_LEFT_SHIFT:
			player->setState(player->stand);
			break;
		case GLFW_KEY_W:
			player->setMoving(0);
			player->setInput(player->forward, 0);
			break;
		case GLFW_KEY_A:
			player->setMoving(0);
			player->setInput(player->left, 0);
			break;
		case GLFW_KEY_S:
			player->setMoving(0);
			player->setInput(player->backward, 0);
			break;
		case GLFW_KEY_D:
			player->setMoving(0);
			player->setInput(player->right, 0);
			break;
		case GLFW_KEY_Z:
			player->setInput(player->up, 0);
			break;
		case GLFW_KEY_X:
			player->setInput(player->down, 0);
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
 * @param mods Any modifiers associated with mouse click (shift, etc.)
 * @author Part of 169 starter code
 */
void Window::mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && GetActiveWindow() != NULL) {
		LeftDown = (action == GLFW_PRESS);
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && GetActiveWindow() != NULL) {
		RightDown = (action == GLFW_PRESS);
	}

	if (LeftDown) {
		player->setHasFired(true);
		//player->setIsFiring(true);
		//player->shootWeapon(gm -> allBoundingBoxes);
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

	

	if (GetActiveWindow() == NULL) {
		return;
	}

	int maxDelta = 100;
	
	int dx = glm::clamp((int)currX - MouseX, -maxDelta, maxDelta);
	int dy = glm::clamp(-((int)currY - MouseY), -maxDelta, maxDelta);
	//cout << "dx: " << dx << endl;
	//cout << "dy:" << dy << endl;
	//cout << "currX: " << currX << endl;
	//cout << "currY: " << currY << endl;
	//cout << "mouseX: " << MouseX << endl;
	//cout << "mouseY: " << MouseY << endl;
	MouseX = (int)currX;
	MouseY = (int)currY;

	const float sensitivity = 0.25f;

	//updating camera viewing direction
	float yaw = Cam->getYaw();
	float pitch = Cam->getPitch();

	yaw += dx * sensitivity;
	
	//rotation animation stuff for player
	player->getPlayerModel()->rotateAnimation(dx * sensitivity * -0.01745f, player->getPlayerModelCenter());
	player->getPlayerModel()->playAnimation(player->getPlayerModel()->animationClipList.at(0), 0.0f, false);
	player->getPlayerGunModel()->rotate(dx * sensitivity * -0.01745f, player->getPlayerGunModelCenter());
	player->getPlayerGunModel()->rotateAnimation(dx * sensitivity * -0.01745f, player->getPlayerGunModelCenter());
	
	pitch += dy * sensitivity;
	Cam->setYaw(yaw);
	Cam->setPitch(pitch);

	//keeps cursor locked in the middle
	/*glfwSetCursorPos(window, width / 2, height / 2);
	MouseX = width / 2;
	MouseY = height / 2;*/
}

////////////////////////////////////////////////////////////////////////////////
