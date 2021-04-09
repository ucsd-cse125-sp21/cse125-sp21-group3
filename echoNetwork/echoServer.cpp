#include <iostream>
#include <cstdlib>
#include <boost/asio.hpp>

using namespace std;
using namespace boost::asio;
using ip::tcp;

string read_(tcp::socket & socket) {
       boost::asio::streambuf buf;
       boost::asio::read_until( socket, buf, "\n" );
       string data = boost::asio::buffer_cast<const char*>(buf.data());
       return data;
}

void send_(tcp::socket & socket, const string& message) {
       const string msg = message + "\n";
       boost::asio::write( socket, boost::asio::buffer(message) );
}

int main(int argc, char* argv[]) {

    boost::asio::io_service io_service;

    //listen for new connection
    tcp::acceptor acceptor_(io_service, tcp::endpoint(tcp::v4(), 1234 ));

    //socket creation 
    tcp::socket socket_(io_service);

    //waiting for connection
    acceptor_.accept(socket_);

    //read operation
    string message = read_(socket_);
    cout << "Client Sent: ";
    cout << message << endl;

    //write operation
    send_(socket_, message);
    cout << "Server echoed back to client" << endl;

    return EXIT_SUCCESS;
}