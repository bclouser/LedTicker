

CFLAGS=-Wall -Wextra -O3
CXXFLAGS=$(CFLAGS) -std=c++11
OBJECTS=PPM.o main.o
BINARIES=testPPMParser




all: $(BINARIES)

$(BINARIES): $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) -o $@ 

.PHONY: clean
clean: 
	rm -rf $(BINARIES) $(OBJECTS)