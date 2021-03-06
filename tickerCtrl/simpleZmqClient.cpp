//
//  Hello World client in C++
//  Connects REQ socket to tcp://localhost:5555
//  Sends "Hello" to server, expects "World" back
//
#include <zmq.hpp>
#include <string>
#include <iostream>

int main ()
{
    //  Prepare our context and socket
    zmq::context_t context (1);
    zmq::socket_t socket (context, ZMQ_REQ);

    std::cout << "Connecting to hello world server…" << std::endl;
    socket.connect ("tcp://192.168.1.12:5555");

    zmq::message_t request(255);
    std::string cmdString("{\"command\":12, \"string\":\"Ben says this is coming from a test app\"}");
    memcpy (request.data(), cmdString.c_str(), cmdString.length());
    std::cout << "Sending Hello " << std::endl;
    socket.send (request);

    //  Get the reply.
    zmq::message_t reply;
    socket.recv (&reply);
    std::cout << "Received "<< (const char*)reply.data() << std::endl;

    return 0;
}
