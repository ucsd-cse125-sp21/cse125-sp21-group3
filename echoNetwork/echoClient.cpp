#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "../util/ts_queue.cpp"
#include "../parsing/clientParse.cpp"

#define PERIOD 500 //client period in ms

using namespace boost::asio;
using ip::tcp;
using std::cout;
using std::endl;


class Client
{

public:

    boost::asio::io_service & io_service_;
    tcp::socket sock;
    std::string input_buf;

    Client(boost::asio::io_service& io_service) : io_service_(io_service), sock(io_service) {

    }


    /*
     * Start will be called once when to program begins to being the async reading loop
     */

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

    /*
     * Async read handler which will recursively call itself to handle incoming messages
     */

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

    /*
     * Simple function to handle the sending of input, will err and close socket if an error occurs
     */

    void client_handle_send_input(const boost::system::error_code& err, size_t bytes_transferred)
    {
        if (err) {
           std::cerr << "error: " << err.message() << endl;
            sock.close();
        }
    }

    /*
     * Client timer. will poll the state of player inputs every PERIOD ms and send them to the server.
     */

    void client_handle_timeout(){
        while(1){
            std::this_thread::sleep_for(std::chrono::milliseconds(PERIOD));
            std::string input_string = buildInputMessage();

            sock.async_write_some(
                boost::asio::buffer(input_string, input_string.size()),
                boost::bind(&Client::client_handle_send_input,
                    this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
        }
    }
    
};

int main(int argc, char* argv[])
{
    boost::asio::io_service io_service;
    Client client(io_service);
    client.sock.connect(tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 1234));
    client.start();
    //run the io_service in its own thread
    std::thread io_thread = std::thread([&]() {client.io_service_.run();});

    //run timer in its own thread
    std::thread timer_thread = std::thread([&]() {client.client_handle_timeout();});

    while(1){
    }
}
