#include "ZeroKVStore.hpp"

#include <cstdlib>
#include <cstring>

ZeroKVStore::ZeroKVStore(unsigned long minSize, unsigned long maxSize) {
	this->minSize = minSize;
	this->maxSize = maxSize;
}

Record* ZeroKVStore::get(std::string key) {
	unsigned long size = minSize + (std::rand() % (maxSize - minSize));
	char* data = (char*)malloc(size);
	std::memset(data, 0, size);
	Blob res;
	res.data = data;
	res.size = size;
	return res;
}

bool ZeroKVStore::put(std::string key, Blob &blob) {
	return true;
}
