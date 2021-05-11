#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <../util/ts_queue.cpp>
#include <../parsing/clientParse.h>
#include <../main.h>
#include <../Game.h>

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

    Game* game;

    Client(boost::asio::io_service& io_service) : io_service_(io_service), sock(io_service) {
        game = new Game();
        game->beginGame();
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
            cout << "Received:" << input_buf << endl;
            clientParse::sortServerMessage(game, input_buf);
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
            std::string input_string =  clientParse::buildInputMessage();

            sock.async_write_some(
                boost::asio::buffer(input_string, input_string.size()),
                boost::bind(&Client::client_handle_send_input,
                    this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
        }
    }
    
};



void error_callback(int error, const char* description)
{
    // Print error.
    std::cerr << description << std::endl;
}

/*
 * Helper method which sets callbacks related to the
 * execution of the program. Most of these callbacks
 * are methods which execute in response to user input.
 *
 * @param window Pointer to the window which the program executes in
 * @author Part of 169 starter code
 */
void setup_callbacks(GLFWwindow* window)
{
    // Set the error callback.
    glfwSetErrorCallback(error_callback);
    // Set the window resize callback.
    glfwSetWindowSizeCallback(window, Window::resizeCallback);

    // Set the key callback.
    glfwSetKeyCallback(window, Window::keyCallback);

    // Set the mouse and cursor callbacks
    glfwSetMouseButtonCallback(window, Window::mouse_callback);
    glfwSetCursorPosCallback(window, Window::cursor_callback);
}

/*
 * Helper method which sets up OpenGL settings related to the
 * execution of the program.
 *
 * @author Part of 169 starter code
 */
void setup_opengl_settings()
{
    // Enable depth buffering.
    glEnable(GL_DEPTH_TEST);
    // Related to shaders and z value comparisons for the depth buffer.
    glDepthFunc(GL_LEQUAL);
    // Set polygon drawing mode to fill front and back of each polygon.
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    // Set clear color to black.
    glClearColor(0.0, 0.0, 0.0, 0.0);
}

/*
 * Prints system specific information that the program is running on.
 *
 * @author Part of 169 starter code
 */
void print_versions()
{
    // Get info of GPU and supported OpenGL version.
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "OpenGL version supported: " << glGetString(GL_VERSION)
        << std::endl;

    //If the shading language symbol is defined.
#ifdef GL_SHADING_LANGUAGE_VERSION
    std::cout << "Supported GLSL version is: " <<
        glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
#endif
}

int main(int argc, char* argv[])
{
    std::cout << "Starting client" << std::endl;
    boost::asio::io_service io_service;
    Client client(io_service);
    client.sock.connect(tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 1234));
    client.start();
    std::cout << "Client started" << std::endl;
    //run the io_service in its own thread
    std::thread io_thread = std::thread([&]() {client.io_service_.run();});

    //run timer in its own thread
    std::thread timer_thread = std::thread([&]() {client.client_handle_timeout();});

    std::cout << "Creating window" << std::endl;


    GLFWwindow* window = Window::createWindow(800, 600);
    if (!window)
    {
        exit(EXIT_FAILURE);
    }

    // Print OpenGL and GLSL versions.
    print_versions();

    // Setup callbacks.
    setup_callbacks(window);
    // Setup OpenGL settings.
    setup_opengl_settings();

    // Initialize the shader program; exit if initialization fails.
    if (!Window::initializeProgram()) exit(EXIT_FAILURE);

    // Initialize objects/pointers for rendering; exit if initialization fails.
    if (!Window::initializeObjects(client.game)) exit(EXIT_FAILURE);


    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);


    // Loop while GLFW window should stay open.
    while (!glfwWindowShouldClose(window))
    {
        // Main render display callback. Rendering of objects is done here.
        Window::displayCallback(window);


        // Idle callback. Updating objects, etc. can be done here.
        Window::idleCallback();
    }


    Window::cleanUp();
    // Destroy the window.
    glfwDestroyWindow(window);
    // Terminate GLFW.
    glfwTerminate();

    std::cout << "Beginning main thread" << std::endl;


    while(1){
    }
}
