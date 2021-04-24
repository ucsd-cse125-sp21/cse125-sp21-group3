#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <../gameState/game_state.cpp>
#include <../util/ts_queue.cpp>
#include <../parsing/clientParse.cpp>


using namespace boost::asio;
using ip::tcp;
using std::cout;
using std::endl;

game_state* state;


class Client
{

public:

    boost::asio::io_service & io_service_;
    tcp::socket sock;
    std::string input_buf;
    char data[200];

    Client(boost::asio::io_service& io_service) : io_service_(io_service), sock(io_service) {

    }

    void start(){
        async_read_until(
                sock,
                boost::asio::dynamic_buffer(input_buf),
                "\r\n",
                boost::bind(&Client::client_handle_read,
                    this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
    }

    void client_handle_read(const boost::system::error_code& err, size_t bytes_transferred)
    {
        if (!err) {
            sortServerMessage(input_buf);
            input_buf = ""; //clear the input buffer
            async_read_until(
                sock,
                boost::asio::dynamic_buffer(input_buf),
                "\r\n",
                boost::bind(&Client::client_handle_read,
                    this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));

        }
        else {
            std::cerr << "error: " << err.message() << std::endl;
            sock.close();
        }
    }

    
};

int main(int argc, char* argv[])
{
    boost::asio::io_service io_service;
    Client client(io_service);
    //boost::asio::io_service::work idleWork(io_service);
    client.sock.connect(tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 1234));
    client.start();
    client.io_service_.run();
    while(1){
    }
}
