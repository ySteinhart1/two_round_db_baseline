#ifndef LEVELDB_KV_STORE_H
#define LEVELDB_KV_STORE_H

#include "KVStore.hpp"
#include "leveldb/db.h"
#include "leveldb/options.h"

#include <string>
#include <unordered_map>
#include <cstdint>
#include <iostream>

class LevelDBKVStore {

private:

	leveldb::DB* db;
	leveldb::ReadOptions readOptions;
	leveldb::WriteOptions writeOptions;

public:

	LevelDBKVStore(std::string dbName) {
		leveldb::Options options;
		options.create_if_missing = true;
		leveldb::Status s = leveldb::DB::Open(options, dbName, &db);
		if(!s.ok()) std::cerr << "db not opened" << std::endl;
	}

	std::string get(std::string key);

	bool put(std::string key, Blob &blob);

	void close();

};

#endif