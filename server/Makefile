CXX = g++
CPPFLAGS = -g -std=c++17

EXECUTABLES = main

main: main.cpp LevelDBKVStore.o Connection.o
	$(CXX) $(CPPFLAGS) main.cpp LevelDBKVStore.o Connection.o -pthread -lboost_thread -lleveldb -o main

LevelDBKVStore.o: LevelDBKVStore.hpp LevelDBKVStore.cpp KVStore.hpp
	$(CXX) $(CPPFLAGS) -c LevelDBKVStore.cpp

Connection.o: Operation.hpp Connection.hpp Connection.cpp
	$(CXX) $(CPPFLAGS) -c Connection.cpp

clean:
	rm main *.o

cleandb:
	rm -rf leveldb/*