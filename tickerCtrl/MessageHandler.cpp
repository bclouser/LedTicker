#include <string>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <thread>
#include "MessageHandler.h"
#include "cppzmq/zmq.hpp"
#include "ImageGen.h"

	static void messsagingThread(MessageHandler* msgHandler) {
	    //  Prepare our context and socket
	    zmq::context_t context (1);
	    zmq::socket_t subscriber (context, ZMQ_SUB);
	    subscriber.connect("tcp://127.0.0.1:5556");
	    const char* tickerFilter = "tickerData";
	    std::cout << "Starting zmq listener" << std::endl;
	    subscriber.setsockopt(ZMQ_SUBSCRIBE, tickerFilter, 10);

	    ImageGen imgGen; // Potentially don't need to even have a class that requires instance if it doesn't have state.
	    if(!imgGen.init()){
	    	std::cout << "Failed to initialize image generator. Thats bad. Exiting thread" << std::endl;
	    	return;
	    }
	    ImageScroller::Image img; // Potentially bad scope of this image... if this thread dies :(

	    while (!msgHandler->m_killThread) {
	        zmq::message_t msg;

	        //  Wait for next msg from client
	        if(!subscriber.recv(&msg)){
	        	std::cout << "Something bad happened while waiting for message" << std::endl;
	        }
	        std::cout << "Got new message!" <<std::endl;

	        //std::istringstream iss(static_cast<char*>(msg.data()));
	        std::string message((char*)msg.data(), msg.size());

	        // Remove the filter from the beginning of the message
	        message.erase(0,std::string(tickerFilter).size());

        	std::cout << "size message = " << message.size() << " Received: "<< message << std::endl;
	
	        if(!imgGen.createImageFromEncodedString(message, img)){
	        	std::cout << "Failed to create the image from encoded string" << std::endl;
	        }

	        if(!msgHandler->m_imgScroller.UpdateImage(img)){
	        	std::cout << "Failed to update image into the image scroller" << std::endl;
	        }
	    	//ImageScroller::printImage(img);
	        
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