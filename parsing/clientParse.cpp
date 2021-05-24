#include "clientParse.h"

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
    //cerr << "server message contains tail at: " << serverMessage.find("\r\n") << endl;
    //cerr << "received message: " << serverMessage << endl;
    //cerr << "received message size: " << serverMessage.size() << endl;
    bool createdMaze = false;
    while (it != messageValues.end())
    {
        if (*it == "joinResponse")
        {
            int playerId = stoi(*(it + 1));
            game->myPlayerId = playerId;
            it = it + 1;
        }
        else if (*it == "player")
        {
            if (game == NULL || game->maze == NULL) {
                return;
            }
            int userId = stoi(*(it + 1));
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
                Window::createOpponent = userId; //set flag to create new player (opponent)
            }

            Player* player = game -> myPlayer;
            //since allPlayers[i] does not necessarily equal player i, we must search to find correct player
            for (int i = 0; i < game->allPlayers.size(); i++)
            {
                if (userId == game->allPlayers.at(i)->getId())
                {
                    Player* player = game->allPlayers.at(i);
                    break;
                    //if (userId != game->myPlayerId) {
                    //    game->allPlayers.at(i)->setMoving(stoi(messageValues.at(2)));
                    //    string hasFired;
                    //    for (int j = 0; j < messageValues.at(6).size(); j++) {
                    //        int c = (int)messageValues.at(6).at(j);
                    //        if (c >= (int)'a' && c <= (int)'z') {
                    //            hasFired += messageValues.at(6).at(j);
                    //        }
                    //    }
                    //    if (hasFired.compare("true") == 0) {
                    //        //cout << "set isFiring for opponent" << endl;
                    //        game->allPlayers.at(i)->setIsFiring(true);
                    //    }
                }
            }


            int isMoving = stoi(*(it + 2));
            
            float position_x = stof(*(it + 3));
            float position_y = stof(*(it + 4));
            float position_z = stof(*(it + 5));

            float velocity_x = stof(*(it + 6));
            float velocity_y = stof(*(it + 7));
            float velocity_z = stof(*(it + 8));

            if (player != NULL)
            {
                //player->setMoving(isMoving);
                player->setPosition(glm::vec3(position_x, position_y, position_z));
            }

            float currentHealth = stof(*(it + 9));
            float maxHealth = stof(*(it + 10));
            float currentArmor = stof(*(it + 11));
            float currentDamageBoost = stof(*(it + 12));
            int currentAbility = stoi(*(it + 13));

            player->setHealth(currentHealth);
            player->setMaxHealth(maxHealth);
            player->setArmor(currentArmor);
            player->setDamageBoost(currentDamageBoost);
            player->setAbility(currentAbility);
            //playerInfoString += to_string(currentHealth) + "," + to_string(maxHealth) + "," + to_string(currentArmor) + "," + to_string(currentDamageBoost) + "," + to_string(currentAbility);

            it = it + 13;

        }
        else if (*it == "mazeInitial")
        {
        }
        // mazeUpdate
        else if (*it == "mU")
        {
            int row = stoi(*(it + 1));
            int col = stoi(*(it + 2));
            int wallDirection = stoi(*(it + 3));
            //cout << "row: " << row << " col: " << col << " wallDirection: " << wallDirection << endl;
            game->maze->setWall(row, col, wallDirection, 1);
            it = (it + 3);
        }
        else if (*it == "mA")
        {
            createdMaze = true;
            int row = stoi(*(it + 1));
            int col = stoi(*(it + 2));
            int ability = stoi(*(it + 3));
            game->maze->setAbility(row, col, ability);
            it = (it + 3);
        }
        else if (*it == "start")
        {
        }
        else if(*it == "chestOpen") {
            wallInfo** mazeArray = game->maze->getMazeArray();
            int r = stoi(*(it + 1));
            int c = stoi(*(it + 2));
            mazeArray[r][c].ability = Player::opened;
            it = it + 2;
        }
        else if (*it == "deleteWall")
        {
            int r = stoi(*(it + 1));
            int c = stoi(*(it + 2));
            int dir = stoi(*(it + 3));
            game->maze->setWall(r, c, dir, 0);
            it = it + 3;
        }
        else if (*it == "deleteAbility")
        {
            int r = stoi(*(it + 1));
            int c = stoi(*(it + 2));
            cout << "removing" << endl;
            game->maze -> removeAbility(r, c);
            it = it + 2;
        }
        else if (*it == "useSeeMap")
        {
            float oldPitch = stof(*(it + 1));
            float oldYaw = stof(*(it + 2));
            game->myPlayer->setOldPitch(oldPitch);
            game->myPlayer->setOldYaw(oldYaw);
            game->myPlayer->seeMapAbility();
            it = it + 2;
        }
        else if (*it == "endSeeMap")
        {
            cout << "ending see map" << endl;
            game->myPlayer->endMapAbility();
        }
        it++;
    }
    if (createdMaze)
    {
        game->gameSet = true;
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
    
    //cout << "inputMessage: " << inputMessage << endl;
    return game -> getClientInputMessage();
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