#include <iostream>
#include <cstdlib>
#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>
#include <unordered_map>
#include <queue>

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

class PlayerClient {

    public:

        class Footstep {
        public:

            double x;
            double y;

            Footstep() {

            }

            Footstep(double x, double y) {
                this->x = x;
                this->y = y;
            }
        };

        double positionX;
        double positionY;
        int health;

        /*
         * Stores all footsteps player has taken.  Most recent footsteps stored
         * first.  Only the first certain number of foosteps should be rendered.
         */
        deque<Footstep> footstepQueue;

        PlayerClient() {

        }

        PlayerClient(double positionX, double positionY, int health) {
            this->positionX = positionX;
            this->positionY = positionY;
            this->health = health;
        }

        double getX() {
            return positionX;
        }

        double getY() {
            return positionY;
        }

        int getHealth() {
            return health;
        }

        deque<Footstep> getFootsteps() {
            return footstepQueue;
        }

        void setX(double positionX) {
            this->positionX = positionX;
        }

        void setY(double positionY) {
            this->positionY = positionY;
        }

        void setHealth(int health) {
            this->health = health;
        }

        /*
         * Add a footstep to the footstep queue.  Remove the oldest footstep
         * if there are more than MAX_FOOTSTEPS footsteps stored.  The queue stores
         * footsteps youngest to oldest from front to back.
         */
        void addFootstep(double x, double y) {
            footstepQueue.push_front(Footstep(x, y));
            if (footstepQueue.size() > MAX_FOOTSTEPS) {
                footstepQueue.pop_back();
            }
        }
 };

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
        unordered_map<string, PlayerClient> idPlayerMap;  // Map unique userId's with each player
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

        void joinResponseHandler(string userId);

        void playerMessageHandler(vector<string> messageValues);

        void mazeInitialMessageHandler(vector<string> messageValues);

        void mazeUpdateMessageHandler(vector<string> messageValues);

        void startMessageHandler();

        void sortServerMessage(string serverMessage);

        string buildJoinMessage();

        string buildLeaveMessage();

        string buildInputMessage();

        void printArray();

        void printFootsteps(string playerId);

};