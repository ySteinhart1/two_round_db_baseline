#ifndef RECORD_H
#define RECORD_H

#include <chrono>
#include <string>
#include <cstdint>
#include <boost/heap/d_ary_heap.hpp>

class Record;

struct RecordTimeCompare {
	bool operator()(const Record* lhs, const Record* rhs) const;
};

class Record {

public:

	std::string key;
	const char* data;
	uint32_t size;
	std::chrono::system_clock::time_point lastHit;
	bool modified;

	typedef typename boost::heap::d_ary_heap<Record*, boost::heap::mutable_<true>, boost::heap::arity<2>, boost::heap::compare<RecordTimeCompare>> PriorityQueue;

	PriorityQueue::handle_type handle;

	Record(std::string key, const char* data, uint32_t size, std::chrono::system_clock::time_point lastHit);
	Record(std::string key, const char* data, uint32_t size);
	~Record();

};

#endif