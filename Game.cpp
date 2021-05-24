#include "Game.h"

Game::Game(bool client)
{
	isClient = client;
	maze = new Maze(21, 7, this, isClient);
	gameBegun = false;
	gameSet = false;
	serverMessage = "";
	gameTime = 0.0f;

}

Game::~Game()
{

}

void Game::beginGame()
{
	maze->createWalls();
	maze->createAbilityChests(30);
	//myPlayer = new Player(glm::vec3(1.0f, 1.0f, 1.0f), maze);
}

void Game::initiateGame()
{
	cout << "Game initiated" << endl;
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
		if (player->getId() != -1) {
			player->update(deltaTime, this);
		}
	}

	gameTime += deltaTime;
}

string Game::getClientInputMessage()
{
	string inputMessage = "";
	if (myPlayer)
	{
		inputMessage = myPlayer->getPlayerInputString();
	}
	inputMessage += MESSAGE_TAIL;
	return inputMessage;

}

string Game::getServerInputMessage()
{
	string inputMessage = "";
	for (int i = 0; i < allPlayers.size(); i++)
	{
		Player* player = allPlayers.at(i);
		//cout << "p = " + to_string(p) + ", pid_str = " + (playerConnections[p]->pid_str) + "\n";
		inputMessage += player->getPlayerInfoString();
	}
	inputMessage += serverMessage;
	inputMessage += MESSAGE_TAIL;
	serverMessage = "";
	return inputMessage;
}

void Game::addServerInputMessage(string message)
{
	cout << "adding message:" << message << endl;
	serverMessage += message;
}