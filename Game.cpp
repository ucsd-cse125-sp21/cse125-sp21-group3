#include "Game.h"

Game::Game(bool client)
{
	isClient = client;
	maze = new Maze(21, 7, this, isClient);
	gameBegun = false;
	gameSet = false;
	serverMessage = "";
	gameTime = 0.0f;
	lastDeleteWallTime = 0.0f;



}

Game::~Game()
{

}

void Game::beginGame()
{
	maze->createWalls();
	maze->createAbilityChests(40);
	//myPlayer = new Player(glm::vec3(1.0f, 1.0f, 1.0f), maze);
	srand(time(NULL));
	for (int i = 0; i < allPlayers.size(); i++)
	{
		bool spaceNotUsed = true;
		while (spaceNotUsed)
		{
			int row = rand() % (maze->getMazeSize() - 1);
			int col = rand() % (maze->getMazeSize() - 1);
			bool notInUse = true;
			for (int j = 0; j < i; j++)
			{
				int* playerPos = maze->getCoordinates(allPlayers.at(j) -> getPosition());

				if (row == playerPos[0] && col == playerPos[1])
				{
					notInUse = false;
				}
			}
			if (notInUse)
			{
				spaceNotUsed = false;
				allPlayers.at(i)->setPosition(glm::vec3((row + 0.5) * maze->getMapScale(), 3.5f, (col + 0.5) * maze->getMapScale()));
			}

		}
	}
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

Player* Game::getPlayer(int id)
{
	Player* player = myPlayer;
	for (int i = 0; i < allPlayers.size(); i++)
	{
		if (id == allPlayers.at(i)->getId())
		{
			player = allPlayers.at(i);
			return player;
		}
	}
	return player;
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
	
	if (!isClient)
	{
		// Randomly remove a wall every x seconds
		if (gameTime >= lastDeleteWallTime + 10.0f)
		{
			bool exist = false;
			wallInfo** mazeArray = maze->getMazeArray();
			int row = 0;
			int column = 0;
			int direction = 0;
			int i = 0;
			while (!exist && i < 1000)
			{
				row = rand() % (maze->getMazeSize() - 1);
				column = rand() % (maze->getMazeSize() - 1);
				direction = rand() % 2;
				if (direction)
				{
					exist = mazeArray[row][column].bottom;
					if (exist)
					{
						exist = mazeArray[row][column].wallBottom->isDeletable();
					}
				}
				else
				{
					exist = mazeArray[row][column].right;
					if (exist)
					{
						exist = mazeArray[row][column].wallRight->isDeletable();
					}
				}
				i++;
				lastDeleteWallTime = gameTime;
			}
			if (i < 1000)
			{
				maze->setWall(row, column, direction, 0);
			}
		}
	}

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
	//cout << "adding message:" << message << endl;
	serverMessage += message;
}