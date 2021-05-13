#include "clientParse.h"
#include "../Window.h"

/*
 * Set selfId field to userId sent in Join Response message.
 */
void clientParse::joinResponseHandler(string userId) {
    //selfId = userId;
}

/*
 * Record information about the player in Player Message in idPlayerMap.
 */
void clientParse::playerMessageHandler(vector<string> messageValues) {
    
    //0th message values is just player
    /*cout << "player: " << messageValues.at(0) << endl;
    cout << "userId: " << messageValues.at(1) << endl;
    cout << "positionX: " << messageValues.at(2) << endl;
    cout << "positionY: " << messageValues.at(3) << endl;
    cout << "health: " << messageValues.at(4) << endl;
    cout << "moving: " << messageValues.at(5) << endl;*/
    
    //string userId = messageValues.at(1);
    //if (hasGameStarted) {
    //    idPlayerMap[userId].setX(stod(messageValues.at(2)));
    //    idPlayerMap[userId].setY(stod(messageValues.at(3)));
    //    idPlayerMap[userId].setHealth(stoi(messageValues.at(4)));
    //}
    //else {
    //    // Creation of entry in idPlayerMap.
    //    idPlayerMap[userId] = PlayerClient(stod(messageValues.at(2)),stod(messageValues.at(3)), stoi(messageValues.at(4)));
    //}
    //
    //int clientId = stoi(selfId);
    //int user = stoi(userId);
    //if (user != clientId) { //if we are receiving a message about an opponent
    //    int moving = stoi(messageValues.at(5));
    //    //TODO update with position and direction instead of hardcoded values
    //    Window::updateOpponent(user, glm::vec3(3.0f, 3.5f, 3.0f), glm::vec3(0.0f, 3.5, 0.0f - 3.0f), moving);
    //}
}

/*
 * Store the 2D Array representation of the maze.
 */
void clientParse::mazeInitialMessageHandler(vector<string> messageValues) {

    ////Initialize maze 2D-array
    //mazeSize = stoi(messageValues.at(1));
    //mazeArr = new int*[mazeSize];
    //for (int i = 0; i < mazeSize; ++i) {
    //    mazeArr[i] = new int[mazeSize];
    //}

    //cout << "Maze initialized" << endl;

    ////Populate 2D-Array
    //int valuesInd = 2;
    //for (int row = 0; row < mazeSize; row++) {
    //    for (int col = 0; col < mazeSize; col++) {
    //        mazeArr[row][col] = stoi(messageValues.at(valuesInd));
    //        valuesInd++;
    //    }
    //}
}

/*
 * Update the 2D Array representation of the maze.  There may be multiple
 * maze updates in a single message.
 */
void clientParse::mazeUpdateMessageHandler(vector<string> messageValues) {
    //for (int i = 1; i < messageValues.size(); i+=MAZE_UPDATE_SIZE) {
    //    int row = stoi(messageValues.at(i));
    //    int col = stoi(messageValues.at(i+1));
    //    int wallState = stoi(messageValues.at(i+2));
    //    mazeArr[row][col] = wallState;
    //}
}

/*
 * Record the beginning of the game.  This is also where the client should start 
 * sending messages to the server every PERIOD.
 */
void clientParse::startMessageHandler() {
    //hasGameStarted = true;
}

/*
 * Sort messages based off header and store payload information where
 * relevant.  Assume the MESSAGE_TAIL has already been removed by a
 * read_until method.
 */
void clientParse::sortServerMessage(Game* game, string serverMessage) {
    vector<string> messageValues;
    boost::split(messageValues, serverMessage, boost::is_any_of(","));
    vector<string>::iterator it = messageValues.begin();
    while (it != messageValues.end())
    {
        if (*it == "joinResponse")
        {
            cout << "joinResponse assigning id: " << messageValues.at(1) << endl;
            game->myPlayerId = stoi(messageValues.at(1));
        }
        else if (*it == "player")
        {
            if (game == NULL || game->maze == NULL) {
                return;
            }
            int userId = stoi(messageValues.at(1));
            bool createPlayer = true;
            if (userId == game->myPlayerId) {
                createPlayer = false;
            }
            else {
                for (int i = 0; i < game->allPlayers.size(); i++) {
                    int id = game->allPlayers.at(i)->getId();
                    if (userId == id) {
                        createPlayer = false;
                        break;
                    }
                }
            }            
            if (createPlayer) {
                Window::createOpponent = userId;
            }
            int x = stoi(*(it + 2));
            int y = stoi(*(it + 3));
            int health = stoi(*(it + 4));
            it = (it + 4);
        }
        else if (*it == "mazeInitial")
        {
        }
        else if (*it == "mazeUpdate")
        {
            int row = stoi(*(it + 1));
            int col = stoi(*(it + 2));
            int wallDirection = stoi(*(it + 3));
            game->maze->setWall(row, col, wallDirection, 1);
            it = (it + 3);
        }
        else if (*it == "start")
        {
        }
        it++;
    }
    //string header = messageValues.front();
    //if (header=="joinResponse") {
    //    joinResponseHandler(messageValues.at(1));
    //}
    //else if (header=="player") {
    //    playerMessageHandler(messageValues);
    //}
    //else if (header=="mazeInitial"){
    //    mazeInitialMessageHandler(messageValues);
    //}
    //else if (header=="mazeUpdate") {
    //    mazeUpdateMessageHandler(messageValues);
    //}
    //else if (header=="start") {
    //    startMessageHandler();
    //}
    //else {
    //    cout<<"Unexpected message type from server"<<endl;
    //}
}

