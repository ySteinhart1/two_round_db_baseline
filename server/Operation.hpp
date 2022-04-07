#ifndef OPERATION_H
#define OPERATION_H

#include <string>
#include <cstdint>

#include "KVStore.hpp"

enum OperationType {
	GET = 0,
	PUT = 1
};

struct Operation {
	OperationType type;
	std::string key;
	Blob value;
};

#endif