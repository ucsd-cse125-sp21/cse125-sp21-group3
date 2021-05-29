#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "util/ts_queue.cpp"
#include "parsing/serverParse.h"
#include "Game.h"
#include "main.h"
#include <glm/gtx/string_cast.hpp>

#define MAX_CONNECTIONS 4
#define PERIOD 25 //server period in ms
#define DELAY_PERIOD 500

using namespace boost::asio;
using ip::tcp;
using std::cout;
using std::endl;


class con_handler : public boost::enable_shared_from_this<con_handler>
{
private:
    tcp::socket sock;
    std::string message = "Hello From Server!";
    enum { max_length = 1024 };
    string buffer;
    boost::asio::dynamic_string_buffer<char, std::char_traits<char>, std::allocator<char>>
        input_buf = boost::asio::dynamic_buffer(buffer);
    ThreadSafeQueue<std::string> input_buffer;

public:
    int pid;
    string pid_str; //for parser
   
    Game* game;

    typedef boost::shared_ptr<con_handler> pointer;
    con_handler(boost::asio::io_service& io_service, int pid, std::string pid_str, Game* gm) : sock(io_service), pid(pid), pid_str(pid_str) {
        game = gm;
    }
    // creating the pointer
    static pointer create(boost::asio::io_service& io_service, int pid, Game* gm)
    {
        return pointer(new con_handler(io_service, pid, to_string(pid), gm));
    }
    //socket creation
    tcp::socket& socket()
    {
        return sock;
    }

    /*
     * Start the async read loop of the server
     */