/*
 * Return the join message string.
 */
string clientParse::buildJoinMessage() {
    //return "join" + MESSAGE_TAIL;
    return "";
}

/*
 * Return the leave message string.
 */
string clientParse::buildLeaveMessage() {
    //return "leave," + selfId + MESSAGE_TAIL;
    return "";
}

/*
 * Return the input message string.  String inserted here is a sample.  Input
 * parameters will need to be added.
 */
string clientParse::buildInputMessage(Game* game) {
    
    //string inputMessage = "input," + selfId + "," + Window::playerInputString
    //    + MESSAGE_TAIL;
    /*string defaultMessage = "input," + selfId + ",true,false,false,false,true,false,300.58,false"
        + MESSAGE_TAIL;
    cout << "theirString: " << defaultMessage;*/
    string inputMessage = "";
    if(game->myPlayer) {
        inputMessage = game->myPlayer->getPlayerInputString();
    }
    
    //cout << "inputMessage: " << inputMessage << endl;
    return inputMessage;// inputMessage;
}

/*
 * Method to display 2D array in string form.  Only for verifying methods.
 */
void clientParse::printArray() {

    //for (int i = 0; i < mazeSize; i++) {
    //    string row = "";
    //    for (int j = 0; j < mazeSize; j++) {
    //        row += to_string(mazeArr[i][j]) + " ";
    //    }
    //    cout << row << endl;
    //}
}

/*
 * Method to display footsteps in string form.  Only for verifying methods.
 */
void clientParse::printFootsteps(string playerId) {
    //deque<PlayerClient::Footstep> footsteps = idPlayerMap[playerId].getFootsteps();
    //string footstepString = "";
    //for (int i = 0; i < footsteps.size(); i++) {
    //    footstepString += "(" + to_string(footsteps[i].x) + "," 
    //        + to_string(footsteps[i].y) + ")";
    //}
    //cout << "Footsteps: " << footstepString << endl;
}

// int main(int argc, char* argv[]) {

//     // Client assigned userId of "0".
//     sortServerMessage(JOIN_RESPONSE);    
//     cout << "userId: " << selfId << endl;

//     // Records position and health of player with userId "1".
//     sortServerMessage(PLAYER_MESSAGE_INITIAL);
//     cout << "X: " << idPlayerMap["1"].getX() << endl;
//     cout << "Y: " << idPlayerMap["1"].getY() << endl;
//     cout << "Health: " << idPlayerMap["1"].getHealth() << endl;
//     printFootsteps("1");

//     // Store initial maze
//     sortServerMessage(MAZE_INITIAL);
//     printArray();

//     // Ensures game start event has been recorded.
//     sortServerMessage(START_MESSAGE);
//     if (hasGameStarted == true) {
//         cout << "Game started" << endl;
//     }

//     // Update position and health of player with userId "1".
//     sortServerMessage(PLAYER_MESSAGE_UPDATE);
//     cout << "X: " << idPlayerMap["1"].getX() << endl;
//     cout << "Y: " << idPlayerMap["1"].getY() << endl;
//     cout << "Health: " << idPlayerMap["1"].getHealth() << endl;

//     // Add footsteps and check
//    idPlayerMap["1"].addFootstep(1.0,1.0);
//     printFootsteps("1");

//     for (double i = 2.0; i < 12.0; i++) {
//         idPlayerMap["1"].addFootstep(i,i);
//     }
//     printFootsteps("1");

//     // Store maze update message.
//     sortServerMessage(MAZE_UPDATE);
//     printArray();

//     // Print messages sent by client.
//     cout << "Join Message:" << buildJoinMessage() << endl;
//     cout << "Leave Message:" << buildLeaveMessage() << endl;
//     cout << "Input Message:" << buildInputMessage() << endl;

//     return EXIT_SUCCESS;
//}