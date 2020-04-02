CC = g++
CCFLAGS = -std=c++11 -g #-O3 


all:
	$(CC) $(CCFLAGS) -o preach++ main.cc ConstructionGraph.cc QueryGraph.cc Fifo.cc
