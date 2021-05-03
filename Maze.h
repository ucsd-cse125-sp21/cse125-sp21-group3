#pragma once

#include "main.h"
#include<vector>
#include "Window.h"
#include <glm/glm.hpp>
#include "Player.h"

struct wallInfo {
	bool right;
	bool bottom;
	int ability;
};

class Maze {
public:

	Maze(int size, int scale);
	~Maze();


	std::vector<Cube*> createWalls();

	Cube* createGround();

	std::vector<Cube*>  createAbilityChests();

	void printMaze();

	void generateMaze(int r_begin, int r_end, int c_begin, int c_end, bool direction);

	std::vector<Cube*> getWalls() { return walls; }

	std::vector<Cube*> getChests() { return abilityChests; }

	std::vector<BoundingBox*> getChestBoundingBox() { return chestBoundingBoxList; }

	std::vector<BoundingBox*> getBoundingBox() { return boundingBoxList; }

	int* getCoordinates(glm::vec3 position);
	int getAbility(int* coordinate);

	void removeAbility(int* coordinate);


	int getMazeSize() { return mazeSize; }

	int getMapScale() { return mapScale; }


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

};
