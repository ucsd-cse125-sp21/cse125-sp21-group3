#pragma once

#ifndef SERVERPARSE_H
#define SERVERPARSE_H

#include <iostream>
#include <cstdlib>
#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>
#include <unordered_map>
#include <queue>
#include "../Game.h"
#include "../Maze.h"

using namespace std;
using namespace boost::asio;
using ip::tcp;





class serverParse {
public:


    /*
 * Field variables.
 */
    static const string MESSAGE_TAIL; // Attatched to the end of every message.
    //unordered_map<string, Client> idClientMap;  // Map unique userId's with each client.
    static int userIdCount; // Incrementing counter that assigns new userId's

 //   static const int MAZE_SIZE = 4; // Height and width of maze.

 //   /*
 //* Mazes are represented by a 2D-Array.  A hidden wall is represented by 0.  A
 //* east-west wall is represented by a 1.  A north-south wall is represented by
 //* a 2.
 //*/

 //   int mazeArr[MAZE_SIZE][MAZE_SIZE];
    static string createMazeString(Maze* maze);
    static string createAbilityString(Maze* maze);


    static void updateMaze(int row, int col, int wallState);
    static void joinMessageHandler();
    static void leaveMessageHandler(string clientId);
    static void inputMessageHandler(Game* game, vector<string> messageValues);
    static void sortClientMessage(Game* game, string clientMessage);
    static string buildJoinResponse(string clientId);
    static string buildPlayerMessage(Game* game, string clientId);
    static string buildStartMessage();
    static string buildMazeInitialMessage();
    static string buildMazeUpdateMessage(Game* game);
    static void printArray();


};

#endif