#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <filesystem>
#include <vector>
#include <cstdint>
#include <boost/asio.hpp>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <cctype>

#include <boost/filesystem.hpp>

#include <boost/archive/tmpdir.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/unordered_set.hpp>
// #include <boost/serialization/unordered_map.hpp>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <unistd.h>
#include <arpa/inet.h>

#include <sodium.h>
#include "CloudDB.hpp"


using namespace boost::asio;
using namespace boost::asio::ip;

std::unordered_set<std::string> keys;


void KeySetup(std::string fileName) {
	if(boost::filesystem::exists(fileName)){
		std::ifstream ifs(fileName);
		boost::archive::text_iarchive ia(ifs);
		ia >> keys;
		ifs.close();
	}
}

void KeyCleanup(std::string fileName) {
	std::ofstream ofs(fileName);
	boost::archive::text_oarchive oa(ofs);
	oa << keys;
	ofs.close();
}

void signal_callback_handler(int signum) {
   KeyCleanup(SAVE_FILE);
   exit(signum);
}





int main(int argc, char* argv[])
{
	signal(SIGINT, signal_callback_handler);
	KeySetup(SAVE_FILE);


	if(sodium_init() < 0) {
        std::cerr << "Sodium couldn't be initialized" << std::endl;
        return 1;
    }

	std::string operationType;
	std::string key;
	std::string value;

	Record* valueRecord;

	FILE* outFile = fopen(outFilePath, "wb");

	CloudDB db(KEY_FILE);

	while(1){

		std::cout << ">";
		std::cin >> operationType;
		std::transform(operationType.begin(), operationType.end(), operationType.begin(), ::tolower);
		if(operationType != "quit")
			std::cin >> key;
		if(operationType == "put")
			std::cin >> value;

		if(std::cin.eof()) break;

		printf("%s %s %s\n", operationType.c_str(), key.c_str(), value.c_str());

		if(operationType == "get")
		{
			if(keys.find(key) != keys.end()){
				Record* rec = db.get(key);
				printf("%s\n", rec->data);

				db.put(rec);
			}
			else{
				std::cout << "Key does not exist in db" << std::endl;
			}
			
		}
		else if(operationType == "put")
		{
			Record* rec = new Record(key, value.c_str(), VALUE_SIZE);
			if(keys.find(key) != keys.end())
				db.get(key);
			db.put(rec);
			keys.insert(key);
		}
		else if(operationType == "quit")
		{	
			KeyCleanup(SAVE_FILE);
			exit(0);
		}
	}

	return 0;
}
