#include "LevelDBKVStore.hpp"

#include "leveldb/slice.h"
#include "leveldb/iterator.h"

#include <cstdlib>
#include <cstring>
#include <iostream>

std::string LevelDBKVStore::get(std::string key) {

	std::string result;
	leveldb::Status status = db->Get(readOptions, key, &result);
	if(!status.ok()) std::cerr << "DB read went badly" << std::endl;

	return result;
}

bool LevelDBKVStore::put(std::string key, Blob &blob) {
	std::string newVal(blob.data, blob.size);
	leveldb::Status status = db->Put(writeOptions, key, newVal);
	if(!status.ok()) std::cerr << "DB write went badly" << std::endl;
	return true;
}

void LevelDBKVStore::close() {
	delete db;
}
