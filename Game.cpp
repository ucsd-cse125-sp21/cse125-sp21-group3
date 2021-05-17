#include "Game.h"

Game::Game(bool client)
{
	isClient = client;
	maze = new Maze(21, 7, isClient);
	gameBegun = false;
	gameSet = false;

}

Game::~Game()
{

}

void Game::beginGame()
{
	maze->createWalls();
	maze->createAbilityChests(15);
	//myPlayer = new Player(glm::vec3(1.0f, 1.0f, 1.0f), maze);
}

void Game::initiateGame()
{
	maze->generateGround();
	maze->generateWalls();
	maze->generateAbilityChests();
	gameBegun = true;
	for (auto boundingBox : maze->getBoundingBox())
	{
		allBoundingBoxes.push_back(boundingBox);
	}
}


void Game::update(float deltaTime)
{
	for (int i = 0; i < allPlayers.size(); i++)
	{
		Player* player = allPlayers.at(i);
		player->update(deltaTime, this);
	}
}
