#pragma once

#include "main.h"
#include<vector>

#include <glm/glm.hpp>

struct wallInfo {
	bool right;
	bool bottom;
};

class Maze {
public:
	int mazeSize;
	int mapScale;
	float wallWidth;

	Maze(int size, int scale);
	~Maze();


	std::vector<Cube*> createWalls();

	void printMaze();

	void generateMaze(int r_begin, int r_end, int c_begin, int c_end, bool direction);



private:

	wallInfo** mazeArray;

	std::vector<Cube*> walls;

};
