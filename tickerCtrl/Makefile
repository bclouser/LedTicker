#CFLAGS=-Wall -O3 -g
CFLAGS=-Wall -Wextra -O3 -g -Ilibzmq/include
CXXFLAGS=$(CFLAGS) -std=c++11
OBJECTS=ImageScroller.o runText.o MessageHandler.o ImageGen.o
BINARIES=runText

# Where our library resides. You mostly only need to change the
# RGB_LIB_DISTRIBUTION, this is where the library is checked out.
RGB_LIB_DISTRIBUTION=rpi-rgb-led-matrix
RGB_INCDIR=$(RGB_LIB_DISTRIBUTION)/include
RGB_LIBDIR=$(RGB_LIB_DISTRIBUTION)/lib
RGB_LIB_NAME=rgbmatrix
RGB_LIB=$(RGB_LIBDIR)/lib$(RGB_LIB_NAME).a

ZMQ_LIB_NAME=zmq
ZMQ_REPO_DIR=libzmq
ZMQ_LIB_DIR=libzmq/src/.libs
ZMQ_LIB=$(ZMQ_LIB_DIR)/lib$(ZMQ_LIB_NAME).a

# TODO, we really can't be statically linking zmq, its huge!!!
LDFLAGS+=-L$(RGB_LIBDIR) -l$(RGB_LIB_NAME) -L$(ZMQ_LIB_DIR) -l:libzmq.a -lrt -lm -lpthread

all : $(BINARIES)

$(ZMQ_LIB):
	cd $(ZMQ_REPO_DIR) && ./autogen.sh
	cd $(ZMQ_REPO_DIR) && CC=$(CC) CXX=$(CXX) ./configure --host=arm-linux-gnueabihf
	$(MAKE) -j8 -C $(ZMQ_REPO_DIR)

$(RGB_LIB): FORCE
	$(MAKE) -C $(RGB_LIBDIR)

$(BINARIES): $(OBJECTS) $(RGB_LIB) $(ZMQ_LIB)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)

%.o : %.cpp
	$(CXX) -I$(RGB_INCDIR) $(CXXFLAGS) -c -o $@ $<

%.o : %.c
	$(CC) -I$(RGB_INCDIR) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJECTS) $(BINARIES)
	$(MAKE) -C $(RGB_LIBDIR) clean
	$(MAKE) -C $(ZMQ_REPO_DIR) clean

FORCE:
.PHONY: FORCE
