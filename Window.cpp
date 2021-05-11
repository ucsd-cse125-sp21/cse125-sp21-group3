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
string Window::playerInputString;
bool Window::isForwardPressed;
bool Window::isRightPressed;
bool Window::isLeftPressed;
bool Window::isBackwardPressed;
bool Window::isCrouched;
bool Window::isSprinting;
glm::vec3 Window::playerDirection;
bool Window::hasFired;
map<int, Opponent*> Window::opponentMap;
Cube* Window::cube;

//temp opponent variables
int createOpponent = -1;

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
	

	//player setup
	player = new Player(Cam->getPosition(), maze);
	player->setPlayerCamera(Cam);
	player->setSoundEngine(soundEngine);

	game->myPlayer = player;

	boundingBoxList = maze->getBoundingBox();


	boundingBoxList.push_back(ground->getBoundingBox());
	boundingBoxList.push_back(player->getBoundingBox());

	glm::mat4 chestRootTransform(1.0f);
	chestRootTransform = glm::translate(chestRootTransform, glm::vec3(2.0f, 0.0f, 2.0f));
	chestRootTransform = glm::rotate(chestRootTransform, 1.57f, glm::vec3(0.0f, 1.0f, 0.0f));
	//chest = new Model("Assets/chestOpen.gltf", chestRootTransform);
	chest = new Model("C:/Users/Calpok/Desktop/CSE 125/chestOpen.gltf", chestRootTransform);
	
	glm::mat4 gunRootTransform(1.0f);
	gunRootTransform = glm::scale(gunRootTransform, glm::vec3(0.5f, 0.5f, 0.5f));
	gunRootTransform = glm::translate(gunRootTransform, glm::vec3(7.0f, 2.0f, 10.0f));
	//gun = new Model("Assets/pistolReload.gltf", gunRootTransform);
	gun = new Model("C:/Users/Calpok/Desktop/CSE 125/pistolReload.gltf", gunRootTransform);

	glm::mat4 characterRootTransform(1.0f);
	characterRootTransform = glm::scale(characterRootTransform, glm::vec3(0.335f, 0.335f, 0.335f));
	characterRootTransform = glm::translate(characterRootTransform, glm::vec3(7.0f, 0.0f, 2.0f));
	//character = new Model("Assets/character.gltf", characterRootTransform);
	character = new Model("C:/Users/Calpok/Desktop/CSE 125/character.gltf", characterRootTransform);




	//Networking Stuff - initial setup
	Window::isForwardPressed = false;
	Window::isRightPressed = false;
	Window::isLeftPressed = false;
	Window::isBackwardPressed = false;
	Window::isCrouched = false;
	Window::isSprinting = false;
	Window::playerDirection = player->getPlayerCamera()->getDirection();
	Window::hasFired = false;
	constructPlayerInputString();
	//Window::updateOpponent(3, glm::vec3(3.0f, 3.5f, 3.0f), glm::vec3(0.0f, 3.5, 0.0f - 3.0f), 0);


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


void Window::constructPlayerInputString() {

	string tempPlayerInputString = "";

	if (Window::isForwardPressed) {
		tempPlayerInputString += "true";
	}
	else {
		tempPlayerInputString += "false";
	}
	tempPlayerInputString += ",";


	if (Window::isRightPressed) {
		tempPlayerInputString += "true";
	}
	else {
		tempPlayerInputString += "false";
	}
	tempPlayerInputString += ",";


	if (Window::isLeftPressed) {
		tempPlayerInputString += "true";
	}
	else {
		tempPlayerInputString += "false";
	}
	tempPlayerInputString += ",";


	if (Window::isBackwardPressed) {
		tempPlayerInputString += "true";
	}
	else {
		tempPlayerInputString += "false";
	}
	tempPlayerInputString += ",";


	if (Window::isCrouched) {
		tempPlayerInputString += "true";
	}
	else {
		tempPlayerInputString += "false";
	}
	tempPlayerInputString += ",";


	if (Window::isSprinting) {
		tempPlayerInputString += "true";
	}
	else {
		tempPlayerInputString += "false";
	}
	tempPlayerInputString += ",";


	//TODO, update how input message is parsed so that it can handle vec3 for direction instead of float
	tempPlayerInputString += to_string(playerDirection.x);
	tempPlayerInputString += ",";


	if (Window::hasFired) {
		tempPlayerInputString += "true";
	}
	else {
		tempPlayerInputString += "false";
	}


	//set playerInputString at the end so playerInputString always holds a constant value
	Window::playerInputString = tempPlayerInputString;
}

