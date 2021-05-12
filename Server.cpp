#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <../util/ts_queue.cpp>
#include <../parsing/serverParse.h>
#include <../Game.h>
#include <../core.h>

#define MAX_CONNECTIONS 4
#define PERIOD 500 //server period in ms

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
    std::string input_str;
    ThreadSafeQueue<std::string> input_buffer;

public:
    int pid;
    string pid_str; //for parser

    serverParse* serverParser;

    typedef boost::shared_ptr<con_handler> pointer;
    con_handler(boost::asio::io_service& io_service, int pid, std::string pid_str) : sock(io_service), pid(pid), pid_str(pid_str) {
        serverParser = new serverParse();
    }
    // creating the pointer
    static pointer create(boost::asio::io_service& io_service, int pid)
    {
        return pointer(new con_handler(io_service, pid, to_string(pid)));
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
            boost::asio::dynamic_buffer(input_str),
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
            input_buffer.push(input_str);
            input_str = "";

            async_read_until(
            sock,
            boost::asio::dynamic_buffer(input_str),
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
           serverParser -> joinMessageHandler();
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
    
        std::string join_msg = serverParser -> buildJoinResponse(to_string(serverParser -> userIdCount-1));

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
    int counter;
    con_handler::pointer playerConnections[MAX_CONNECTIONS];

    int userIdCount;

    Maze* maze;

    Game* game;
    
    void start_accept()
    {
        // socket
        con_handler::pointer connection = con_handler::create(io_service_, userIdCount);

        // asynchronous accept operation and wait for a new connection.
        acceptor_.async_accept(connection->socket(),
        boost::bind(&Server::handle_accept, this, connection,
        boost::asio::placeholders::error));
    }
public:
    //constructor for accepting connection from client
    Server(boost::asio::io_service& io_service); 

    void begin_game()
    {
        game = new Game(false);
        game->beginGame();

        Maze* maze = game->maze;
        string message = "";
        wallInfo** mazeArray = maze->getMazeArray();
        for (int r = 0; r <  maze -> getMazeSize(); r++)
        {
            for (int c = 0; c < maze -> getMazeSize(); c++)
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
        broadcast(message);
    }

    void handle_accept(con_handler::pointer connection, const boost::system::error_code& err)
    {
        if (!err) {
            playerConnections[userIdCount++] = connection;
            connection -> send_join_message();
        }
        start_accept();
        if (userIdCount >= 1)
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
        for(x = 0; x < userIdCount; x++)
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
            if (userIdCount > 0) {

                //first handle incoming messages, if there are any
                int bufIndex;
                for (bufIndex = 0; bufIndex < userIdCount; bufIndex++) {
                    std::string nextMessage = playerConnections[bufIndex]->dequeue();
                    //cout << "calling sort for playerConnection: " << bufIndex << endl;
                    if (!nextMessage.empty())
                        serverParse::sortClientMessage(nextMessage);

                    //printMoving(playerConnections[0]->pid_str);
                    //then broadcast the game_state
                    for (int p = 0; p < userIdCount; p++) {
                        cout << "p = " + to_string(p) + ", pid_str = " + (playerConnections[p]->pid_str) + "\n";
                        std::string playerStateString = serverParse::buildPlayerMessage(playerConnections[p]->pid_str);
                        broadcast(playerStateString);
                    }

                }
            }

        }
    }

    
};

Server::Server(boost::asio::io_service& io_service) : io_service_(io_service),
acceptor_(io_service_, tcp::endpoint(tcp::v4(), 1234)) {
    counter = 0;
    userIdCount = 0;
    start_accept();

}



int main(int argc, char* argv[])
{
    try
    {
        
        boost::asio::io_service io_service;
        Server server(io_service);
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