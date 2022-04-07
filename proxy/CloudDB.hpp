
#include <cstdint>
#include <boost/asio.hpp>

#include <unistd.h>
#include <arpa/inet.h>

#include <sodium.h>

#include "Record.hpp"

using namespace boost::asio;
using namespace boost::asio::ip;

#define PACKET_SIZE 4096
#define CHUNK_SIZE (PACKET_SIZE - crypto_secretbox_MACBYTES - crypto_secretbox_NONCEBYTES)

struct EncryptedPacket {
	unsigned char nonce[crypto_secretbox_NONCEBYTES];
	unsigned char ciphertext[crypto_secretbox_MACBYTES + CHUNK_SIZE];
};

class CloudDB {

private:

	unsigned char sodiumKey[crypto_secretbox_KEYBYTES];
	void loadKey(std::string keyFileName);

	unsigned char plaintext[CHUNK_SIZE];

	EncryptedPacket packetBuf;

	mutable_buffer msgBuf;

	io_context ioc;
	tcp::socket sock;

	uint8_t opType; // GET: 0, PUT: 1
	uint8_t keyLen;
	uint32_t msgLen;
	uint32_t netMsgLen;

	mutable_buffer netMsgLenBuf;

public:

	CloudDB(std::string keyFileName);

	Record* get(std::string key);
	void fakeGet(std::string key);

	void put(Record* record);

};