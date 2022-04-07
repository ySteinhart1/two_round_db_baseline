#include "Cache.hpp"

#include <chrono>

#include <iostream>

Cache::Cache(unsigned long size, FrequencySmoother* freqSmoother) {
	this->size = size;
	this->freeSpace = size;
	this->freqSmoother = freqSmoother;
	this->hits = 0;
	this->requests = 0;
	records = std::unordered_map<std::string, Record*>();
	evictionQueue = boost::heap::d_ary_heap<Record*, boost::heap::mutable_<true>, boost::heap::arity<2>, boost::heap::compare<RecordTimeCompare>>();
}

Record* Cache::get(std::string key) {

	requests += 1;

	auto query = records.find(key);

	if(query != records.end()) {

		hits += 1;

		(*(query->second->handle))->lastHit = std::chrono::system_clock::now();

		evictionQueue.update(query->second->handle);

		return query->second;

	} else {

		Record* record = freqSmoother->get(key);

		if(freeSpace < record->size) makeSpace(record->size);

		records[key] = record;

		freeSpace -= record->size;

		auto handle = evictionQueue.push(record);
		(*handle)->handle = handle;

		//printf("Records in Cache: %lu\nFree space: %lu\n", records.size(), freeSpace);

		return record;
	}
}

bool Cache::makeSpace(unsigned long size) {
	
	if(this->size < size) return false;

	Record* toEvict;

	while(freeSpace < size) {
		toEvict = evictionQueue.top();
		evictionQueue.pop();
		records.erase(toEvict->key);

		if(toEvict->modified) freqSmoother->put(toEvict);

		freeSpace += toEvict->size;
		delete toEvict;
	}

	return true;
}

bool Cache::put(Record* record) {

	requests += 1;

	auto query = records.find(record->key);

	uint32_t currSpace;

	if(query != records.end()) {
		
		hits += 1;

		currSpace = query->second->size;
		evictionQueue.erase(query->second->handle);
		delete query->second;
		freeSpace += currSpace;
		records.erase(record->key);
	}

	if(freeSpace < record->size) makeSpace(record->size);

	std::string key = record->key;
	records[key] = record;
	record->modified = true;
	auto handle = evictionQueue.push(record);
	(*handle)->handle = handle;

	record->lastHit = std::chrono::system_clock::now();

	freeSpace -= record->size;

	//printf("Records in Cache: %lu\nFree space: %lu\n", records.size(), freeSpace);

	return true;
}

bool Cache::flush() {

	for(auto it : records) {
		if(it.second->modified) freqSmoother->put(it.second);
		delete it.second;
	}
	return true;
}

void Cache::printStatistics() {
	printf("Records in Cache: %lu\n", records.size());
	printf("Hits: %d\n", hits);
	printf("Total: %d\n", requests);
	printf("Hit Ratio: %.4f\n", hits / (float)requests);
}

