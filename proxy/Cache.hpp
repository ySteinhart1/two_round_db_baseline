#ifndef CACHE_H
#define CACHE_H

#include "Record.hpp"
#include "FrequencySmoother.hpp"

#include <string>
#include <vector>
#include <unordered_map>

#include <boost/heap/d_ary_heap.hpp>

class Cache {

private:

	unsigned long size;
	unsigned long freeSpace;
	
	std::unordered_map<std::string, Record*> records;

	int hits;
	int requests;

	typedef typename boost::heap::d_ary_heap<Record*, boost::heap::mutable_<true>, boost::heap::arity<2>, boost::heap::compare<RecordTimeCompare>> PriorityQueue;

	PriorityQueue evictionQueue;

	FrequencySmoother* freqSmoother;

	bool makeSpace(unsigned long size);

public:

	Cache(unsigned long size, FrequencySmoother* freqSmoother);

	Record* get(std::string key);

	bool put(Record* record);

	bool flush();
	
	void printStatistics();

};

#endif
