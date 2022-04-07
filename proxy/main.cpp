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

#include <unistd.h>
#include <arpa/inet.h>

#include <sodium.h>

#include "Cache.hpp"
#include "FrequencySmoother.hpp"

using namespace boost::asio;
using namespace boost::asio::ip;

#define outFilePath "/home/narahari387/Desktop/outFile.txt"

int main(int argc, char* argv[])
{

	if(sodium_init() < 0) {
        std::cerr << "Sodium couldn't be initialized" << std::endl;
        return 1;
    }

	std::string operationType;
	std::string key;
	std::string value;

	Record* valueRecord;

	FrequencySmoother fs(20, 10, "/home/narahari387/Desktop/freqFile.txt", "/home/narahari387/Desktop/waffle/proxy/secretKeys/sodiumKey");

	Cache cache(2000, &fs);

	FILE* outFile = fopen(outFilePath, "wb");

	while(1){

		printf("Getting op\n");

		std::cin >> operationType;
		std::cin >> key;
		std::cin >> value;

		if(std::cin.eof()) break;

		printf("%s %s %s\n", operationType.c_str(), key.c_str(), value.c_str());

		if(operationType == "get")
		{
			valueRecord = fs.get(key);
			if(valueRecord){
				printf("Record Size: %d\n", valueRecord->size);
				fseek(outFile, 0, SEEK_SET);
				fwrite(valueRecord->data, 1, valueRecord->size, outFile);
			}
			else printf("Record not found\n");
		}
		else if(operationType == "put")
		{
			char* valueData = (char*)malloc(value.size() + 1);
			memcpy(valueData, value.c_str(), value.size() + 1);
			valueRecord = new Record(key, valueData, value.size() + 1, std::chrono::system_clock::now());
			fs.put(valueRecord);
			delete valueRecord;
		}
		else if(operationType == "freq")
		{
			std::cerr << "Printing frequencies" << std::endl;
			fs.printFrequencies();
		}
		else if(operationType == "quit")
		{
			fs.serialize("freqFile");
			exit(0);
		}
		else if(operationType == "stats")
		{
			fs.printStatistics();
			cache.printStatistics();
		}
	}

	return 0;
}
