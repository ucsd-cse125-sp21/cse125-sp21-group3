#include "Game.h"

Game::Game(bool client)
{
	isClient = client;
}

Game::~Game()
{

}

void Game::beginGame()
{
	maze = new Maze(21, 7);
	if (!isClient) {
		maze->createWalls();
	}
	//myPlayer = new Player(glm::vec3(1.0f, 1.0f, 1.0f), maze);
}