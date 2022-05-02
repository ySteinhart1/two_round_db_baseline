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

#include "Record.hpp"
#include "CloudDB.hpp"


using namespace boost::asio;
using namespace boost::asio::ip;
using namespace std::chrono;


#define MAX_KEY 10000
#define VALUE_SIZE 1000

#define outFilePath "./outFile.txt"

int main(int argc, char* argv[])
{

	if(sodium_init() < 0) {
        std::cerr << "Sodium couldn't be initialized" << std::endl;
        return 1;
    }

	std::string operationType;
	std::string value;

	Record* valueRecord;

	CloudDB store("secretKeys/sodiumKey");

	FILE* outFile = fopen(outFilePath, "wb");

    float diff;
    std::vector<float> put_times, get_times;
    std::string key;
    srand( (unsigned)time( NULL ) );
    for(int i = 0; i < 1000; i++){
        
        key = std::string(std::to_string(rand() % MAX_KEY));
        value = "1";


        char* valueData = (char*)malloc(VALUE_SIZE);
        memcpy(valueData, value.c_str(), value.size() + 1);
        valueRecord = new Record(key, valueData, VALUE_SIZE, std::chrono::system_clock::now());
        store.put(valueRecord);
        delete valueRecord;

        value = "2";

        auto start = high_resolution_clock::now();

        valueData = (char*)malloc(VALUE_SIZE);

        memcpy(valueData, value.c_str(), value.size() + 1);
        valueRecord = new Record(key, valueData, VALUE_SIZE, std::chrono::system_clock::now());


        Record* tmp = store.get(key);
        store.put(valueRecord);
        auto stop = high_resolution_clock::now();
        delete tmp;
        delete valueRecord;
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
        delete valueRecord;
        get_times.push_back(duration_cast<microseconds>(stop - start).count());

    }
    std::cout << "get_times avg: " << 1.0 * std::accumulate(get_times.begin(), get_times.end(), 0.0) / get_times.size() << std::endl;
    std::cout << "put_times avg: " << 1.0 * std::accumulate(put_times.begin(), put_times.end(), 0.0) / put_times.size() << std::endl;
	return 0;
}
