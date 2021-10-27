CC=gcc
CXX=g++
RM=rm -f
CPPFLAGS=-I ../Dependencies/redis-plus-plus/src/
LDFLAGS=-g $(shell root-config --ldflags)
LDLIBS=$(shell root-config --libs)

SRCS=ExampleMicroservice/ExampleMicroservice.cpp
OBJS=$(subst .cc,.o,$(SRCS))

all: ExampleMicroservice

ExampleMicroservice: $(OBJS)
	g++ -I../Dependencies/redis-plus-plus/src/ -I ../Dependencies/redis/deps/ -I ../Dependencies/redis-plus-plus/src/sw/redis++/cxx11 -I ../Dependencies/redis-plus-plus/src/sw/redis++/no_tls -I ../Dependencies/protobuf/src -I ../DataModel/out/cpp ExampleMicroservice/ExampleMicroservice.cpp -o ExampleMicroservice
    #$(CXX) $(LDFLAGS) -o tool $(OBJS) $(LDLIBS)

ExampleMicroservice.o: ExampleMicroservice/ExampleMicroservice.cpp

clean:
    $(RM) $(OBJS)

distclean: clean
    $(RM) tool