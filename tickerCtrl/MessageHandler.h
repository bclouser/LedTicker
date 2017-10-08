#ifndef _MESSAGE_HANDLER_
#define _MESSAGE_HANDLER_

#include <thread>
#include "ImageScroller.h"

class MessageHandler{
public:
	MessageHandler(ImageScroller& imgScroller):m_imgScroller(imgScroller){}
	int start();
	int stop();
	bool m_killThread = false;
	ImageScroller& m_imgScroller;
private:
	std::thread m_msgThread;
};




#endif