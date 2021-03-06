#include "Maze.h"
#include <time.h> 
// Iniitalize maze
Maze::Maze(int size, int scale, Game* gm, bool client)
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
			mazeArray[r][c].ability = Player::none;
			mazeArray[r][c].wallRight = NULL;
			mazeArray[r][c].wallBottom = NULL;
			mazeArray[r][c].abilityChest = NULL;
		}
	}

	mapScale = scale;
	wallWidth = 0.1f;
	wallHeight = 5.0f;

	isClient = client;
	game = gm;

	// Set seed for random creation for testing purposes
	//srand(0);
	srand(time(NULL));

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



Cube * Maze::generateGround()
{
	ground = new Cube(glm::vec3(0, -1.0f, 0.0f), glm::vec3((mazeSize - 1) * mapScale, 0.0f, (mazeSize - 1) * mapScale), Cube::border, isClient);
	ground->setColor(glm::vec3(0.1f, 0.1f, 0.1f));
	glm::mat4 groundModel = ground->getModel();

	boundingBoxList.push_back(ground->getBoundingBox());
	return ground;
}




void Maze::createAbilityChests(int numChests)
{
	int numAbilities = numChests;
	for (int i = 0; i < numAbilities; i++)
	{
		int abilityType = rand() % 8;
		while (abilityType == Player::none || abilityType == Player::trackPlayer || abilityType == Player::opened || abilityType == Player::seeMap )
		{
			abilityType = rand() % 8;
		}
		// Choose ability to test
		//abilityType = Player::seeMap;
		int row = rand() % (mazeSize - 1);
		int column = rand() % (mazeSize - 1);
		if (mazeArray[row][column].ability == Player::none)
		{
			mazeArray[row][column].ability = abilityType;
		}
		else
		{
			numAbilities++;
		}
	}
	return;
}


std::vector<Model*> Maze::generateAbilityChests()
{
	for (int r = 0; r < mazeSize; r++)
	{
		for (int c = 0; c < mazeSize; c++)
		{
			if (mazeArray[r][c].ability != Player::none) {
				glm::vec3 chestPosition((r + 0.5) * mapScale, 0.0f, (c + 0.5) * mapScale);
				glm::mat4 translation = glm::translate(glm::mat4(1.0f), chestPosition);
				Model* chest = new Model("Assets/chestOpen.gltf", translation, isClient);
				chest->boundingBox = new BoundingBox(glm::vec3(chestPosition.x - 0.5f, 0.0f, chestPosition.z - 1.0f),
					glm::vec3(chestPosition.x + 0.5f, 1.5f, chestPosition.z + 1.0f), chest, true);
				abilityChests.push_back(chest);
				boundingBoxList.push_back(chest->getBoundingBox());
				chestBoundingBoxList.push_back(chest->getBoundingBox());
				mazeArray[r][c].abilityChest = chest;
			}
		}
	}
	return abilityChests;
}




// Generate the maze and create wall objects
void  Maze::createWalls()
{

	// Set borders
	// Row = X, column = Z
	for (int i = 0; i < mazeSize - 1; i++)
	{
		mazeArray[0][i].bottom = true; // Left border
		mazeArray[i][0].right = true; // Top border
		mazeArray[mazeSize - 1][i].bottom = true; // Right border
		mazeArray[i][mazeSize - 1].right = true; // Bottom border
	}

	// Generate random walls
	createWallsRecursion(0, mazeSize - 1, 0, mazeSize - 1, rand() % 2);


	// Add an end
	//mazeArray[mazeSize - 2][mazeSize - 1].right = false;

	// Add a beginning
	//mazeArray[0][0].right = false;
}


