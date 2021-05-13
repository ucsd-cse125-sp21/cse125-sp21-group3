#pragma once

#ifndef GAME_H
#define GAME_H

#include "main.h"
#include<vector>
#include <glm/glm.hpp>
#include "Maze.h"
#include "Player.h"

class Game {
public:
	Game(bool client = true);
	~Game();

	void beginGame();

	Maze* maze;
	Player* myPlayer;
	int myPlayerId;
	vector<Player*> allPlayers;

	bool isClient;
};

#endif