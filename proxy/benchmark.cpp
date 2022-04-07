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
#include <chrono>
#include <numeric>


#include <unistd.h>
#include <arpa/inet.h>

#include <sodium.h>

#include "Cache.hpp"
#include "FrequencySmoother.hpp"

using namespace boost::asio;
using namespace boost::asio::ip;
using namespace std::chrono;


#define outFilePath "/home/ubuntu/waffle/outFile.txt"

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

	CloudDB store;

	FILE* outFile = fopen(outFilePath, "wb");

    float diff;
    std::vector<float> put_times, get_times;
    char entry[1000];
    std::string str_entry;
    for(int i = 0; i < 1000; i++){
        bzero(entry, 1000);
        entry[i / 8] |= 1 << i % 8;
        str_entry = std::string(entry);
        value = "1";


        char* valueData = (char*)malloc(value.size() + 1);
        memcpy(valueData, value.c_str(), value.size() + 1);
        valueRecord = new Record(entry, valueData, value.size() + 1, std::chrono::system_clock::now());
        store.put(valueRecord);
        delete valueRecord;

        value = "2";

        auto start = high_resolution_clock::now();

        valueData = (char*)malloc(value.size() + 1);
        memcpy(valueData, value.c_str(), value.size() + 1);
        valueRecord = new Record(entry, valueData, value.size() + 1, std::chrono::system_clock::now());
        store.get(entry);
        store.put(valueRecord);
        delete valueRecord;
        auto stop = high_resolution_clock::now();
        put_times.push_back(duration_cast<microseconds>(stop - start).count());

        start = high_resolution_clock::now();
        valueRecord = store.get(key);
        store.put(valueRecord);
        // if(valueRecord){
        //     printf("Record Size: %d\n", valueRecord->size);
        //     fseek(outFile, 0, SEEK_SET);
        //     fwrite(valueRecord->data, 1, valueRecord->size, outFile);
        // }
        // else printf("Record not found\n");
        stop = high_resolution_clock::now();
        get_times.push_back(duration_cast<microseconds>(stop - start).count());

    }
    std::cout << "get_times avg: " << 1.0 * std::accumulate(get_times.begin(), get_times.end(), 0.0) / get_times.size() << std::endl;
    std::cout << "put_times avg: " << 1.0 * std::accumulate(put_times.begin(), put_times.end(), 0.0) / put_times.size() << std::endl;
	return 0;
}
