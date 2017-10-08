#include <string>
#include <iostream>
#include <unistd.h>
#include <thread>
#include "MessageHandler.h"
#include "cppzmq/zmq.hpp"
#include "json/json.h"

	static void messsagingThread(MessageHandler* msgHandler) {
	    //  Prepare our context and socket
	    zmq::context_t context (1);
	    zmq::socket_t socket (context, ZMQ_REP);
	    socket.bind ("tcp://*:5555");

	    while (!msgHandler->m_killThread) {
	        zmq::message_t msg;

	        //  Wait for next msg from client
	        if(!socket.recv(&msg)){
	        	std::cout << "Something bad happened while waiting for message" << std::endl;
	        }

	        std::cout << "Got message of size: " << msg.size() << std::endl;
	        std::string buf((char*)msg.data(), msg.size());

	        std::cout << "Received: "<< buf << std::endl;
	        //std::cout << "Received Hello" << std::endl;

	        Json::Value jsonMsg;
            Json::Reader reader;
            bool parsingSuccessful = reader.parse(buf.c_str(), jsonMsg);
            if (!parsingSuccessful)
            {
                std::cout  << "Bad json message received. Parser Error: "
                       << reader.getFormattedErrorMessages();
            }
            std::cout << jsonMsg.get("command", -1).asString() << std::endl;

            int command = jsonMsg.get("command", -1).asInt();
	        zmq::message_t reply (12);
            if(command == 12){
		        // Load new image
		        if(!msgHandler->m_imgScroller.reLoadPPM()){
		        	std::cout << "failed to load new image" << std::endl;
		        	memcpy (reply.data(), "Failed", 6);
		        }
		        else{
		        	memcpy (reply.data(), "Succeeded", 9);
		        }
            }
            else{
            	std::cout << "Unrecognized command: " << command << std::endl;
    		}
	        //  Send reply back to client
	        socket.send(reply);
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