    void start()
    {
        async_read_until(
            sock,
            input_buf,
            "\r\n",
            boost::bind(&con_handler::handle_read,
                shared_from_this(),
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
                
    }

    /*
     * Read handler which will push incoming messages onto the input queue
     */

    void handle_read(const boost::system::error_code& err, size_t bytes_transferred)
    {
        if (!err) {
            //std::string incoming;
            //incoming.assign(data, bytes_transferred);
            string temp = buffer.substr(0, buffer.find("\r\n"));
            input_buffer.push(temp);
            input_buf.consume(bytes_transferred);

            async_read_until(
            sock,
            input_buf,
            "\r\n",
            boost::bind(&con_handler::handle_read,
                shared_from_this(),
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));

        }
        else {
            std::cerr << "error: " << err.message() << std::endl;
            sock.close();
        }
    }
    
    /*
     * Simple handler to terminate connection upon error
     */ 
    void handle_send_state(const boost::system::error_code& err, size_t bytes_transferred)
    {
        if (err) {
           std::cerr << "error: " << err.message() << endl;
            sock.close();
        }
    }

    /*
     * Join handler which will begin listening for incoming messages from a player
     * once it is confirmed that they have received the join_response
     */ 

    void handle_send_join(const boost::system::error_code& err, size_t bytes_transferred)
    {
        if (!err) {
           serverParse::joinMessageHandler();
           start();
        } else {
            std::cerr << "error: " << err.message() << endl;
            sock.close();
        }

    }

    /*
     * Method to send the state of the game to the player at the other end of this connection
     */

    void send_game_state(std::string state_string)
    {
         sock.async_write_some(
                boost::asio::buffer(state_string, state_string.size()),
                boost::bind(&con_handler::handle_send_state,
                    shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
    }

    /*
     * Method to send join_response to the player at the other end of this connection
     */

    void send_join_message(){
    
        std::string join_msg = serverParse::buildJoinResponse(to_string(serverParse::userIdCount-1));
        sock.async_write_some(
                boost::asio::buffer(join_msg, join_msg.size()),
                boost::bind(&con_handler::handle_send_join,
                    shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
    }

    /*
     * Method which pops the first message from this connections input buffer
     */ 

    std::string dequeue(){
        return input_buffer.pop();
    }

    
};

class Server
{
private:
    boost::asio::io_service & io_service_;
    tcp::acceptor acceptor_;
    con_handler::pointer playerConnections[MAX_CONNECTIONS];

    
    void start_accept()
    {
        // socket
        con_handler::pointer connection = con_handler::create(io_service_, serverParse::userIdCount, game);

        // asynchronous accept operation and wait for a new connection.
        acceptor_.async_accept(connection->socket(),
        boost::bind(&Server::handle_accept, this, connection,
        boost::asio::placeholders::error));
    }
public:
    Game* game;

    //constructor for accepting connection from client
    Server(boost::asio::io_service& io_service, int portNum) : io_service_(io_service),
        acceptor_(io_service_, tcp::endpoint(tcp::v4(), portNum)) {
        start_accept();
        game = new Game(false);
    }

    void begin_game()
    {
        game->beginGame();
        game->initiateGame();

        std::this_thread::sleep_for(std::chrono::milliseconds(DELAY_PERIOD));

        string message = serverParse::createMazeString(game->maze);
        while (message.size() > 256) {
            for (int j = 255; j >= 0; j--) {
                if (message.at(j) == 'm') {
                    string temp = message.substr(0, j);
                    //cerr << "sending temp message: " << temp << endl;
                    //cerr << "temp message size: " << temp.size() << endl;
                    temp = temp + "\r\n";
                    broadcast(temp);
                    message.erase(message.begin(), message.begin() + j);
                    //cerr << "message size is now: " << message.size() << endl;
                    break;
                }
            }
        }
        if (message.size() > 0) {
            message = message + "\r\n";
            //cerr << "sending remaining: " << message.size() << endl;
            broadcast(message);
        }
        //broadcast("mU,1,1,1,");
        message = "";

        std::this_thread::sleep_for(std::chrono::milliseconds(DELAY_PERIOD));

        message = serverParse::createAbilityString(game->maze);
        broadcast(message);
    }

    void handle_accept(con_handler::pointer connection, const boost::system::error_code& err)
    {
        if (!err) {
            playerConnections[serverParse::userIdCount++] = connection;
            connection -> send_join_message();
        }
        start_accept();

        Player* player = new Player(glm::vec3((7.0f * serverParse::userIdCount) + 3.0f, 3.5f, 3.0f), game, false);
        game->allPlayers.push_back(player);
        game->allBoundingBoxes.push_back(player->getBoundingBox());
        player->setId(serverParse::userIdCount - 1);
        if (serverParse::userIdCount == 1)
        {
            begin_game();
        }

    }

    /*
     *Helper function to send a msg to all clients
     */

    void broadcast(std::string msg)
    {
        int x;
        for(x = 0; x < serverParse::userIdCount; x++)
            playerConnections[x] -> send_game_state(msg);
    }

    /*
     *This function will be called once and will run perpetually.
     *Every PERIOD milliseconds, the function will handle one message per 
     *input queue and broadcast the player info to all connected clients
     */

    void handle_timeout() {
        while (1) {
            std::this_thread::sleep_for(std::chrono::milliseconds(PERIOD));
            if (serverParse::userIdCount > 0) {

                //first handle incoming messages, if there are any
                int bufIndex;
                for (bufIndex = 0; bufIndex < serverParse::userIdCount; bufIndex++) {
                    std::string nextMessage = playerConnections[bufIndex]->dequeue();
                    //cout << "calling sort for playerConnection: " << bufIndex << endl;
                    if (!nextMessage.empty()) {
                        //cout << "Receiving message for player:" << bufIndex << ":" << nextMessage << endl;
                        serverParse::sortClientMessage(game, nextMessage);
                    }
                    string inputMessage = "";
                    if (game)
                    {
                        game->update(PERIOD / 1000.0f);
                        inputMessage = game->getServerInputMessage();
                    }


                    //cout << "Sending: " << inputMessage << endl;
                    broadcast(inputMessage);
                    //printMoving(playerConnections[0]->pid_str);
                    //then broadcast the game_state
                    //for (int p = 0; p < serverParse::userIdCount; p++) {
                    //    //cout << "p = " + to_string(p) + ", pid_str = " + (playerConnections[p]->pid_str) + "\n";
                    //    //cout << "before broadcast position is: " << glm::to_string(game->allPlayers.at(0)->getPosition()) << endl;
                    //    std::string playerStateString = serverParse::buildPlayerMessage(game, playerConnections[p]->pid_str);
                    //    //cout << "Broadcasting:" << playerStateString << endl;
                    //    broadcast(playerStateString);
                    //}
                }
            }
        }
    }
};



int main(int argc, char* argv[])
{
    try
    {
       /*
        * Set port if available.  If not, use default value.
        */
        int portNum = 1234;
        if (argc > 1) {
            portNum = std::stoi(argv[1]);
        }
        
        boost::asio::io_service io_service;
        Server server(io_service,portNum);
        //init state to dummy values

        boost::asio::io_service::work idleWork(io_service);
        std::thread io_thread = std::thread([&]() {io_service.run();});
        std::thread timer_thread = std::thread([&]() {server.handle_timeout();});

        std::cout << "Starting loop" << std::endl;


        while (1) {
        }
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << endl;
    }
    return 0;
}
