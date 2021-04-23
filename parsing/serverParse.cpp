#include <iostream>
#include <cstdlib>
#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>
#include <unordered_map>

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
class Client{
    public:
        double positionX = 0;
        double positionY = 0;
        int health = 100;

        int keyDirection;
        bool isCrouching = false;
        bool isSprinting = false;
        double playerDirection;
        bool isFiring = false;

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
};

const string MESSAGE_TAIL = "\r\n"; // Attatched to the end of every message.
unordered_map<string,Client> idClientMap;  // Map unique userId's with each client.
int userIdCount = 0; // Incrementing counter that assigns new userId's

/*
 * Create new entry in idClientMap
 */
void joinMessageHandler(){
    idClientMap[to_string(userIdCount)] = Client();
    cout << "New User Id:" << userIdCount << endl;
    userIdCount++;
}

/*
 * Remove entry from idClientMap
 */
void leaveMessageHandler(string clientId){
    idClientMap.erase(clientId);
    cout << "Removed User Id:" << clientId << endl;
}

/*
 * Store data from input message in the approriate idClientMap entry.
 */
void inputMessageHandler(vector<string> messageValues){
    string clientId = messageValues.at(1);
    vector<string> keyInputVector(&messageValues[2],&messageValues[6]);
    idClientMap[clientId].setKeyDirection(keyInputVector);
    idClientMap[clientId].setIsCrouching(messageValues.at(6)=="true");
    idClientMap[clientId].setIsSprinting(messageValues.at(7)=="true");
    idClientMap[clientId].setPlayerDirection(stod(messageValues.at(8)));
    idClientMap[clientId].setIsFiring(messageValues.at(9)=="true");
}

/*
 * Sort messages based off header and store payload information where
 * relevant.  Assume the MESSAGE_TAIL has already been removed by a
 * read_until method.
 */
void sortClientMessage(string clientMessage) {
    vector<string> messageValues;
    boost::split(messageValues, clientMessage, boost::is_any_of(","));
    string header = messageValues.front();
    if (header=="join") {
        joinMessageHandler();
    }
    else if (header=="leave") {
        leaveMessageHandler(messageValues.at(1));
    }
    else if (header=="input"){
        inputMessageHandler(messageValues);
    }
    else {
        cout<<"Unexpected message type from client"<<endl;
    }
}

/*
 * Return the join response string.
 */
string buildJoinResponse(string clientId) {
    return "join," + clientId + MESSAGE_TAIL;
}

/*
 * Return the player message string.
 */
string buildPlayerMessage(string clientId) {
    Client client = idClientMap[clientId];
    return "player," + clientId + "," + to_string(client.getX()) + "," 
        + to_string(client.getY()) + "," + to_string(client.getHealth()) 
        + MESSAGE_TAIL;
}

/*
 * Return the build message string.
 */
string buildStartMessage() {
    return "start" + MESSAGE_TAIL;
}

int main(int argc, char* argv[]) {

    // Client with userId "0" will be created and deleted
    sortClientMessage(JOIN_MESSAGE);
    sortClientMessage(LEAVE_MESSAGE);

    // Client with userId "1" will be created and input stored
    sortClientMessage(JOIN_MESSAGE);
    sortClientMessage(INPUT_MESSAGE);
    cout << "ClientId 1's Player Direction: ";
    cout << idClientMap["1"].playerDirection << endl;

    // Print messages sent by server.
    cout << "Join Response:" << buildJoinResponse("1") << endl;
    cout << "Player Message:" << buildPlayerMessage("1") << endl;
    cout << "Start Message:" << buildStartMessage() << endl;

    return EXIT_SUCCESS;
}