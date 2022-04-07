#ifndef FREQ_SMOOTHER_H
#define FREQ_SMOOTHER_H

#include <string>
#include <unordered_map>
#include <utility>
#include <set>

#include "CloudDB.hpp"

bool freqCmp(std::pair<std::string, int> a, std::pair<std::string, int> b);

class FrequencySmoother {

private:

	CloudDB store;

	std::set<std::pair<std::string, int>, decltype(&freqCmp)> readTree;
	std::set<std::pair<std::string, int>, decltype(&freqCmp)> writeTree;

	std::unordered_map<std::string, int> readFreqs;
	std::unordered_map<std::string, int> writeFreqs;

	int max_delta_read;
	int max_delta_write;

	bool create(Record* record);
	
	int fakeReadAccesses;
	int fakeWriteAccesses;

public:

	FrequencySmoother(int max_delta_read, int max_delta_write, std::string fileName, std::string keyFileName);

	Record* get(std::string key);

	bool put(Record* record);

	void printFrequencies();

	void serialize(std::string fileName);
	
	void printStatistics();
	
};

#endif
