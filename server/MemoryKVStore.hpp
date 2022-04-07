#ifndef MEMORY_KV_STORE_H
#define MEMORY_KV_STORE_H

#include "KVStore.hpp"

#include <string>
#include <unordered_map>
#include <cstdint>

class MemoryKVStore : public KVStore {

private:

	std::unordered_map<std::string, Blob> dict;

public:

	virtual Blob get(std::string key) override;

	virtual bool put(std::string key, Blob &blob) override;

};

#endif