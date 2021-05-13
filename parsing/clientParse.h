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

#pragma once

const string JOIN_RESPONSE = "joinResponse,0";
const string START_MESSAGE = "start";
const string PLAYER_MESSAGE_INITIAL = "player,1,0.0,0.0,100";
const string PLAYER_MESSAGE_UPDATE = "player,1,555.5,444.4,33";
const string MAZE_INITIAL = "mazeInitial,4,1,1,2,0,1,0,0,1,2,1,0,1,2,1,1,0";
const string MAZE_UPDATE = "mazeUpdate,0,0,2,1,1,2";

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