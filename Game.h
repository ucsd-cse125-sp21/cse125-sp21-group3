#pragma once

#ifndef GAME_H
#define GAME_H

#include "main.h"
#include <vector>
#include <glm/glm.hpp>
#include "Maze.h"
#include "Player.h"

class Game {
public:
	Game(bool client = true);
	~Game();

	void beginGame();
	void initiateGame();
	void update(float deltaTime);


	Maze* maze;
	Player* myPlayer;
	int myPlayerId;
	vector<Player*> allPlayers;

	vector<BoundingBox*> allBoundingBoxes;

	bool isClient;
	bool gameBegun;
	bool gameSet;
};

#endif