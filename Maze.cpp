#include "Maze.h"


Maze::Maze(int size, int scale)
{
	mazeSize = size;
	mazeArray = new wallInfo*[mazeSize];
	for (int i = 0; i < mazeSize; i++)
	{
		mazeArray[i] = new wallInfo[mazeSize];
	}
	// Initialize maze to 0
	for (int r = 0; r < mazeSize; r++)
	{
		for (int c = 0; c < mazeSize; c++)
		{
			mazeArray[r][c].right = false;
			mazeArray[r][c].bottom = false;
		}
	}

	mapScale = scale;
	wallWidth = 0.1;
}

Maze::~Maze()
{
	for (int i = 0; i < mazeSize; i++)
	{
		delete[] mazeArray[i];
	}
	delete[] mazeArray;
}

std::vector<Cube*>  Maze::createWalls()
{
	// Set seed for random creation for testing purposes
	srand(1);
	// Set borders
	for (int i = 0; i < mazeSize; i++)
	{
		mazeArray[0][i].right = true; // Top
		mazeArray[i][0].bottom = true; // Left
		mazeArray[mazeSize - 1][i].right = true; // Bottom
		mazeArray[i][mazeSize - 1].bottom = true; // Right
	}

	// Generate random walls
	for (int r = 0; r < mazeSize - 1; r++)
	{
		for (int c = 0; c < mazeSize - 1; c++)
		{
			if (rand() % 2)
			{
				mazeArray[r][c].right = true;
			}
			if (rand() % 2)
			{
				mazeArray[r][c].bottom = true;
			}
		}
	}





	// Create right walls (horizontal)
	for (int r = 0; r < mazeSize; r++)
	{
		bool prev = false;
		int lowerBound = 0;
		for (int c = 0; c < mazeSize; c++)
		{
			if (mazeArray[r][c].right)
			{
				if (!prev)
				{
					lowerBound = c;
				}
				prev = true;

			}
			else
			{
				if (prev)
				{
					Cube* newWall = new Cube(glm::vec3(r*mapScale, 0, lowerBound*mapScale), glm::vec3(r*mapScale + wallWidth, 5, c*mapScale));
					walls.push_back(newWall);
				}
				prev = false;
			}
		}
		if (prev)
		{
			Cube* newWall = new Cube(glm::vec3(r*mapScale, 0, lowerBound*mapScale), glm::vec3(r*mapScale + wallWidth, 5, (mazeSize - 1)*mapScale));
			walls.push_back(newWall);
		}
	}

	// Create bottom walls (vertical)
	for (int c = 0; c < mazeSize; c++)
	{
		bool prev = false;
		int lowerBound = -1;
		for (int r = 0; r < mazeSize; r++)
		{
			if (mazeArray[r][c].bottom)
			{
				if (!prev)
				{
					lowerBound = r;
				}
				prev = true;

			}
			else
			{
				if (prev)
				{
					Cube* newWall = new Cube(glm::vec3(lowerBound*mapScale, 0, c * mapScale), glm::vec3(r * mapScale, 5, c * mapScale + wallWidth));
					walls.push_back(newWall);
				}
				prev = false;
			}
		}
		if (prev)
		{
			Cube* newWall = new Cube(glm::vec3(lowerBound * mapScale, 0, c * mapScale), glm::vec3((mazeSize - 1) * mapScale, 5, c * mapScale + wallWidth));
			walls.push_back(newWall);
		}
	}
	//printMaze();
	return walls;
}

void Maze::printMaze()
{
	std::cout << "PRINTING MAZE:" << std::endl;

	for (int r = 0; r < mazeSize; r++)
	{
		for (int c = 0; c < mazeSize; c++)
		{
			char outputChar;
			int numWalls = 0;
			if (mazeArray[r][c].right)
			{
				outputChar = '_';
				std::cout << outputChar;
				numWalls++;
			}
			if (mazeArray[r][c].bottom)
			{
				outputChar = '|';
				std::cout << outputChar;
				numWalls++;
			}
			for (int i = numWalls; i < 2; i++)
			{
				std::cout << " ";
			}
		}
		std::cout << std::endl;
	}
}