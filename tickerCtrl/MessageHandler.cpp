#include "MessageHandler.h"
#include "cppzmq/zmq.hpp"
#include <string>
#include <iostream>
#include <unistd.h>
#include <thread>

	static void messsagingThread(MessageHandler* msgHandler) {
		std::cout << "Hello, World" << std::endl;
	    //  Prepare our context and socket
	    zmq::context_t context (1);
	    zmq::socket_t socket (context, ZMQ_REP);
	    socket.bind ("tcp://*:5555");

	    while (!msgHandler->m_killThread) {
	        zmq::message_t request;

	        //  Wait for next request from client
	        socket.recv (&request); // This blocks

	        std::cout << "Received: "<< (const char*)request.data() << std::endl;
	        //std::cout << "Received Hello" << std::endl;

	        zmq::message_t reply (12);
	        // Load new image
	        if(!msgHandler->m_imgScroller.reLoadPPM()){
	        	std::cout << "failed to load new image" << std::endl;
	        	memcpy (reply.data (), "Failed", 6);
	        }
	        else{
	        	memcpy (reply.data (), "Succeeded", 9);
	        }

	        //  Send reply back to client
	        socket.send (reply);
	    }
	}

	int MessageHandler::start(){
		// TODO, we really dont want start to be called twice
		m_killThread = false;
		// *this because we need a const reference
		m_msgThread = std::thread(messsagingThread, this);
		return 0;
	}
	int MessageHandler::stop(){
		m_killThread = true;
		m_msgThread.join();
		return 0;
	}