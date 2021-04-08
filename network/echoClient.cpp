#include <iostream>
#include <cstdlib>
#include <boost/asio.hpp>

using namespace std;
using namespace boost::asio;
using ip::tcp;

int main(int argc, char* argv[]) {
    
    boost::asio::io_service io_service;

    //socket creation
    tcp::socket socket(io_service);

    //connection
    socket.connect(tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 1234 ));

    // request/message from client
    const string msg = "Hello, this is Client.\n";
    boost::system::error_code error;
    boost::asio::write( socket, boost::asio::buffer(msg), error );
    if (!error) {
        cout << "Client sent message." << endl;
    }
    else {
        cout << "send failed: " << error.message() << endl;
    }

    // getting response from server
    boost::asio::streambuf receive_buffer;
    boost::asio::read(socket, receive_buffer, boost::asio::transfer_all(), error);
    if( error && error != boost::asio::error::eof ) {
        cout << "receive failed: " << error.message() << endl;
    }
    else {
        const char* data = boost::asio::buffer_cast<const char*>(receive_buffer.data());
        cout << "Server Echoed:";
        cout << data << endl;
    }

    return EXIT_SUCCESS;
}