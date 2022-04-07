CXX = g++
CPPFLAGS = -g -std=c++17

JAVA_HOME = /usr/lib/jvm/java-11-openjdk-amd64

ALL = main keygen benchmark

all: $(ALL)


main: main.cpp Cache.o FrequencySmoother.o CloudDB.o Record.o
	$(CXX) $(CPPFLAGS) main.cpp Cache.o FrequencySmoother.o CloudDB.o Record.o -pthread -lsodium -lboost_serialization -o main

benchmark: benchmark.cpp CloudDB.o Record.o
	$(CXX) $(CPPFLAGS) benchmark.cpp CloudDB.o Record.o -pthread -lsodium -lboost_serialization -o benchmark

keygen: keygen.cpp
	$(CXX) $(CPPFLAGS) keygen.cpp -lsodium -o keygen

CloudDB.o: CloudDB.hpp Record.hpp CloudDB.cpp
	$(CXX) $(CPPFLAGS) -c CloudDB.cpp -fPIC

FrequencySmoother.o: Record.hpp CloudDB.hpp FrequencySmoother.hpp FrequencySmoother.cpp
	$(CXX) $(CPPFLAGS) -c FrequencySmoother.cpp -fPIC

Cache.o: Cache.hpp Record.hpp Cache.cpp
	$(CXX) $(CPPFLAGS) -c Cache.cpp -fPIC

Record.o: Record.hpp Record.cpp
	$(CXX) $(CPPFLAGS) -c Record.cpp -fPIC

clean:
	rm *.o $(ALL)