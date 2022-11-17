CXX = g++
CPPFLAGS = -g -std=c++17 -DBOOST_NO_CXX11_SCOPED_ENUMS -lboost_system 

JAVA_HOME = /usr/lib/jvm/java-11-openjdk-amd64

ALL = main keygen benchmark

all: $(ALL)


main: main.cpp CloudDB.o Record.o
	$(CXX) $(CPPFLAGS) main.cpp CloudDB.o Record.o -pthread -lsodium -lboost_serialization -o main -lboost_filesystem

benchmark: benchmark.cpp CloudDB.o Record.o
	$(CXX) $(CPPFLAGS) benchmark.cpp CloudDB.o Record.o -pthread -lsodium -lboost_serialization -o benchmark

keygen: keygen.cpp
	$(CXX) $(CPPFLAGS) keygen.cpp -lsodium -o keygen

CloudDB.o: CloudDB.hpp Record.hpp CloudDB.cpp
	$(CXX) $(CPPFLAGS) -c CloudDB.cpp -fPIC

Record.o: Record.hpp Record.cpp
	$(CXX) $(CPPFLAGS) -c Record.cpp -fPIC

clean:
	rm *.o $(ALL)