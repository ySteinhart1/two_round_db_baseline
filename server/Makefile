CXX = g++
CPPFLAGS = -g -std=c++17

EXECUTABLES = main

main: main.cpp LevelDBKVStore.o Connection.o
	$(CXX) $(CPPFLAGS) main.cpp LevelDBKVStore.o Connection.o -pthread -lboost_thread -lleveldb -o main

ZeroKVStore.o: KVStore.hpp ZeroKVStore.hpp ZeroKVStore.cpp
	$(CXX) $(CPPFLAGS) -c ZeroKVStore.cpp

MemoryKVStore.o: KVStore.hpp MemoryKVStore.hpp MemoryKVStore.cpp
	$(CXX) $(CPPFLAGS) -c MemoryKVStore.cpp

LevelDBKVStore.o: KVStore.hpp LevelDBKVStore.hpp LevelDBKVStore.cpp
	$(CXX) $(CPPFLAGS) -c LevelDBKVStore.cpp

Connection.o: Operation.hpp Connection.hpp Connection.cpp
	$(CXX) $(CPPFLAGS) -c Connection.cpp