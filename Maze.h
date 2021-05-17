#pragma once

#ifndef MAZE_H
#define MAZE_H

#include "main.h"
#include "core.h"
#include<vector>
#include "Window.h"
#include <glm/glm.hpp>
#include "Player.h"

struct wallInfo {
	bool right;
	bool bottom;
	int ability;
	Cube* wallRight;
	Cube* wallBottom;
	Cube* abilityChest;
};

class Maze {
public:

	Maze(int size, int scale, bool client=true);
	~Maze();

	Cube* generateGround();

	void  createAbilityChests(int numChests);

	std::vector<Cube*> generateAbilityChests();


	void printMaze();

	void createWalls();
	std::vector<Cube*> generateWalls();

	void createWallsRecursion(int r_begin, int r_end, int c_begin, int c_end, bool direction);

	std::vector<Cube*> getWalls() { return walls; }

	std::vector<Cube*> getChests() { return abilityChests; }

	std::vector<BoundingBox*> getChestBoundingBox() { return chestBoundingBoxList; }

	std::vector<BoundingBox*> getBoundingBox() { return boundingBoxList; }

	int* getCoordinates(glm::vec3 position);
	int getAbility(int* coordinate);

	void removeAbility(int* coordinate);

	void setWall(int r, int c, bool direction, bool exist);
	void setAbility(int r, int c, int ab);

	int getMazeSize() { return mazeSize; }

	int getMapScale() { return mapScale; }

	wallInfo** getMazeArray() { return mazeArray; }

	Cube* getGround() { return ground; }


private:

	wallInfo** mazeArray;

	std::vector<Cube*> walls;

	std::vector<Cube*> abilityChests;


	std::vector<BoundingBox*> boundingBoxList;
	std::vector<BoundingBox*> chestBoundingBoxList;

	Cube* ground;

	int mazeSize;
	int mapScale;

	float wallWidth;
	float wallHeight;

	bool isClient;

};

#endif