std::vector<Cube*> Maze::generateWalls()
{
	// Create Unit walls
	for (int r = 0; r < mazeSize; r++)
	{
		for (int c = 0; c < mazeSize; c++)
		{
			int canDelete = Cube::wall;
			if (mazeArray[r][c].bottom)
			{
				if (r == 0 || r == mazeSize - 1)
				{
					canDelete = Cube::border;
				}
				Cube* newWall = new Cube(glm::vec3(r * mapScale, -1.2f, c * mapScale), glm::vec3(r * mapScale + wallWidth, wallHeight, (c + 1) * mapScale + wallWidth), canDelete, isClient);
				walls.push_back(newWall);
				boundingBoxList.push_back(newWall->getBoundingBox());
				mazeArray[r][c].wallBottom = newWall;
				if (r == 0 || r == mazeSize - 1)
				{
					newWall->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
				}
			}
			canDelete = Cube::wall;
			if (mazeArray[r][c].right)
			{
				if (c == 0 || c == mazeSize - 1)
				{
					canDelete = Cube::border;
				}
				Cube* newWall = new Cube(glm::vec3(r * mapScale, -1.2f, c * mapScale), glm::vec3((r + 1) * mapScale, wallHeight, c * mapScale + wallWidth), canDelete, isClient);
				walls.push_back(newWall);
				boundingBoxList.push_back(newWall->getBoundingBox());
				mazeArray[r][c].wallRight = newWall;
				if (c == 0 || c == mazeSize - 1)
				{
					newWall->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
				}
			}
		}
	}
	return walls;
}

	

	// Create connected walls as single object
	// Create right walls (horizontal)
	//for (int r = 0; r < mazeSize; r++)
	//{
	//	bool prev = false;
	//	int lowerBound = 0;
	//	for (int c = 0; c < mazeSize; c++)
	//	{
	//		// Wall continues on here
	//		if (mazeArray[r][c].right)
	//		{
	//			if (!prev)
	//			{
	//				lowerBound = c;
	//			}
	//			prev = true;
	//		}
	//		// Wall ends here
	//		else
	//		{
	//			if (prev)
	//			{
	//				Cube* newWall = new Cube(glm::vec3(r*mapScale, -2.0f, lowerBound*mapScale), glm::vec3(r*mapScale + wallWidth, wallHeight, c*mapScale));
	//				walls.push_back(newWall);
	//				boundingBoxList.push_back(newWall->getBoundingBox());
	//			}
	//			prev = false;
	//		}
	//	}
	//	// Wall goes to edge of maze
	//	if (prev)
	//	{
	//		Cube* newWall = new Cube(glm::vec3(r*mapScale, -2.0f, lowerBound*mapScale), glm::vec3(r*mapScale + wallWidth, wallHeight, (mazeSize - 1)*mapScale));
	//		walls.push_back(newWall);
	//	}
	//}
	//
	//// Create bottom walls (vertical)
	//for (int c = 0; c < mazeSize; c++)
	//{
	//	bool prev = false;
	//	int lowerBound = -1;
	//	for (int r = 0; r < mazeSize; r++)
	//	{
	//		// Wall continues on here
	//		if (mazeArray[r][c].bottom)
	//		{
	//			if (!prev)
	//			{
	//				lowerBound = r;
	//			}
	//			prev = true;

	//		}
	//		// Wall ends here
	//		else
	//		{
	//			if (prev)
	//			{
	//				Cube* newWall = new Cube(glm::vec3(lowerBound*mapScale, -2.0f, c * mapScale), glm::vec3(r * mapScale, wallHeight, c * mapScale + wallWidth));
	//				walls.push_back(newWall);
	//			}
	//			prev = false;
	//		}
	//	}
	//	// Wall goes to edge of maze
	//	if (prev)
	//	{
	//		Cube* newWall = new Cube(glm::vec3(lowerBound * mapScale, -2.0f, c * mapScale), glm::vec3((mazeSize - 1) * mapScale, wallHeight, c * mapScale + wallWidth));
	//		walls.push_back(newWall);
	//	}
	//}
	//



// Generate maze structure
void Maze::createWallsRecursion(int r_begin, int r_end, int c_begin, int c_end, bool direction)
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

		// Walls going bottom
		if (direction)
		{
			int randomRow = rand() % (r_end - r_begin - 1) + r_begin + 1;
			int randomOpening = rand() % (c_end - c_begin) + c_begin;

			// Set all points to a wall, except for the opening
			for (int i = c_begin; i < c_end; i++)
			{
				if (i != randomOpening)
				{
					mazeArray[randomRow][i].bottom = true;
				}
			}
			// Recursively call for the other two sides (created from the wall currently bisecting the area into two)
			createWallsRecursion(r_begin, randomRow, c_begin, c_end, chooseDirection1);
			createWallsRecursion(randomRow, r_end, c_begin, c_end, chooseDirection2);

		}
		// Walls going right
		else
		{
			int randomColumn = rand() % (c_end - c_begin - 1) + c_begin + 1;
			int randomOpening = rand() % (r_end - r_begin) + r_begin;

			// Set all points to a wall, except for the opening
			for (int i = r_begin; i < r_end; i++)
			{
				if (i != randomOpening)
				{
					mazeArray[i][randomColumn].right = true;
				}
			}

			// Recursively call for the other two sides (created from the wall currently bisecting the area into two)
			createWallsRecursion(r_begin, r_end, c_begin, randomColumn, chooseDirection1);
			createWallsRecursion(r_begin, r_end, randomColumn, c_end, chooseDirection2);
		}
		return;

	}
}



int* Maze::getCoordinates(glm::vec3 position)
{
	int* coordinates = new int[2];
	coordinates[0] = position[0] / mapScale;
	coordinates[1] = position[2] / mapScale;

	return coordinates;
}



int Maze::getAbility(int r, int c)
{
	return mazeArray[r][c].ability;
}



void Maze::removeAbility(int r, int c)
{
	mazeArray[r][c].ability = Player::opened;
	if (!isClient)
	{
		string inputMessage = "deleteAbility," + to_string(r) + "," + to_string(c) + ",";
		game->addServerInputMessage(inputMessage);
		// cout << "deleting:" << r << "|" << c << endl;
	}
	if (mazeArray[r][c].abilityChest)
	{
		//delete mazeArray[r][c].abilityChest;
		//mazeArray[r][c].abilityChest = NULL;
	}

}



void Maze::setWall(int r, int c, bool direction, bool exist)
{
	if (direction)
	{
		mazeArray[r][c].bottom = exist;
		if (!exist && mazeArray[r][c].wallBottom)
		{
			delete mazeArray[r][c].wallBottom;
			//mazeArray[r][c].wallBottom = NULL;
		}
	}
	else
	{
		mazeArray[r][c].right = exist;
		if (!exist && mazeArray[r][c].wallRight)
		{
			delete mazeArray[r][c].wallRight;
			//mazeArray[r][c].wallRight = NULL;
		}
	}
	if (!isClient && !exist)
	{
		string inputMessage = "deleteWall," + to_string(r) + "," + to_string(c) + "," + to_string((int) direction) + ",";
		game->addServerInputMessage(inputMessage);
	}
}


void Maze::setAbility(int r, int c, int ab)
{
	mazeArray[r][c].ability = ab;
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