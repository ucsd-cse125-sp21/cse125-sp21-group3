#pragma once

#ifndef CLIENTPARSE_H
#define CLIENTPARSE_H


#include <iostream>
#include <cstdlib>
#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>
#include <unordered_map>
#include <queue>
#include "../Game.h"
#include "../Window.h"
#include "../Player.h"

using namespace std;
using namespace boost::asio;
using ip::tcp;


const int MAX_FOOTSTEPS = 10;


class clientParse
{
    public:
        /*
     * Example messages for client parsing.  It assumes the MESSAGE_TAIL has already
     * been removed by a read_until method.
     */

        /*
         * Store information about each player in the game.
         */
    

        string selfId;  // This player's unique userId.
        //unordered_map<string, PlayerClient> idPlayerMap;  // Map unique userId's with each player
        bool hasGameStarted = false;
        const string MESSAGE_TAIL = "\r\n";  //Attatched to the end of every message.

        /*
         * Mazes are represented by a 2D-Array.  A hidden wall is represented by 0.  A
         * east-west wall is represented by a 1.  A north-south wall is represented by
         * a 2.
         */
        const int MAZE_UPDATE_SIZE = 3;
        int mazeSize; // Height and width of maze.
        int** mazeArr; // 2D representation of maze.

        static void joinResponseHandler(string userId);

        static void playerMessageHandler(vector<string> messageValues);

        static void mazeInitialMessageHandler(vector<string> messageValues);

        static void mazeUpdateMessageHandler(vector<string> messageValues);

        static void startMessageHandler();

        static void sortServerMessage(Game* game, string serverMessage);

        static string buildJoinMessage();

        static string buildLeaveMessage();

        static string buildInputMessage(Game* game);

        void printArray();

        void printFootsteps(string playerId);

};


#endif