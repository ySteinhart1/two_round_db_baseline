CXX = g++
CPPFLAGS = -g -std=c++17

ALL = keygen proxy client server clients

all: $(ALL)

keygen: keygen.cpp
	$(CXX) $(CPPFLAGS) keygen.cpp -lsodium -o keygen

proxy: proxy.cpp gen-cpp/KV_RPC.o gen-cpp/KV_RPC_types.o gen-cpp/Send_Op.o gen-cpp/Operation_types.o crypto_ops.o
	g++ $(CPPFLAGS) $^ -lboost_filesystem -lboost_serialization -lthrift -lsodium -pthread -fPIC -o $@

server: server.cpp gen-cpp/KV_RPC.o gen-cpp/KV_RPC_types.o
	g++ $(CPPFLAGS) $^ -lboost_filesystem -lboost_serialization -lthrift -lrocksdb -lsodium -pthread -fPIC -o $@

client: client.cpp gen-cpp/Send_Op.o gen-cpp/Operation_types.o
	g++ $(CPPFLAGS) $^ -lboost_filesystem -lboost_serialization -lthrift -lsodium -pthread -fPIC -o $@

clients: clients.cpp gen-cpp/Send_Op.o gen-cpp/Operation_types.o
	g++ $(CPPFLAGS) $^ -lboost_filesystem -lboost_serialization -lthrift -lsodium -pthread -fPIC -o $@

crypto_ops.o: crypto_ops.h crypto_ops.cpp
	g++ ${CPPFLAGS} -lsodium -c crypto_ops.cpp

gen-cpp/KV_RPC.o: gen-cpp/KV_RPC.h gen-cpp/KV_RPC.cpp
	g++ $(CPPFLAGS) gen-cpp/KV_RPC.cpp -c -fPIC -o gen-cpp/KV_RPC.o

gen-cpp/KV_RPC_types.o: gen-cpp/KV_RPC_types.h gen-cpp/KV_RPC_types.cpp
	g++ $(CPPFLAGS) gen-cpp/KV_RPC_types.cpp -c -fPIC -o gen-cpp/KV_RPC_types.o

gen-cpp/KV_RPC.h:
	thrift -r --gen cpp KV_RPC.thrift

gen-cpp/KV_RPC.cpp:
	thrift -r --gen cpp KV_RPC.thrift

gen-cpp/KV_RPC_types.h:
	thrift -r --gen cpp KV_RPC.thrift

gen-cpp/KV_RPC_types.cpp:
	thrift -r --gen cpp KV_RPC.thrift

gen-cpp/Operation_types.cpp:
	thrift -r --gen cpp Operation.thrift

gen-cpp/Operation_types.h:
	thrift -r --gen cpp Operation.thrift

gen-cpp/Send_Op.cpp:
	thrift -r --gen cpp Operation.thrift

gen-cpp/Send_Op.h:
	thrift -r --gen cpp Operation.thrift

clean:
	rm *.o $(ALL) gen-cpp/*