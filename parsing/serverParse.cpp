#include "serverParse.h"

using namespace std;
using namespace boost::asio;
using ip::tcp;


int serverParse::userIdCount = 0;
const string serverParse::MESSAGE_TAIL = "\r\n";


string serverParse::createMazeString(Maze* maze)
{
    string message = "";
    wallInfo** mazeArray = maze->getMazeArray();
    for (int r = 0; r < maze->getMazeSize(); r++)
    {
        for (int c = 0; c < maze->getMazeSize(); c++)
        {
            if (mazeArray[r][c].right)
            {
                message += "mazeUpdate," + to_string(r) + "," + to_string(c) + ",0,";
            }
            if (mazeArray[r][c].bottom)
            {
                message += "mazeUpdate," + to_string(r) + "," + to_string(c) + ",1,";
            }
        }
    }
    return message;
}

string serverParse::createAbilityString(Maze* maze)
{
    string message = "";
    wallInfo** mazeArray = maze->getMazeArray();
    for (int r = 0; r < maze->getMazeSize(); r++)
    {
        for (int c = 0; c < maze->getMazeSize(); c++)
        {
            if (mazeArray[r][c].ability != Player::none)
            {
                message += "mazeAbility," + to_string(r) + "," + to_string(c) + "," + to_string(mazeArray[r][c].ability) + ",";
            }

        }
    }
    return message;
}


/*
 * Update the 2D-Array representation of the array.  Once abilities are added,
 * inputMessageHandler will call this method to update the map.
 * 
 * Add update message
 */
void serverParse::updateMaze(int row, int col, int wallState) {
    //mazeArr[row][col] = wallState;
    //string mazeUpdate = "," + to_string(row) + "," + to_string(col) + "," + to_string(wallState);
    //for (auto it = idClientMap.begin(); it != idClientMap.end(); ++it) {
    //    it->second.addMazeUpdate(mazeUpdate);
    //}
}

/*
 * Create new entry in idClientMap
 */
void serverParse::joinMessageHandler(){
    //idClientMap[to_string(userIdCount-1)] = Client();
    //userIdCount++;
}

/*
 * Remove entry from idClientMap
 */
void serverParse::leaveMessageHandler(string clientId){
    //idClientMap.erase(clientId);
    //cout << "Removed User Id:" << clientId << endl;
}

/*
 * Store data from input message in the approriate idClientMap entry.
 */
void serverParse::inputMessageHandler(vector<string> messageValues){

    //string clientId;

    ////making sure clientId only contains valid char values
    //for (char c : messageValues.at(1)) {
    //    int val = (int)c;
    //    if (val >= 48 && val <= 57) {
    //        clientId += c;
    //    }
    //}

    //vector<string> keyInputVector = { messageValues[2], messageValues[3], messageValues[4], messageValues[5], messageValues[6], messageValues[7] };
    //idClientMap[clientId].setKeyDirection(keyInputVector);
    //idClientMap[clientId].setIsCrouching(messageValues.at(6)=="true");
    //idClientMap[clientId].setIsSprinting(messageValues.at(7)=="true");
    //idClientMap[clientId].setPlayerDirection(stod(messageValues.at(8)));
    //idClientMap[clientId].setIsFiring(messageValues.at(9)=="true");
}

/*
 * Sort messages based off header and store payload information where
 * relevant.  Assume the MESSAGE_TAIL has already been removed by a
 * read_until method.
 */
void serverParse::sortClientMessage(string clientMessage) {
    vector<string> messageValues;
    boost::split(messageValues, clientMessage, boost::is_any_of(","));
    string header = messageValues.front();
    if (header=="join") {
        serverParse::joinMessageHandler();
    }
    else if (header=="leave") {
        serverParse::leaveMessageHandler(messageValues.at(1));
    }
    else if (header=="input"){
        serverParse::inputMessageHandler(messageValues);
    }
    else {
        cout << clientMessage << endl;
        cout<<"Unexpected message type from client"<<endl;
    }
}

/*
 * Return the join response string.
 */
string serverParse::buildJoinResponse(string clientId) {
    return "joinResponse," + clientId + serverParse::MESSAGE_TAIL;
}

/*
 * Return the player message string.
 */
string serverParse::buildPlayerMessage(string clientId) {

    //return "player," + clientId + idClientMap[clientId].getPayloadString() 
    //    + MESSAGE_TAIL;
    return "";
}

/*
 * Return the build message string.
 */
string serverParse::buildStartMessage() {
    //return "start" + MESSAGE_TAIL;
}

/*
 * Return the initial maze message sent before the game.
 */
string serverParse::buildMazeInitialMessage() {
    //string message = "mazeInitial," + to_string(MAZE_SIZE);
    //for (int i = 0; i < MAZE_SIZE; i++) {
    //    for (int j = 0; j < MAZE_SIZE; j++) {
    //        message += "," + to_string(mazeArr[i][j]);
    //    }
    //}
    //return message + MESSAGE_TAIL;
    return "";
}

/*
 * Return the update maze message sent during the game after updates to the
 * maze for the passed client.
 */
string serverParse::buildMazeUpdateMessage(string clientId) {
    //return "mazeUpdate" + idClientMap[clientId].getMazeUpdates() + MESSAGE_TAIL;
    return "";
}

/*
 * Method to display 2D array in string form.  Only for verifying methods.
 */
void serverParse::printArray() {

    //for (int i = 0; i < MAZE_SIZE; i++) {
    //    string row = "";
    //    for (int j = 0; j < MAZE_SIZE; j++) {
    //        row += to_string(mazeArr[i][j]) + " ";
    //    }
    //    cout << row << endl;
    //}
}


// int main(int argc, char* argv[]) {

//     // Client with userId "0" will be created and deleted
//     sortClientMessage(JOIN_MESSAGE);
//     sortClientMessage(LEAVE_MESSAGE);

//     // Client with userId "1" will be created and input stored
//     sortClientMessage(JOIN_MESSAGE);
//     sortClientMessage(INPUT_MESSAGE);
//     cout << "ClientId 1's Player Direction: ";
//     cout << idClientMap["1"].playerDirection << endl;

//     // Set maze, update maze
//     int exampleArr[4][4]  = {{0,1,2,0},{1,2,0,1},{2,1,0,1},{2,1,1,0}};
//     memcpy(mazeArr,exampleArr,sizeof(mazeArr));
//     printArray();
//     updateMaze(0,0,1);
//     cout << "Client 1 maze update 1:" << idClientMap["1"].unsentMazeUpdates << endl;
//     updateMaze(1,1,0);
//     cout << "Client 1 maze update 2:" << idClientMap["1"].unsentMazeUpdates << endl;
//     printArray();

//     // Print messages sent by server.
//     cout << "Join Response:" << buildJoinResponse("1") << endl;
//     cout << "Player Message:" << buildPlayerMessage("1") << endl;
//     cout << "Start Message:" << buildStartMessage() << endl;
//     cout << "Maze Initial:" << buildMazeInitialMessage() << endl;
//     cout << "Maze Update (Client 1):" << buildMazeUpdateMessage("1") << endl;
//     cout << "Client 1's empty maze updates:" << idClientMap["1"].unsentMazeUpdates << endl;

//     return EXIT_SUCCESS;
// }