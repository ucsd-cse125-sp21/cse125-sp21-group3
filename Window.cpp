#include "Window.h"
#include "Cube.h"
#include "Player.h"
#include "Maze.h"
#include "Model.h"
#include <windows.h>
#include <glm/gtx/string_cast.hpp>
#include "Player.h"
#include "Camera.h"

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
const char* Window::windowTitle = "Game";
const int digitSegmentLength = 20;
const int digitSegmentThickness = 3;
float verticalDigitSegment[digitSegmentLength][digitSegmentThickness][3];
float horizontalDigitSegment[digitSegmentThickness][digitSegmentLength][3];

std::vector<Cube*> walls;

Cube* ground;
Player* player;
std::vector<BoundingBox*> boundingBoxList;

// Camera Properties
Camera* Cam;

irrklang::ISoundEngine* soundEngine = irrklang::createIrrKlangDevice();

// Interaction Variables
bool LeftDown, RightDown;
int MouseX, MouseY;

// The shader program id
GLuint Window::shaderProgram;

//toggle to see bounding boxes
bool Window::debugMode;
Model* chest;
Model* gun;
Model* character;

Maze* maze;


//Networking Stuff
Cube* Window::cube;
int Window::createOpponent;
vector<string> Window::messagesToServer;

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
	maze = game->maze;

	ground = maze->generateGround();

	walls = maze->generateWalls();

	//cout << "generate chests for client" << endl;
	maze->generateAbilityChests();
	

	//player setup
	player = new Player(Cam->getPosition(), maze);
	player->setPlayerCamera(Cam);
	player->setSoundEngine(soundEngine);

	game->myPlayer = player;
	game->allPlayers.push_back(player);
	cout << "setting player id to in window: " << game->myPlayerId << endl;
	player->setId(game->myPlayerId);

	boundingBoxList = maze->getBoundingBox();


	boundingBoxList.push_back(ground->getBoundingBox());
	boundingBoxList.push_back(player->getBoundingBox());

	glm::mat4 chestRootTransform(1.0f);
	chestRootTransform = glm::translate(chestRootTransform, glm::vec3(2.0f, 0.0f, 2.0f));
	chestRootTransform = glm::rotate(chestRootTransform, 1.57f, glm::vec3(0.0f, 1.0f, 0.0f));
	//chest = new Model("Assets/chestOpen.gltf", chestRootTransform);
	//chest = new Model("C:/Users/Calpok/Desktop/CSE 125/chestOpen.gltf", chestRootTransform);
	
	glm::mat4 gunRootTransform(1.0f);
	gunRootTransform = glm::scale(gunRootTransform, glm::vec3(0.5f, 0.5f, 0.5f));
	gunRootTransform = glm::translate(gunRootTransform, glm::vec3(7.0f, 2.0f, 10.0f));
	//gun = new Model("Assets/pistolReload.gltf", gunRootTransform);
	//gun = new Model("C:/Users/Calpok/Desktop/CSE 125/pistolReload.gltf", gunRootTransform);

	glm::mat4 characterRootTransform(1.0f);
	characterRootTransform = glm::scale(characterRootTransform, glm::vec3(0.335f, 0.335f, 0.335f));
	characterRootTransform = glm::translate(characterRootTransform, glm::vec3(7.0f, 0.0f, 2.0f));
	//character = new Model("Assets/character.gltf", characterRootTransform);
	//character = new Model("C:/Users/Calpok/Desktop/CSE 125/character.gltf", characterRootTransform);

	//initializing digit segments to represent health
	for (int y = 0; y < 20; y++)
	{
		for (int x = 0; x < 3; x++)
		{
			verticalDigitSegment[y][x][0] = 1.0f;
			verticalDigitSegment[y][x][1] = 0.0f;
			verticalDigitSegment[y][x][2] = 0.0f;
		}
	}

	for (int y = 0; y < 3; y++)
	{
		for (int x = 0; x < 20; x++)
		{
			horizontalDigitSegment[y][x][0] = 1.0f;
			horizontalDigitSegment[y][x][1] = 0.0f;
			horizontalDigitSegment[y][x][2] = 0.0f;
		}
	}

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
	for (BoundingBox* bound : maze->getBoundingBox())
	{
		delete bound;
	}

	delete maze;
	delete chest;
	delete gun;
	delete player;
	delete Cam;

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
	Cam = new Camera(glm::vec3(2.5f, 3.5f, 2.5f));
	Cam->SetAspect(float(width) / float(height));

	// initialize the interaction variables
	LeftDown = RightDown = false;
	MouseX = MouseY = 0;

	// Call the resize callback to make sure things get drawn immediately.
	Window::resizeCallback(window, width, height);
	
	//disable cursor
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

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

	Cam->SetAspect(float(width) / float(height));
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
	// Perform any updates as necessary.
	//Cam->Update();
	player->setVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
	player->setMoving(0);

	if (GetAsyncKeyState(GLFW_KEY_W)) {
		player->moveDirection(player->forward);
		player->setMoving(1);
	}
	if (GetAsyncKeyState(GLFW_KEY_D)) {
		player->moveDirection(player->right);
		player->setMoving(1);
	}
	if (GetAsyncKeyState(GLFW_KEY_A)) {
		player->moveDirection(player->left);
		player->setMoving(1);
	}
	if (GetAsyncKeyState(GLFW_KEY_S)) {
		player->moveDirection(player->backward);
		player->setMoving(-1);
	}
	
	//if (GetAsyncKeyState(GLFW_KEY_E)) {
	//	player->useAbility();
	//}
	//if (GetAsyncKeyState(GLFW_KEY_F)) {
	//	player->pickUpAbility();
	//}
	// Allow player to move up and down for debugging
	if (GetAsyncKeyState(GLFW_KEY_X)) {
		player->moveDirection(player->down);
	}
	if (GetAsyncKeyState(GLFW_KEY_Z)) {
		player->moveDirection(player->up);
	}

	//update all players in the game	
	for (int i = 0; i < game->allPlayers.size(); i++) {
		game->allPlayers.at(i)->update(0.01f, boundingBoxList, game);
	}

	//chest->playAnimation(chest->animationClipList.at(0), 0.01f);
	//gun->playAnimation(gun->animationClipList.at(0), 0.05f, false);
	//character->playAnimation(character->animationClipList.at(0), 0.05f);


	//Networking Stuff
	//------------------------------------------------------------------------
	if (Window::createOpponent != -1) {
		cout << "creating player: " << Window::createOpponent << endl;
		Player* p = new Player(glm::vec3(3.0f, 3.5f, 3.0f), game->maze, true);

		//changing position for testing purposes
		p->getPlayerModel()->rootModel[3][2] -= 5.0f;
		p->getPlayerGunModel()->rootModel[3][2] -= 5.0f;
		////////////////////////////////////////

		p->setId(Window::createOpponent);
		game->allPlayers.push_back(p);
		cout << "added player successfully" << endl;
		Window::createOpponent = -1;
	}
	//------------------------------------------------------------------------
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
 * Draws health in the form of digits
 *
 * @author Lucas Hwang
 */

