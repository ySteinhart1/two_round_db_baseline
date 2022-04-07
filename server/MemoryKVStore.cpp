#include "MemoryKVStore.hpp"

#include <cstdlib>
#include <cstring>

Blob MemoryKVStore::get(std::string key) {
	
	if(dict.find(key) == dict.end()) return Blob();

	return dict[key];
}

bool MemoryKVStore::put(std::string key, Blob &blob) {

	if(dict.find(key) != dict.end()) dict[key].free();

	Blob newBlob(blob);
	dict[key] = newBlob;

	return true;
}
