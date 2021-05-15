#pragma once

#ifndef SERVERPARSE_H
#define SERVERPARSE_H

#include <iostream>
#include <cstdlib>
#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>
#include <unordered_map>
#include <queue>
#include "Game.h"

using namespace std;
using namespace boost::asio;
using ip::tcp;

/*
 * Example messages for server parsing.  It assumes the MESSAGE_TAIL has already
 * been removed by a read_until method.
 */
const string JOIN_MESSAGE = "join";
const string LEAVE_MESSAGE = "leave,0";
const string INPUT_MESSAGE = "input,1,true,false,false,false,true,false,300.58,false";




/*
 * Store information about the client including the most recent input message
 * data.
 */
class Client {

public:
    double positionX = 0;
    double positionY = 0;
    int health = 100;

    int keyDirection;
    bool isCrouching = false;
    bool isSprinting = false;
    double playerDirection;
    bool isFiring = false;

    string unsentMazeUpdates = "";

    /*
     * Must somehow maintain connection with client.  Server still needs to
     * send player, maze, and start messages to client before regular
     * messagin begins.
     *
     * Socket might not be the right thing to store here, but whatever is
     * needed to send the initial player and star messages should be.
     */
     //tcp::socket & clientSocket;

    Client()
    {
    }

    /*
     * This constructor is to be used in joinMessageHandler.
     */
     /*
     Client(tcp::socket & clientSocket)
     {
         this->clientSocket = clientSocket;
     }
     */

    double getX()
    {
        return positionX;
    }

    double getY()
    {
        return positionY;
    }

    int getHealth()
    {
        return health;
    }

    /*
     * Return the stringified payload for player_message.
     */
    string getPayloadString() {
        return "," + to_string(positionX) + "," + to_string(positionY)
            + "," + to_string(health);
    }

    /*
     * Return maze updates for this Client.  Clear unsent maze update
     * list.
     */
    string getMazeUpdates() {
        string mazeUpdates = unsentMazeUpdates;
        unsentMazeUpdates = "";
        return mazeUpdates;
    }

    void setKeyDirection(vector<string> keyInputVector)
    {
        keyDirection = 0;
    }

    void setIsCrouching(bool isCrouching)
    {
        this->isCrouching = isCrouching;
    }

    void setIsSprinting(bool isSprinting)
    {
        this->isSprinting = isSprinting;
    }

    void setPlayerDirection(double playerDirection)
    {
        this->playerDirection = playerDirection;
    }

    void setIsFiring(bool isFiring) {
        this->isFiring = isFiring;
    }

    void setX(double positionX)
    {
        this->positionX = positionX;
    }

    void setY(double positionY)
    {
        this->positionY = positionY;
    }

    void setHealth(int health)
    {
        this->health = health;
    }

    void addMazeUpdate(string mazeUpdate) {
        unsentMazeUpdates += mazeUpdate;
    }
};




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