void Window::drawDigit(int startingX, int startingY, vector<bool> segmentsUsed) {

	
	glWindowPos2i(startingX, startingY);
	if (segmentsUsed.at(0)) {
		glDrawPixels(digitSegmentLength, digitSegmentThickness, GL_RGB, GL_FLOAT, horizontalDigitSegment);
	}
	if (segmentsUsed.at(1)) {
		glDrawPixels(digitSegmentThickness, digitSegmentLength, GL_RGB, GL_FLOAT, verticalDigitSegment);
	}
	glWindowPos2i(startingX + digitSegmentLength - digitSegmentThickness, startingY);
	if (segmentsUsed.at(2)) {
		glDrawPixels(digitSegmentThickness, digitSegmentLength, GL_RGB, GL_FLOAT, verticalDigitSegment);
	}
	glWindowPos2i(startingX, startingY + digitSegmentLength - digitSegmentThickness);
	if (segmentsUsed.at(3)) {
		glDrawPixels(digitSegmentLength, digitSegmentThickness, GL_RGB, GL_FLOAT, horizontalDigitSegment);
	}
	if (segmentsUsed.at(4)) {
		glDrawPixels(digitSegmentThickness, digitSegmentLength, GL_RGB, GL_FLOAT, verticalDigitSegment);
	}
	glWindowPos2i(startingX + digitSegmentLength - digitSegmentThickness, startingY + digitSegmentLength - digitSegmentThickness);
	if (segmentsUsed.at(5)) {
		glDrawPixels(digitSegmentThickness, digitSegmentLength, GL_RGB, GL_FLOAT, verticalDigitSegment);
	}
	glWindowPos2i(startingX, startingY + 2 * (digitSegmentLength - digitSegmentThickness));
	if (segmentsUsed.at(6)) {
		glDrawPixels(digitSegmentLength, digitSegmentThickness, GL_RGB, GL_FLOAT, horizontalDigitSegment);
	}
}

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

		drawDigit(startingX, height - 50, segmentsUsed);
		startingX += 30;
	}
	
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
	// Clear the color and depth buffers.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (Cube* footprint : player->getFootprints()) {
		//footprint->draw(Cam->GetViewProjectMtx(), Window::shaderProgram);
	}

	//player->draw(Cam->GetViewProjectMtx(), Window::shaderProgram);
	for (int i = 0; i < game->allPlayers.size(); i++) {
		game->allPlayers.at(i)->draw(Cam->GetViewProjectMtx(), Window::shaderProgram);
	}
	
	Camera* playCam = player->getPlayerCamera();
	irrklang::vec3df position(player->getPosition().x, player->getPosition().y, player->getPosition().z);        // position of the listener
	irrklang::vec3df lookDirection(playCam->getDirection().x, playCam->getDirection().y, playCam->getDirection().z); // the direction the listener looks into
	irrklang::vec3df velPerSecond(player->getVelocity().x, player->getVelocity().y, player->getVelocity().z);    // only relevant for doppler effects
	irrklang::vec3df upVector(0, 1, 0);        // where 'up' is in your 3D scene
	soundEngine->setListenerPosition(position, lookDirection, velPerSecond, upVector);

	for (Cube* wall : walls)
	{
		wall ->draw(Cam->GetViewProjectMtx(), Window::shaderProgram);
	}
	
	//chest->draw(Cam->GetViewProjectMtx(), Window::shaderProgram);
	//gun->draw(Cam->GetViewProjectMtx(), Window::shaderProgram);

	//for (Model* abilityChest : maze->getChests())
	//{
	//	//cout << "draw ability chest" << endl;
	//	if (abilityChest->opening && !abilityChest->opened) {
	//		if (abilityChest->animationClipList.at(0)->prevTime + 0.1f > abilityChest->animationClipList.at(0)->duration) {
	//			abilityChest->opening = false;
	//			abilityChest->opened = true;
	//		}
	//		else {
	//			abilityChest->playAnimation(abilityChest->animationClipList.at(0), 0.1f, false);
	//		}
	//	}
	//	abilityChest->draw(Cam->GetViewProjectMtx(), Window::shaderProgram);
	//}

	
	for (Model* abilityChest : maze->getChests())
	{
		wallInfo** mazeArray = maze->getMazeArray();
		glm::vec3 abilityChestLocation(abilityChest->rootModel[3][0], abilityChest->rootModel[3][1], abilityChest->rootModel[3][2]);
		int* abilityChestPos = maze->getCoordinates(abilityChestLocation);
		if (abilityChest->opening) { //if client is in the process of opening the chest
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

		abilityChest->draw(Cam->GetViewProjectMtx(), Window::shaderProgram);
	}


	//character->draw(Cam->GetViewProjectMtx(), Window::shaderProgram);
	//cube->draw(Cam->GetViewProjectMtx(), Window::shaderProgram);
	//cube2->draw(Cam->GetViewProjectMtx(), Window::shaderProgram);

	ground->draw(Cam->GetViewProjectMtx(), Window::shaderProgram);


	// Gets events, including input such as keyboard and mouse or window resizing.
	glfwPollEvents();
	
	drawCrosshair();
	drawHealth();
	
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
 * @param mods Any modifiers associated with key press (shift, etc.)
 * @author Part of 169 starter code
 */
void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
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
			glfwSetWindowShouldClose(window, GL_TRUE);
			break;
		case GLFW_KEY_LEFT_CONTROL:
			player->setState(player->crouch);

			break;
		case GLFW_KEY_LEFT_SHIFT:
			player->setState(player->sprint);
	
			break;
		case GLFW_KEY_F:
			player->pickUpAbility();
			break;
		case GLFW_KEY_E:
			player->useAbility();
			break;
		case GLFW_KEY_UP:
			player->setHealth(player->getHealth() + 1);
			break;
		case GLFW_KEY_DOWN:
			player->setHealth(player->getHealth() - 1);
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
		default:
			break;
		}
	}

	//Networking Stuff
	//------------------------------------------------------------------------

	//------------------------------------------------------------------------
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
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		LeftDown = (action == GLFW_PRESS);
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		RightDown = (action == GLFW_PRESS);
	}

	if (LeftDown) {
		player->setHasFired(true);
		player->setIsFiring(true);
		std::cerr << "Fired" << std::endl;
		player->shootWeapon(boundingBoxList);
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
	glfwSetCursorPos(window, width / 2, height / 2);
	MouseX = width / 2;
	MouseY = height / 2;
}

////////////////////////////////////////////////////////////////////////////////
