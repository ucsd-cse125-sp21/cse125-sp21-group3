#include <iostream>
#include <cstdlib>
#include <boost/asio.hpp>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace std;
using namespace boost::asio;
using ip::tcp;


int main(int argc, char* argv[]) {

    boost::asio::io_service io_service;

    //socket creation
    tcp::socket socket(io_service);

    //connection
    socket.connect(tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 1234));
    
    // request/message from client
    boost::system::error_code error;
    while (true) {
        char message[256];
        std::cin >> message;
        socket.write_some(boost::asio::buffer(message));
        //boost::asio::write(socket, boost::asio::buffer(message), error);
        if (!error) {
            cout << "Client sent: " << message << endl;
        }
        else {
            cout << "send failed: " << error.message() << endl;
        }

        // getting response from server

        char messageReceived[256];
        socket.read_some(boost::asio::buffer(messageReceived, 256));
        //boost::asio::read(socket, receive_buffer, boost::asio::transfer_all(), error);
        /*
        if (error && error != boost::asio::error::eof) {
            cout << "receive failed: " << error.message() << endl;
        }
        else {
            const char* data = boost::asio::buffer_cast<const char*>(receive_buffer.data());
            cout << "Server Echoed: ";
            cout << data << endl;
        }
        */

        cout << "Server Echoed: ";
        cout << messageReceived << endl;
    }
    return EXIT_SUCCESS;
}