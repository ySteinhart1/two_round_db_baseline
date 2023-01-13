#include <set>
#include <string>
#include <iostream>
#include <unordered_map>
#include <numeric>
#include <chrono>

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include "gen-cpp/KV_RPC.h"
#include "crypto_ops.h"
#include "constants.h"
#include "gen-cpp/Operation_types.h"
#include "gen-cpp/Send_Op.h"
#include <thread>

#include <boost/filesystem.hpp>

#include <boost/archive/tmpdir.hpp>
#include <boost/serialization/utility.hpp>
// #include <boost/serialization/unordered_set.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>



using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace std::chrono;


std::atomic<int> accesses{0};
std::atomic<int> aborted{0};

std::atomic<long int> avg_encrypt{0};
std::atomic<long int> avg_round_trip{0};

std::unordered_map<std::string, std::atomic<bool>> locks;
std::unordered_map<std::string, int> valueSizes;






// Operation parseOperation() {
//   Operation op;
//   std::string tmp;
//   std::cin >> tmp;
//   op.type = (tmp == "PUT") ? PUT : GET;
//   std::cin >> op.key;
//   if(op.type == PUT)
//     std::cin >> op.value;
//   return op;
// }

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

Operation randGenOperation(){
    float r = (float) rand()/RAND_MAX;
    Operation op;
    if(r < 0.5){
        op.__set_op("PUT");
    }
    else{
        op.__set_op("GET");
    }
    int key = rand()% KEY_MAX;
    op.__set_key(std::string(std::to_string(key)));
    if(op.op == "GET"){
        char value[VALUE_SIZE];
        randombytes_buf(value, VALUE_SIZE);
        op.__set_value(std::string(value));
    }
    return op;

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

void signal_callback_handler(int signum) {
   KeyCleanup(SAVE_FILE);
   exit(signum);
}

void handleOp(Operation op, std::string* _return, KV_RPCClient& client, std::vector<float>* stat){
  // std::cout << &stat << std::endl;
  // std::cout << op << std::endl;
    if(op.op == "get"){
			if(valueSizes.find(op.key) != valueSizes.end() && locks[op.key].exchange(false)){
        auto start = high_resolution_clock::now();
        Entry getEntry = constructGetEntry(op.key);
        std::string ciphertext;
        client.access(ciphertext, getEntry);
        *_return = decrypt(ciphertext).substr(0, valueSizes[op.key]);
        Entry putEntry = constructPutEntry(op.key, *_return);
        std::string tmp;
        client.access(tmp, putEntry);
        locks[op.key].exchange(true);
        ++accesses;
        auto end = high_resolution_clock::now();
        stat->push_back(duration_cast<microseconds>(end - start).count());
			}
			else{
				*_return = "Key not found or another transaction blocked this";
        aborted++;
			}
			
		}
		else if(locks[op.key].exchange(false)){
      auto start = high_resolution_clock::now();
		  if(valueSizes.find(op.key) == valueSizes.end()){
        valueSizes[op.key] = op.value.size();
        Entry putEntry = constructPutEntry(op.key, op.value);
        client.create(putEntry);
        locks[op.key].exchange(true);
        ++accesses;
      }
      else{
        Entry getEntry = constructGetEntry(op.key);
        client.access(*_return, getEntry);
        Entry putEntry = constructPutEntry(op.key, op.value);
        valueSizes[op.key] = op.value.size();
        std::string tmp;
        client.access(*_return, putEntry);
        locks[op.key].exchange(true);
        ++accesses;
      }
      auto end = high_resolution_clock::now();
      stat->push_back(duration_cast<microseconds>(end - start).count());

		}
    else{
      ++aborted;
    }
}

void clientThread(int i, std::vector<std::vector<float>* >* client_stats){
  std::shared_ptr<TTransport> socket(new TSocket(SERVER_IP, SERVER_PORT));
  std::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
  std::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
  KV_RPCClient client(protocol);

  Operation op;

  try {
    transport->open();
    float diff;
    std::vector<float>* avg_time = (*client_stats)[i];
    // std::cout << &avg_time << std::endl;
    std::string key;
    std::string value;
    for(int j = 0; j < 100; j++) {
        Operation op = randGenOperation();
        std::string ret;
        handleOp(op, &ret, client, avg_time);
        // std::cout << get_times.back() << put_times.back()  << std::endl;
      //std::cerr << (op.type ? "PUT" : "GET") << " " << op.key << " " << op.value << std::endl;
    }    
      } catch (TException& tx) {
      cout << "ERROR: " << tx.what() << endl;
    }

    transport->close();

}

int main() {
  KeySetup(SAVE_FILE);
  srand( (unsigned)time( NULL ) );


  signal(SIGINT, signal_callback_handler);
  std::thread clients[NUM_CLIENTS];
  std::vector<std::vector<float>*> client_stats;
  auto begin = high_resolution_clock::now();
  for(int i = 0; i < NUM_CLIENTS; i++){
    client_stats.push_back(new std::vector<float>);
    clients[i] = std::thread(clientThread, i, &client_stats);
  }
  for(int i = 0; i < NUM_CLIENTS; i++){
    clients[i].join();
  }
  auto end = high_resolution_clock::now();

  int total_ops = 0;
  float total_time = 0;
  for(int i = 0; i < NUM_CLIENTS; i++){
    total_ops += client_stats[i]->size();
    total_time += std::accumulate(client_stats[i]->begin(), client_stats[i]->end(), 0.0);
    delete client_stats[i];
  }
  std::cout << "Completed " << total_ops << " in avg " << total_time / total_ops << std::endl;
  std::cout << "Total time " << duration_cast<microseconds>(end - begin).count() / 1000.0 << "ms" << std::endl;

  KeyCleanup(SAVE_FILE);
}
