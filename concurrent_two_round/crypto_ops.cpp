
#include "crypto_ops.h"

unsigned char sodiumKey[crypto_secretbox_KEYBYTES];

void loadKey(std::string keyFileName) {
	FILE* keyFile = fopen(keyFileName.c_str(), "rb");
	if(!keyFile) {
		std::cerr << "Sodium Key file not found" << std::endl;
		exit(1);
	}
	fread(sodiumKey, 1, crypto_secretbox_KEYBYTES, keyFile);
	fclose(keyFile);
}

void encrypt(std::string& val, Entry& entry){
    entry.encryptedValue.resize(CIPHERTEXT_LEN + crypto_secretbox_NONCEBYTES);
    unsigned char* ciphertext = (unsigned char*)&entry.encryptedValue[0];
    unsigned char message[VALUE_SIZE];
    memset(message, 0, VALUE_SIZE);
    memcpy(message, val.c_str(), val.length());
    randombytes_buf(ciphertext + CIPHERTEXT_LEN, crypto_secretbox_NONCEBYTES);
    crypto_secretbox_easy(ciphertext, (unsigned char*)val.c_str(), VALUE_SIZE, ciphertext + CIPHERTEXT_LEN, sodiumKey);
}

std::string decrypt(std::string& ciphertext){
    std::string result;
    result.resize(VALUE_SIZE);
    unsigned char* c_text = (unsigned char*)ciphertext.c_str();
    if (crypto_secretbox_open_easy((unsigned char*)&result[0], c_text, CIPHERTEXT_LEN, c_text + CIPHERTEXT_LEN, sodiumKey) != 0) {
        return NULL;
    }
    return result;

}