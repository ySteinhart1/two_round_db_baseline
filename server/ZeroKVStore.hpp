#ifndef ZERO_KV_STORE
#define ZERO_KV_STORE

#include "KVStore.hpp"

#include <string>

class ZeroKVStore : public KVStore {

private:

	unsigned long minSize;
	unsigned long maxSize;

public:
	
	ZeroKVStore(unsigned long minSize, unsigned long maxSize);

	virtual Blob get(std::string key) override;

	virtual bool put(std::string key, Blob blob) override;

};

#endif