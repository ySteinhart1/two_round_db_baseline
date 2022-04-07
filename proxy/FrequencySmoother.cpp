#include "FrequencySmoother.hpp"

#include <iostream>
#include <fstream>
#include <filesystem>

#include <boost/archive/tmpdir.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/unordered_map.hpp>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

bool freqCmp(std::pair<std::string, int> a, std::pair<std::string, int> b){ return (a.second == b.second ? a.first < b.first : a.second < b.second); }

FrequencySmoother::FrequencySmoother(int max_delta_read, int max_delta_write, std::string fileName, std::string keyFileName) : store(keyFileName), readTree(freqCmp), writeTree(freqCmp) {

	this->max_delta_read = max_delta_read;
	this->max_delta_write = max_delta_write;

	if(std::filesystem::exists(fileName)) {
		std::cerr << "Loading from " << fileName << std::endl;
		std::ifstream ifs(fileName);
		boost::archive::text_iarchive ia(ifs);
		ia >> readTree;
		ia >> writeTree;
		ia >> readFreqs;
		ia >> writeFreqs;
	}

}

Record* FrequencySmoother::get(std::string key) {

	int readFreq;

	auto query = readFreqs.find(key);

	if(query != readFreqs.end()) readFreq = query->second;
	else {
		std::cerr << "Could not find key " << key << " in frequency tree; dne" << std::endl;
		return NULL;
	}

	auto minIt = readTree.begin();
	std::pair<std::string, int> minKey;

	if(minIt != readTree.end()) {
		minKey = *minIt;
		while(readFreq - minKey.second >= max_delta_read) {
			readTree.erase(minIt);
			for(int i = 0; i < readFreq - minKey.second - max_delta_read + 1; i++) {
				store.fakeGet(minKey.first);
				fakeReadAccesses += 1;
			}
			minKey.second = readFreq - max_delta_read + 1;
			readTree.insert(minKey);
			readFreqs[minKey.first] = readFreq - max_delta_read + 1;
			minIt = readTree.begin();
			minKey = *minIt;
		}
	}

	Record* record = store.get(key);
	store.put(record);

	readTree.erase(std::pair<std::string, int>(key, readFreq));

	readTree.insert(std::pair<std::string, int>(key, readFreq + 1));

	readFreqs[key] += 1;
	writeFreqs[key] += 1;

	return record;
}

bool FrequencySmoother::put(Record* record) {

	int writeFreq;
	Record* tmp;

	std::string key = record->key;

	auto query = writeFreqs.find(key);
	if(query != writeFreqs.end()) writeFreq = query->second;
	else {
		create(record);
		return true;
	}

	auto minIt = writeTree.begin();
	std::pair<std::string, int> minKey;

	if(minIt != writeTree.end()) {
		minKey = *minIt;
		while(writeFreq - minKey.second >= max_delta_write) {
			writeTree.erase(minIt);

			tmp = get(minKey.first);

			for(int i = 0; i < writeFreq - minKey.second - max_delta_write + 1; i++) {
				store.put(tmp);
				fakeWriteAccesses += 1;
			}

			delete tmp;

			minKey.second = writeFreq - max_delta_write + 1;
			writeTree.insert(minKey);
			writeFreqs[minKey.first] = writeFreq - max_delta_write + 1;
			minIt = writeTree.begin();
			minKey = *minIt;
		}
	}
	store.fakeGet(key);
	store.put(record);

	writeTree.erase(std::pair<std::string, int>(key, writeFreq));

	writeTree.insert(std::pair<std::string, int>(key, writeFreq + 1));

	writeFreqs[key] += 1;
	readFreqs[key] += 1;

	return true;
}

bool FrequencySmoother::create(Record* record) {

	std::string key = record->key;
	writeFreqs[key] = 0;
	readFreqs[key] = 0;

	auto maxIt = writeTree.rbegin();
	std::pair<std::string, int> maxKey;

	if(maxIt != writeTree.rend()) {
		maxKey = *maxIt;
		for(int i = 0; i < maxKey.second - max_delta_write; i++) {
			store.put(record);
		}
		writeFreqs[key] = maxKey.second - max_delta_write > 0 ? maxKey.second - max_delta_write: 0;
	}

	writeFreqs[key] += 1;
	writeTree.insert(std::pair<std::string, int>(key, 1));
	store.put(record);

	maxIt = readTree.rbegin();

	if(maxIt != readTree.rend()) {
		maxKey = *maxIt;
		for(int i = 0; i < maxKey.second - max_delta_read; i++) {
			store.get(key);
			fakeReadAccesses += 1;
		}
		readFreqs[key] = maxKey.second - max_delta_read > 0 ? maxKey.second - max_delta_read: 0;
	}

	readTree.insert(std::pair<std::string, int>(key, 0));

	return true;

}

void FrequencySmoother::printFrequencies() {
	std::cerr << "Read freqs" << std::endl;
	for(auto it = readFreqs.begin(); it != readFreqs.end(); it++) {
		std::cerr << it->first << " " << it->second << std::endl;
	}
	std::cerr << "Read freqs" << std::endl;
	for(auto it = readTree.begin(); it != readTree.end(); it++) {
		std::cerr << it->first << " " << it->second << std::endl;
	}
	std::cerr << "Write freqs" << std::endl;
	for(auto it = writeFreqs.begin(); it != writeFreqs.end(); it++) {
		std::cerr << it->first << "" << it->second << std::endl;
	}
	std::cerr << "Write freqs" << std::endl;
	for(auto it = writeTree.begin(); it != writeTree.end(); it++) {
		std::cerr << it->first << " " << it->second << std::endl;
	}
}


void FrequencySmoother::printStatistics() {
	printf("Fake Reads: %d\n", fakeReadAccesses);
	printf("Fake Writes: %d\n", fakeWriteAccesses);
}


void FrequencySmoother::serialize(std::string fileName) {
	std::ofstream ofs(fileName);
	boost::archive::text_oarchive oa(ofs);
	oa << readTree << writeTree;
	oa << readFreqs << writeFreqs;
}
