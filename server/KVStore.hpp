#ifndef CLOUD_KV_STORE_H
#define CLOUD_KV_STORE_H

#include <chrono>
#include <string>
#include <cstring>

struct Blob {
	uint32_t size;
	const char* data;

	Blob(){size = 0; data = NULL;}
	Blob(uint32_t mySize){size = mySize; data = (char*)malloc(size);}
	Blob(const char* myData, uint32_t mySize){size = mySize; data = myData;}	
	Blob(const Blob& otherBlob){
		size = otherBlob.size;
		data = size ? (char*)malloc(size) : NULL;
		std::memcpy((void*)data, otherBlob.data, size);
	}
	void free(){std::free((void*)data);}
};

class KVStore {

public:
	
	KVStore(){}

	virtual Blob get(std::string key) = 0;

	virtual bool put(std::string key, Blob &blob) = 0;

};

#endif