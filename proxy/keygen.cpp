#include <fstream>
#include <sodium.h>

using namespace std;

unsigned char key[crypto_secretbox_KEYBYTES];

int main() {
	crypto_secretbox_keygen(key);
	FILE *keyFile = fopen("secretKeys/sodiumKey", "wb");
	fwrite(key, 1, crypto_secretbox_KEYBYTES, keyFile);
	fclose(keyFile);
}
