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
	maze = new Maze(21, 7, isClient);
	maze->createWalls();
	maze->createAbilityChests(15);
	//myPlayer = new Player(glm::vec3(1.0f, 1.0f, 1.0f), maze);
}

void Game::initiateGame()
{
	maze->generateGround();
	maze->generateWalls();
	maze->generateAbilityChests();
}