/*
 * This method is called every frame and performs any operations needed to update
 * the game state before everything is drawn.
 *
 * @author Part of 169 starter code
 */
void Window::idleCallback()
{
	// Perform any updates as necessary.
	//Cam->Update();
	player->setVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
	string tempPlayerInputString = "";
	Window::isForwardPressed = false;
	Window::isRightPressed = false;
	Window::isLeftPressed = false;
	Window::isBackwardPressed = false;
	
	if (GetAsyncKeyState(GLFW_KEY_W)) {
		player->moveDirection(player->forward);
		Window::isForwardPressed = true;
	}
	if (GetAsyncKeyState(GLFW_KEY_D)) {
		player->moveDirection(player->right);
		Window::isRightPressed = true;
	}
	if (GetAsyncKeyState(GLFW_KEY_A)) {
		player->moveDirection(player->left);
		Window::isLeftPressed = true;
	}
	if (GetAsyncKeyState(GLFW_KEY_S)) {
		player->moveDirection(player->backward);
		Window::isBackwardPressed = true;
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
	player->update(0.01f, boundingBoxList);
	//chest->playAnimation(chest->animationClipList.at(0), 0.01f);
	//gun->playAnimation(gun->animationClipList.at(0), 0.05f, false);
	//character->playAnimation(character->animationClipList.at(0), 0.05f);


	//Networking Stuff
	//------------------------------------------------------------------------
	constructPlayerInputString();
	if (createOpponent != -1) {
		opponentMap.insert(pair<int, Opponent*>(createOpponent, new Opponent(createOpponent, glm::vec3(3.0f, 3.5f, 3.0f))));
		createOpponent = -1;
	}
	for (pair<int, Opponent*> p : Window::opponentMap) {
		p.second->update();
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

	for (Cube* footprint : player->getFootprints()) {
		//footprint->draw(Cam->GetViewProjectMtx(), Window::shaderProgram);
	}

	player->draw(Cam->GetViewProjectMtx(), Window::shaderProgram);
	for(pair<int, Opponent*> p : Window::opponentMap) {
		p.second->draw(Cam->GetViewProjectMtx(), Window::shaderProgram);
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

	for (Cube* abilityChests : maze->getChests())
	{
		abilityChests->draw(Cam->GetViewProjectMtx(), Window::shaderProgram);
	}

	//character->draw(Cam->GetViewProjectMtx(), Window::shaderProgram);
	//cube->draw(Cam->GetViewProjectMtx(), Window::shaderProgram);
	//cube2->draw(Cam->GetViewProjectMtx(), Window::shaderProgram);
	//drawCrosshair();

	ground->draw(Cam->GetViewProjectMtx(), Window::shaderProgram);


	// Gets events, including input such as keyboard and mouse or window resizing.
	glfwPollEvents();
	
	drawCrosshair();
	
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
			Window::isCrouched = true;
			break;
		case GLFW_KEY_LEFT_SHIFT:
			player->setState(player->sprint);
			Window::isSprinting = true;
			break;
		case GLFW_KEY_F:
			player->pickUpAbility();
			break;
		case GLFW_KEY_E:
			player->useAbility();
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
			Window::isCrouched = false;
			break;
		case GLFW_KEY_LEFT_SHIFT:
			player->setState(player->stand);
			Window::isSprinting = false;
			break;
		default:
			break;
		}
	}

	//Networking Stuff
	//------------------------------------------------------------------------
	constructPlayerInputString();
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

	if (LeftDown && !Window::hasFired) {
		std::cerr << "Fired" << std::endl;
		Window::hasFired = true;
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

void Window::updateOpponent(int id, glm::vec3 position, glm::vec3 direction, int moving) {

	if (Window::opponentMap.find(id) == Window::opponentMap.end()) {
		createOpponent = id;
	}
	else {
		Opponent* opponent = Window::opponentMap.find(id)->second;
		opponent->setPosition(position);
		opponent->setDirection(direction);
		opponent->setMoving(moving);
	}

}
////////////////////////////////////////////////////////////////////////////////
