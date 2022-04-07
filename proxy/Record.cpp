#include "Record.hpp"

#include <chrono>

Record::Record(std::string key, const char* data, uint32_t size, std::chrono::system_clock::time_point lastHit) {
	this->key = key;
	this->data = data;
	this->size = size;
	this->lastHit = lastHit;
	this->modified = false;
}

Record::Record(std::string key, const char* data, uint32_t size) {
	this->key = key;
	this->data = data;
	this->size = size;
	this->lastHit = std::chrono::system_clock::now();
	this->modified = false;
}

Record::~Record() {
	free((char*)data);
}

bool RecordTimeCompare::operator()(const Record* lhs, const Record* rhs) const {
	return lhs->lastHit > rhs->lastHit;
}