#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <../gameState/game_state.cpp>
#include <../util/ts_queue.cpp>
#define MAX_CONNECTIONS 4
#define PERIOD 500 //server period in ms

using namespace boost::asio;
using ip::tcp;
using std::cout;
using std::endl;

game_state* state;

class con_handler : public boost::enable_shared_from_this<con_handler>
{
private:
    tcp::socket sock;
    std::string message = "Hello From Server!";
    enum { max_length = 1024 };
    char data[max_length];
    int pid;
    ThreadSafeQueue<std::string> input_buffer;

public:
    typedef boost::shared_ptr<con_handler> pointer;
    con_handler(boost::asio::io_service& io_service, int pid) : sock(io_service), pid(pid) {}
    // creating the pointer
    static pointer create(boost::asio::io_service& io_service, int pid)
    {
        return pointer(new con_handler(io_service, pid));
    }
    //socket creation
    tcp::socket& socket()
    {
        return sock;
    }

    void start()
    {
        sock.async_read_some(
            boost::asio::buffer(data, max_length),
            boost::bind(&con_handler::handle_read,
                shared_from_this(),
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
    }

    void handle_read(const boost::system::error_code& err, size_t bytes_transferred)
    {
        if (!err) {
            std::string incoming;
            incoming.assign(data, bytes_transferred);
            input_buffer.push(incoming);

            sock.async_read_some(
            boost::asio::buffer(data, max_length),
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
    void handle_write(const boost::system::error_code& err, size_t bytes_transferred)
    {
        if (!err) {
            cout << "Server sent Hello message!" << endl;
            sock.async_read_some(
                boost::asio::buffer(data, max_length),
                boost::bind(&con_handler::handle_read,
                    shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
        }
        else {
            std::cerr << "error: " << err.message() << endl;
            sock.close();
        }
    }

    void handle_send_state(const boost::system::error_code& err, size_t bytes_transferred)
    {
        if (err) {
           std::cerr << "error: " << err.message() << endl;
            sock.close();
        }
    }

    void send_game_state(std::string state_string)
    {
         sock.async_write_some(
                boost::asio::buffer(state_string, max_length),
                boost::bind(&con_handler::handle_send_state,
                    shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
    }

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
    con_handler::pointer players[MAX_CONNECTIONS];
    
    void start_accept()
    {
        // socket
        con_handler::pointer connection = con_handler::create(io_service_, counter);

        // asynchronous accept operation and wait for a new connection.
        acceptor_.async_accept(connection->socket(),
        boost::bind(&Server::handle_accept, this, connection,
        boost::asio::placeholders::error));
    }
public:
    //constructor for accepting connection from client
    Server(boost::asio::io_service& io_service) : io_service_(io_service),
        acceptor_(io_service_, tcp::endpoint(tcp::v4(), 1234)) {
        counter = 0;
        start_accept();
    }
    void handle_accept(con_handler::pointer connection, const boost::system::error_code& err)
    {
        if (!err) {
            connection->start();
            players[counter++] = connection;
        }
        start_accept();
    }

    void handle_timeout(){
        while(1){
            std::this_thread::sleep_for(std::chrono::milliseconds(PERIOD));
            if(counter > 0){

                //first handle incoming messages, if there are any
                int player;
                for(player = 0; player < counter; player++){
                    std::string nextMessage = players[player] -> dequeue();
                    if(!nextMessage.empty())
                        cout << nextMessage << endl; //replace with message parser

                }
                //then broadcast the game_state
                std::string response;
                response.assign((char*) state, sizeof(game_state));
                int i;
                for(i = 0; i < counter; i++)
                    players[i] -> send_game_state(response);

            }
        }
        
    }

    
};

int main(int argc, char* argv[])
{
    try
    {
        
        boost::asio::io_service io_service;
        Server server(io_service);
        //init state to dummy values
        state = (game_state*) malloc(sizeof(game_state));
        state -> x = (float)rand()/(float)(RAND_MAX/10.0);
        state -> y = (float)rand()/(float)(RAND_MAX/10.0);
        state -> z = (float)rand()/(float)(RAND_MAX/10.0);

        boost::asio::io_service::work idleWork(io_service);
        std::thread io_thread = std::thread([&]() {io_service.run();});
        std::thread timer_thread = std::thread([&]() {server.handle_timeout();});
        while(1){
            
        }
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << endl;
    }
    return 0;
}
