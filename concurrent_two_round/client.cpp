#include <unordered_map>

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>
#include "gen-cpp/Operation_types.h"
#include "gen-cpp/Send_Op.h"
#include <chrono>
#include <numeric>

#include <sodium.h>



using namespace std;
using namespace::chrono;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
#include <iostream>

#include "constants.h"



void signal_callback_handler(int signum) {
   exit(signum);
}

Operation parseOperation() {
  std::cout << ">";
  Operation op;
  std::string tmp;
  std::cin >> tmp;
  std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::tolower);
  if(tmp == "exit"){
    op.__set_op("exit");
    return op;
  }
  op.__set_op((tmp == "put") ? "put" : "get");
  std::cin >> tmp;
  op.__set_key(tmp);
  if(op.op == "put"){
    std::cin >> tmp;
    op.__set_value(tmp);
  }
  return op;
}


int main() {
    std::shared_ptr<TTransport> socket(new TSocket(PROXY_IP, PROXY_PORT));
    std::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
    std::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
    Send_OpClient client(protocol);
    Operation op;

    try {
        transport->open();

        while(true){
            op = parseOperation();
            if(op.op == "exit"){
                break;
            }
            std::string val;
            client.access(val, op);
            std::cout << val << std::endl;
        }    

        transport->close();
    } catch (TException& tx) {
        cout << "ERROR: " << tx.what() << endl;
  }

}
