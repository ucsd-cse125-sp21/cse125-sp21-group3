#include "serverParse.h"


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
                message += "mU," + to_string(r) + "," + to_string(c) + ",0,";
            }
            if (mazeArray[r][c].bottom)
            {
                message += "mU," + to_string(r) + "," + to_string(c) + ",1,";
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
                message += "mA," + to_string(r) + "," + to_string(c) + "," + to_string(mazeArray[r][c].ability) + ",";
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
void serverParse::inputMessageHandler(Game* game, vector<string> messageValues){

    //string clientId;

    ////making sure clientId only contains valid char values
    //for (char c : messageValues.at(1)) {
    //    int val = (int)c;
    //    if (val >= 48 && val <= 57) {
    //        clientId += c;
    //    }
    //}
    int clientId = stoi(messageValues.at(1));
    if (clientId >= game->allPlayers.size()) {
        cout << "invalid clientId in inputMessageHandler: " << clientId << endl;
        return;
    }
    Player* player = NULL;
    for (int i = 0; i < game->allPlayers.size(); i++) {
        if (clientId == game->allPlayers.at(i)->getId()) {
            player = game->allPlayers.at(i);
            break;
        }
    }
    if (player == NULL) {
        cout << "invalid player in inputMessageHandler: " << clientId << endl;
        return;
    }

    
    player->setMoving(stoi(messageValues.at(2)));
    //player->setLookingDirection(glm::vec3(stof(messageValues.at(3)), stof(messageValues.at(4)), stof(messageValues.at(5))));
    string hasFired;
    for (int i = 0; i < messageValues.at(6).size(); i++) {
        int c = (int)messageValues.at(6).at(i);
        if (c >= (int)'a' && c <= (int)'z') {
            hasFired += messageValues.at(6).at(i);
        }
    }
    if (hasFired.compare("true") == 0) {
        cout << "received hasFired to true for player " << clientId << endl;
        player->setHasFired(true);
    }
 
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
void serverParse::sortClientMessage(Game* game, string clientMessage) {
    
    vector<string> messageValues;
    boost::split(messageValues, clientMessage, boost::is_any_of(","));
    vector<string>::iterator it = messageValues.begin();
    while (it != messageValues.end())
    {
        if (*it == "join") {
        }
        else if (*it == "leave") {
        }
        else if (*it == "input") {
            int playerID = stoi(*(it + 1));
            int isMoving = stoi(*(it + 2));
            float yaw = stof(*(it + 3));
            float pitch = stof(*(it + 4));
            int state = stoi(*(it + 5));
            Player* player = game->allPlayers.at(0);
            for (int i = 1; i < game->allPlayers.size(); i++)
            {
                if (game->allPlayers.at(i)->getId() == playerID)
                {
                    player = game->allPlayers.at(i);
                    break;
                }
            }

            Camera* cam = player->getPlayerCamera();
            cam->setYaw(yaw);
            cam->setPitch(pitch);
            cam->Update();
            player->setVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
            player->setState(state);
            player->setMoving(isMoving);

            for (int i = Player::forward; i <= Player::down; i++)
            {
                int dirOn = 0;
                dirOn = stoi(*(it + 6 + i));
                if (dirOn)
                {
                    player->moveDirection(i);
                }
            }
            int hasFired = stoi(*(it + 6 + Player::down + 1));
            if (hasFired)
            {
                player->shootWeapon(game -> allBoundingBoxes, true);
            }
            player->setHasFired(hasFired);
           
            int pickUpAbility = stoi(*(it + 6 + Player::down + 2));
            if (pickUpAbility)
            {
                player->pickUpAbility();
                player->setPickUpAbilityKey(false);
            }
            int useAbility = stoi(*(it + 6 + Player::down + 3));
            if (useAbility)
            {
                player -> useAbility();
                player->setUseAbilityKey(false);
            }
            float animationRootModelRotation = stof(*(it + 6 + Player::down + 4));
            player->getPlayerModel()->animationRootModelRotation = animationRootModelRotation;
            it = it + 6 + Player::down + 4;
        }
        else {
        }
        it++;
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
string serverParse::buildPlayerMessage(Game* game, string clientId) {

    string playerMessage = "";
    if (game == NULL) {
        return playerMessage;
    }
    if (stoi(clientId) >= game->allPlayers.size()) {
        cout << "invalid clientId in buildPlayerMessage: " << clientId << endl;
        return playerMessage;
    }

    for (int i = 0; i < game->allPlayers.size(); i++) {
        if (game->allPlayers.at(i)->getId() == stoi(clientId)) {
            playerMessage = game->allPlayers.at(i)->getPlayerInfoString();
            //cout << "sending playerMessage: " << playerMessage << endl;
            game->allPlayers.at(i)->setHasFired(false);
            break;
        }
    }

    return playerMessage;
    //return "";
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
string serverParse::buildMazeUpdateMessage(Game* game) {
    Maze* maze = game->maze;
    string message = "";
    wallInfo** mazeArray = maze->getMazeArray();
    for (int r = 0; r < maze->getMazeSize(); r++)
    {
        for (int c = 0; c < maze->getMazeSize(); c++)
        {
            if (mazeArray[r][c].right)
            {
                message += "mazeUpdate," + to_string(r) + "," + to_string(c) + ",0,";
                message += to_string(mazeArray[r][c].ability) + ",";                
            }
            if (mazeArray[r][c].bottom)
            {
                message += "mazeUpdate," + to_string(r) + "," + to_string(c) + ",1,";
                message += to_string(mazeArray[r][c].ability) + ",";
            }

            //message += to_string(0) + ",";
            //cout << "message: " << message << endl;
        }
    }
    return message;
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