#include "Maze.h"

// Iniitalize maze
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


// Delete maze
Maze::~Maze()
{
	for (int i = 0; i < mazeSize; i++)
	{
		delete[] mazeArray[i];
	}
	delete[] mazeArray;
}


// Generate the maze and create wall objects
std::vector<Cube*>  Maze::createWalls()
{
	// Set seed for random creation for testing purposes
	srand(0);
	//srand(time(NULL));
	// Set borders
	// Row = X, column = Z
	for (int i = 0; i < mazeSize; i++)
	{
		mazeArray[0][i].right = true; // Top
		mazeArray[i][0].bottom = true; // Left
		mazeArray[mazeSize - 1][i].right = true; // Bottom
		mazeArray[i][mazeSize - 1].bottom = true; // Right
	}

	// Generate random walls
	generateMaze(0, mazeSize - 1, 0, mazeSize - 1, rand() % 2);

	//printMaze();

	// Add an end
	mazeArray[mazeSize - 1][mazeSize - 2].right = false;




	// Create right walls (horizontal)
	for (int r = 0; r < mazeSize; r++)
	{
		bool prev = false;
		int lowerBound = 0;
		for (int c = 0; c < mazeSize; c++)
		{
			// Wall continues on here
			if (mazeArray[r][c].right)
			{
				if (!prev)
				{
					lowerBound = c;
				}
				prev = true;
			}
			// Wall ends here
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
		// Wall goes to edge of maze
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
			// Wall continues on here
			if (mazeArray[r][c].bottom)
			{
				if (!prev)
				{
					lowerBound = r;
				}
				prev = true;

			}
			// Wall ends here
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
		// Wall goes to edge of maze
		if (prev)
		{
			Cube* newWall = new Cube(glm::vec3(lowerBound * mapScale, 0, c * mapScale), glm::vec3((mazeSize - 1) * mapScale, 5, c * mapScale + wallWidth));
			walls.push_back(newWall);
		}
	}
	return walls;
}



// Generate maze structure
void Maze::generateMaze(int r_begin, int r_end, int c_begin, int c_end, bool direction)
{
	if (r_begin >= r_end - 1 || c_begin >= c_end - 1)
	{
		return;
	}
	else
	{
		// Bias towards having walls that bisect the area
		bool chooseDirection1 = rand() % (r_end - r_begin + c_end - c_begin) <= (r_end - r_begin);
		bool chooseDirection2 = rand() % (r_end - r_begin + c_end - c_begin) <= (r_end - r_begin);

		// Walls going right
		if (direction)
		{
			int randomRow = rand() % (r_end - r_begin - 1) + r_begin + 1;
			int randomOpening = rand() % (c_end - c_begin) + c_begin;

			// Set all points to a wall, except for the opening
			for (int i = c_begin; i < c_end; i++)
			{
				if (i != randomOpening)
				{
					mazeArray[randomRow][i].right = true;
				}
			}
			// Recursively call for the other two sides (created from the wall currently bisecting the area into two)
			generateMaze(r_begin, randomRow, c_begin, c_end, chooseDirection1);
			generateMaze(randomRow, r_end, c_begin, c_end, chooseDirection2);

		}
		// Walls going bottom/down
		else
		{
			int randomColumn = rand() % (c_end - c_begin - 1) + c_begin + 1;
			int randomOpening = rand() % (r_end - r_begin) + r_begin;

			// Set all points to a wall, except for the opening
			for (int i = r_begin; i < r_end; i++)
			{
				if (i != randomOpening)
				{
					mazeArray[i][randomColumn].bottom = true;
				}
			}

			// Recursively call for the other two sides (created from the wall currently bisecting the area into two)
			generateMaze(r_begin, r_end, c_begin, randomColumn, chooseDirection1);
			generateMaze(r_begin, r_end, randomColumn, c_end, chooseDirection2);
		}
		return;

	}
}



// Print out maze to console
void Maze::printMaze()
{
	std::cout << "PRINTING MAZE:" << std::endl;

	for (int r = 0; r < mazeSize; r++)
	{
		for (int c = 0; c < mazeSize; c++)
		{
			char outputChar;
			int numWalls = 0;
			if (mazeArray[r][c].bottom)
			{
				outputChar = '|';
				std::cout << outputChar;
				numWalls++;
			}
			if (mazeArray[r][c].right)
			{
				outputChar = '_';
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