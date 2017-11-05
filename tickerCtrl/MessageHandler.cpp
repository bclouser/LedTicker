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
	    subscriber.setsockopt(ZMQ_SUBSCRIBE, tickerFilter, 10);

	    ImageGen imgGen; // Potentially don't need to even have a class that requires instance if it doesn't have state.
	    ImageScroller::Image img; // Potentially bad scope of this image... if this thread dies :(

	    while (!msgHandler->m_killThread) {
	        zmq::message_t msg;

	        //  Wait for next msg from client
	        if(!subscriber.recv(&msg)){
	        	std::cout << "Something bad happened while waiting for message" << std::endl;
	        }

	        //std::istringstream iss(static_cast<char*>(msg.data()));
	        std::string filter((char*)msg.data(), msg.size());

	        if(filter == tickerFilter){
	        	//  pull out the data from subscription
	        	if(!subscriber.recv(&msg)){
	        		std::cout << "Something bad happened while pulling out data" << std::endl;
	        	}

	        	std::string buf(static_cast<char*>(msg.data()), msg.size());
	        	//std::cout << "size buf = " << msg.size() << " Received: "<< buf << std::endl;
		
		        if(!imgGen.createImageFromEncodedString(buf, img)){
		        	std::cout << "Failed to create the image from encoded string" << std::endl;
		        }

		        if(!msgHandler->m_imgScroller.UpdateImage(img)){
		        	std::cout << "Failed to update image into the image scroller" << std::endl;
		        }
		    	//msgHandler->m_imgScroller.printImage(img);
	        }
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