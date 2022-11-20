#include "gen-cpp/KV_RPC.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TThreadedServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>

#include <cstdint>
#include <cassert>
#include <thread>
#include <algorithm>
#include <iostream>

#include <sodium.h>
#include "rocksdb/db.h"

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

#include "constants.h"

class KV_RPCHandler : virtual public KV_RPCIf {
 public:

  rocksdb::DB* db;

  KV_RPCHandler() {
    // Your initialization goes here

    rocksdb::Options options;
    options.create_if_missing = true;
    rocksdb::Status status = rocksdb::DB::Open(options, "db", &db);
    assert(status.ok());

  }

  void create(const Entry& entry) {
    // Your implementation goes here
    //printf("create %s\n", entry.keyName.c_str());
    //fflush(stdout);

    db->Put(rocksdb::WriteOptions(), entry.keyName, entry.encryptedValue);

  }

  void access(std::string& _return, const Entry& entry) {
    // Your implementation goes here
    //printf("access %s\n", entry.keyName.c_str());

    // std::cout << "Got " << entry << std::endl;
    if(entry.op == "get")
        db->Get(rocksdb::ReadOptions(), entry.keyName, &_return);
    else
        db->Put(rocksdb::WriteOptions(), entry.keyName, entry.encryptedValue);
  }

};

int main(int argc, char **argv) {
  int port = SERVER_PORT;
  ::std::shared_ptr<KV_RPCHandler> handler(new KV_RPCHandler());
  ::std::shared_ptr<TProcessor> processor(new KV_RPCProcessor(handler));
  ::std::shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
  ::std::shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
  ::std::shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());
  std::shared_ptr<apache::thrift::server::TServer> server;


  server.reset(
        new TThreadedServer(processor, serverTransport, transportFactory, protocolFactory));
  server->serve();

  return 0;
}

