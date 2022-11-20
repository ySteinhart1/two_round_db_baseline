#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TThreadedServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/transport/TSocket.h>
#include "gen-cpp/KV_RPC.h"
#include "gen-cpp/Send_Op.h"
#include "crypto_ops.h"
#include <unordered_set>
#include <atomic>
#include "constants.h"
#include <iostream>


#include <boost/filesystem.hpp>

#include <boost/archive/tmpdir.hpp>
#include <boost/serialization/utility.hpp>
// #include <boost/serialization/unordered_set.hpp>
#include <boost/serialization/unordered_map.hpp>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>


using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;



std::unordered_map<std::string, int> valueSizes;



std::atomic<int> accesses{0};
std::atomic<int> aborted{0};

std::unordered_map<std::string, std::atomic<bool>> locks;

Entry constructGetEntry(std::string& key){
    Entry entry;
    entry.__set_keyName(key);
    entry.__set_encryptedValue("");
    entry.__set_op("get");
    return entry;
}

Entry constructPutEntry(std::string& key, std::string& value){
    Entry entry;
    entry.__set_keyName(key);
    encrypt(value, entry);
    entry.__set_op("put");
    return entry;
}



void KeySetup(std::string fileName) {
  for(int i = 0; i < KEY_MAX; i++){
    locks[std::to_string(i)] = ATOMIC_VAR_INIT(true);
  }
	if(boost::filesystem::exists(fileName)){
		std::ifstream ifs(fileName);
		boost::archive::text_iarchive ia(ifs);
		ia >> valueSizes;
		ifs.close();
	}
    loadKey(KEY_FILE);
}

void KeyCleanup(std::string fileName) {
	std::ofstream ofs(fileName);
	boost::archive::text_oarchive oa(ofs);
	oa << valueSizes;
	ofs.close();
}

void handleOp(Operation op, std::string* _return, KV_RPCClient& client){
  std::cout << op << std::endl;
    if(op.op == "get"){
			if(valueSizes.find(op.key) != valueSizes.end() && locks[op.key].exchange(false)){
                Entry getEntry = constructGetEntry(op.key);
                std::string ciphertext;
                client.access(ciphertext, getEntry);
                *_return = decrypt(ciphertext).substr(0, valueSizes[op.key]);
                Entry putEntry = constructPutEntry(op.key, *_return);
                std::string tmp;
                client.access(tmp, putEntry);
                locks[op.key].exchange(true);
			}
			else{
				*_return = "Key not found or another transaction blocked this";
        aborted++;
			}
			
		}
		else if(locks[op.key].exchange(false)){
			if(valueSizes.find(op.key) != valueSizes.end()){
            Entry getEntry = constructGetEntry(op.key);
            client.access(*_return, getEntry);
        }
        Entry putEntry = constructPutEntry(op.key, op.value);
        valueSizes[op.key] = op.value.size();
        std::string tmp;
        client.access(*_return, putEntry);
        locks[op.key].exchange(true);
		}
}




class Send_OpHandler : virtual public Send_OpIf {
 public:

  Send_OpHandler() {
    
    KeySetup(SAVE_FILE);

  }

  void access(std::string& _return, const Operation& operation) {
    // std::cout << "PROXY: " << operation.op << " " << operation.key << " " << operation.value << std::endl;
    ::std::shared_ptr<TTransport> socket(new TSocket(SERVER_IP, SERVER_PORT));
    ::std::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
    ::std::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
    KV_RPCClient client(protocol);
    transport->open();

    Operation op;
    op.__set_op(operation.op);
    op.__set_key(operation.key);
    op.__set_value(operation.value);
    handleOp(op, &_return, client);
  }

};

void signal_callback_handler(int signum) {
   KeyCleanup(SAVE_FILE);
   std::cout << "Accesses: " << accesses << std::endl;
   std::cout << "Aborted: " << aborted << std::endl;
   // Terminate program
   exit(signum);
}

int main(int argc, char **argv) {
  signal(SIGINT, signal_callback_handler);
  int port = PROXY_PORT;
  ::std::shared_ptr<Send_OpHandler> handler(new Send_OpHandler());
  ::std::shared_ptr<TProcessor> processor(new Send_OpProcessor(handler));
  ::std::shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
  ::std::shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
  ::std::shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

  
 std::shared_ptr<apache::thrift::server::TServer> server;


  server.reset(
        new TThreadedServer(processor, serverTransport, transportFactory, protocolFactory));
  server->serve();

  return 0;
}
