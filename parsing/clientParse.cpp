#include <iostream>
#include <cstdlib>
#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>
#include <unordered_map>

using namespace std;
using namespace boost::asio;
using ip::tcp;

/*
 * Example messages for client parsing.  It assumes the MESSAGE_TAIL has already
 * been removed by a read_until method.
 */
const string JOIN_RESPONSE = "joinResponse,0";
const string START_MESSAGE = "start";
const string PLAYER_MESSAGE_INITIAL = "player,1,432.1,-123.4,65";
const string PLAYER_MESSAGE_UPDATE = "player,1,555.5,444.4,33";


/*
 * Store information about each player in the game.
 */
class Player {
    public:
        double positionX;
        double positionY;
        int health;

        Player() {

        }

        Player(double positionX, double positionY, int health) {
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

        void setX(double positionX) {
            this->positionX = positionX;
        }

        void setY(double positionY) {
            this->positionY = positionY;
        }

        void setHealth(int health) {
            this->health = health;
        }
};

string selfId;  // This player's unique userId.
unordered_map<string,Player> idPlayerMap;  // Map unique userId's with each player
bool hasGameStarted = false;
const string MESSAGE_TAIL = "\r\n";  //Attatched to the end of every message.

/*
 * Set selfId field to userId sent in Join Response message.
 */
void joinResponseHandler(string userId) {
    selfId = userId;
}

/*
 * Record information about the player in Player Message in idPlayerMap.
 */
void playerMessageHandler(vector<string> messageValues) {        

    string userId = messageValues.at(1);
    if (hasGameStarted) {
        idPlayerMap[userId].setX(stod(messageValues.at(2)));
        idPlayerMap[userId].setY(stod(messageValues.at(3)));
        idPlayerMap[userId].setHealth(stoi(messageValues.at(4)));
    }
    else {
        // Creation of entry in idPlayerMap.
        idPlayerMap[userId] = Player(stod(messageValues.at(2)),stod(messageValues.at(3)), stoi(messageValues.at(4)));
    }
}

/*
 * To be created method to handle storing the maze and updated version of it.
 */
void mazeMessageHandler(vector<string> messageValues) {

}

/*
 * Record the beginning of the game.  This is also where the client should start 
 * sending messages to the server every PERIOD.
 */
void startMessageHandler() {
    hasGameStarted = true;
}

/*
 * Sort messages based off header and store payload information where
 * relevant.  Assume the MESSAGE_TAIL has already been removed by a
 * read_until method.
 */
void sortServerMessage(string serverMessage) {
    vector<string> messageValues;
    boost::split(messageValues, serverMessage, boost::is_any_of(","));
    string header = messageValues.front();
    if (header=="joinResponse") {
        joinResponseHandler(messageValues.at(1));
    }
    else if (header=="player") {
        playerMessageHandler(messageValues);
    }
    else if (header=="maze"){
        mazeMessageHandler(messageValues);
    }
    else if (header=="start") {
        startMessageHandler();
    }
    else {
        cout<<"Unexpected message type from server"<<endl;
    }
}

/*
 * Return the join message string.
 */
string buildJoinMessage() {
    return "join" + MESSAGE_TAIL;
}

/*
 * Return the leave message string.
 */
string buildLeaveMessage() {
    return "leave," + selfId + MESSAGE_TAIL;
}

/*
 * Return the input message string.
 */
string buildInputMessage() {
    return "player," + selfId + ",true,false,false,false,true,false,300.58,false" 
        + MESSAGE_TAIL;
}

int main(int argc, char* argv[]) {

    // Client assigned userId of "0".
    sortServerMessage(JOIN_RESPONSE);
    cout << "userId: " << selfId << endl;

    // Records position and health of player with userId "1".
    sortServerMessage(PLAYER_MESSAGE_INITIAL);
    cout << "X: " << idPlayerMap["1"].getX() << endl;
    cout << "Y: " << idPlayerMap["1"].getY() << endl;
    cout << "Health: " << idPlayerMap["1"].getHealth() << endl;

    // Updates position and health of player with userId "1".
    sortServerMessage(PLAYER_MESSAGE_UPDATE);
    cout << "X: " << idPlayerMap["1"].getX() << endl;
    cout << "Y: " << idPlayerMap["1"].getY() << endl;
    cout << "Health: " << idPlayerMap["1"].getHealth() << endl;

    // Ensures game start event has been recorded.
    sortServerMessage(START_MESSAGE);
    if (hasGameStarted == true) {
        cout << "Game started" << endl;
    }

    // Print messages sent by client.
    cout << "Join Message:" << buildJoinMessage() << endl;
    cout << "Leave Message:" << buildLeaveMessage() << endl;
    cout << "Input Message:" << buildInputMessage() << endl;

    return EXIT_SUCCESS;
}