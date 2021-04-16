#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <../gameState/game_state.cpp>
#define MAX_CONNECTIONS 4

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

public:
    typedef boost::shared_ptr<con_handler> pointer;
    con_handler(boost::asio::io_service& io_service) : sock(io_service) {}
    // creating the pointer
    static pointer create(boost::asio::io_service& io_service)
    {
        return pointer(new con_handler(io_service));
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
            cout << "Server received: " << data << endl;
            std::string response;
            response.assign((char*) state, sizeof(game_state));

            sock.async_write_some(
                boost::asio::buffer(response, max_length),
                boost::bind(&con_handler::handle_write,
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
};

class Server
{
private:
    boost::asio::io_service & io_service_;
    tcp::acceptor acceptor_;
    
    void start_accept()
    {
        // socket
        con_handler::pointer connection = con_handler::create(io_service_);

        // asynchronous accept operation and wait for a new connection.
        acceptor_.async_accept(connection->socket(),
        boost::bind(&Server::handle_accept, this, connection,
        boost::asio::placeholders::error));
    }
public:
    //constructor for accepting connection from client
    Server(boost::asio::io_service& io_service) : io_service_(io_service),
        acceptor_(io_service_, tcp::endpoint(tcp::v4(), 1234)) {
        
        start_accept();
    }
    void handle_accept(con_handler::pointer connection, const boost::system::error_code& err)
    {
        if (!err) {
            connection->start();
        }
        start_accept();
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
        while(1){
            
        }
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << endl;
    }
    return 0;
}
