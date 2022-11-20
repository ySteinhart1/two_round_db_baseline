#ifndef CRYPTO_OPS
#define CRYPTO_OPS
#include <sodium.h>
#include <string>
#include <cstring>
#include "gen-cpp/KV_RPC.h"
#include <iostream>
#include "constants.h"

#define CIPHERTEXT_LEN (crypto_secretbox_MACBYTES + VALUE_SIZE)

void loadKey(std::string keyFileName);

void encrypt(std::string& val, Entry& entry);

std::string decrypt(std::string& ciphertext);

#endif