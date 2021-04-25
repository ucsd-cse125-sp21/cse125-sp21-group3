#include <iostream>
#include <cstdlib>
#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>
#include <unordered_map>
#include <queue>

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


const int MAX_FOOTSTEPS = 10;
/*
 * Store information about the client including the most recent input message
 * data.
 */
class Client{

    class Footstep{

        public:

            double x;
            double y;
            
            Footstep() {

            }

            Footstep(double x,double y) {
                this->x = x;
                this->y = y;
            }

            string toString() {
                return "," + to_string(x) + "," + to_string(y);
            }
    };

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
        deque<Footstep> footstepQueue; //Youngest to oldest from front to back.

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
            string footsteps = "";
            for (int i = 0; i < footstepQueue.size(); i++) {
                footsteps += footstepQueue[i].toString();
            }
            return "," + to_string(positionX) + "," + to_string(positionY) 
                + "," + to_string(health) + footsteps;
        }

        /*
         * Return maze updates for this Client.  Clear unsent maze update
         * list.
         */
        string getMazeUpdates(){
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

        void addMazeUpdate(string mazeUpdate){
            unsentMazeUpdates += mazeUpdate;
        }

        /*
         * Add a footstep to the footstep queue.  Remove the oldest footstep
         * if there are more than 10 footsteps stored.  The queue stores
         * footsteps youngest to oldest from front to back.
         */
        void addFootstep(double x, double y) {
            footstepQueue.push_front(Footstep(x,y));
            if (footstepQueue.size() > MAX_FOOTSTEPS) {
                footstepQueue.pop_back();
            }
        }
};

/*
 * Field variables.
 */
const string MESSAGE_TAIL = "\r\n"; // Attatched to the end of every message.
unordered_map<string,Client> idClientMap;  // Map unique userId's with each client.
int userIdCount = 0; // Incrementing counter that assigns new userId's


/*
 * Mazes are represented by a 2D-Array.  A hidden wall is represented by 0.  A
 * east-west wall is represented by a 1.  A north-south wall is represented by
 * a 2.
 */
const int MAZE_SIZE = 4; // Height and width of maze.
int mazeArr[MAZE_SIZE][MAZE_SIZE];

/*
 * Update the 2D-Array representation of the array.  Once abilities are added,
 * inputMessageHandler will call this method to update the map.
 * 
 * Add update message
 */
void updateMaze(int row, int col, int wallState) {
    mazeArr[row][col] = wallState;
    string mazeUpdate = "," + to_string(row) + "," + to_string(col) + "," + to_string(wallState);
    for (auto it = idClientMap.begin(); it != idClientMap.end(); ++it) {
        it->second.addMazeUpdate(mazeUpdate);
    }
}

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
    return "player," + clientId + idClientMap[clientId].getPayloadString() 
        + MESSAGE_TAIL;
}

/*
 * Return the build message string.
 */
string buildStartMessage() {
    return "start" + MESSAGE_TAIL;
}

/*
 * Return the initial maze message sent before the game.
 */
string buildMazeInitialMessage() {
    string message = "mazeInitial," + to_string(MAZE_SIZE);
    for (int i = 0; i < MAZE_SIZE; i++) {
        for (int j = 0; j < MAZE_SIZE; j++) {
            message += "," + to_string(mazeArr[i][j]);
        }
    }
    return message + MESSAGE_TAIL;
}

/*
 * Return the update maze message sent during the game after updates to the
 * maze for the passed client.
 */
string buildMazeUpdateMessage(string clientId) {
    return "mazeUpdate" + idClientMap[clientId].getMazeUpdates() + MESSAGE_TAIL;
}

/*
 * Method to display 2D array in string form.  Only for verifying methods.
 */
void printArray() {

    for (int i = 0; i < MAZE_SIZE; i++) {
        string row = "";
        for (int j = 0; j < MAZE_SIZE; j++) {
            row += to_string(mazeArr[i][j]) + " ";
        }
        cout << row << endl;
    }
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

    // Set maze, update maze
    int exampleArr[4][4]  = {{0,1,2,0},{1,2,0,1},{2,1,0,1},{2,1,1,0}};
    memcpy(mazeArr,exampleArr,sizeof(mazeArr));
    printArray();
    updateMaze(0,0,1);
    cout << "Client 1 maze update 1:" << idClientMap["1"].unsentMazeUpdates << endl;
    updateMaze(1,1,0);
    cout << "Client 1 maze update 2:" << idClientMap["1"].unsentMazeUpdates << endl;
    printArray();

    // Add 11 footsteps to Client with userId "1".
    for (double i = 0; i < 11; i++) {
        idClientMap["1"].addFootstep(i,i+1);
    }
    cout << "Num footsteps: " << idClientMap["1"].footstepQueue.size() << endl;

    // Print messages sent by server.
    cout << "Join Response:" << buildJoinResponse("1") << endl;
    cout << "Player Message:" << buildPlayerMessage("1") << endl;
    cout << "Start Message:" << buildStartMessage() << endl;
    cout << "Maze Initial:" << buildMazeInitialMessage() << endl;
    cout << "Maze Update (Client 1):" << buildMazeUpdateMessage("1") << endl;
    cout << "Client 1's empty maze updates:" << idClientMap["1"].unsentMazeUpdates << endl;

    return EXIT_SUCCESS;
}