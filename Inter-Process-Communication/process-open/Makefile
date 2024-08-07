
CXX = clang++
CFLAGS = -std=c++20 -Wall -pedantic 
CDEBFLAGS = -g -fsanitize=address

all: process-fork-exec process-popen dummy

dummy: dummy.cpp
	$(CXX) -o $@ $< $(CFLAGS) 

process-fork-exec: process-fork-exec.cpp
	$(CXX) -o $@ $< $(CFLAGS) $(CDEBFLAGS)

process-popen: process-popen.cpp
	$(CXX) -o $@ $< $(CFLAGS) $(CDEBFLAGS)

activate: activate.cpp
	$(CXX) -o $@ $< $(CFLAGS) $(CDEBFLAGS)

clean:
	rm -rf dummy process-fork-exec 	process-popen
