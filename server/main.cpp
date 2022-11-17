#include <iostream>
#include <string>
#include <thread>
#include <cstdint>
#include <utility>

#include <iostream>

#include <unistd.h>

#include "Operation.hpp"
#include "Connection.hpp"
#include "LevelDBKVStore.hpp"

using namespace boost::asio;
using namespace boost::asio::ip;

int main() {

	LevelDBKVStore store("leveldb/db");

	Connection proxyConn(3000);

	Operation op;

	try{

	while(1) {

		op = proxyConn.getOperation();
		// std::cout << op.key << std::endl;

		if(op.type == GET) {
			std::string result = store.get(op.key);
			proxyConn.sendResult(result);
		} else if(op.type == PUT) {
			// std::cout << op.value.data << std::endl;
			store.put(op.key, op.value);
		}

	}
	} catch(std::exception &e) {
		std::cerr << e.what() << std::endl;
		store.close();
	}

	return 0;
}