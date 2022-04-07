#include "CloudDB.hpp"

#include <iostream>

CloudDB::CloudDB(std::string keyFileName) : ioc(), sock(ioc) {
	if(sodium_init() < 0) {
		std::cerr << "Sodium couldn't be initialized" << std::endl;
		exit(1);
	}

	loadKey(keyFileName);

	sock.connect(tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 3000));

	msgBuf = buffer((char*)&packetBuf, sizeof(EncryptedPacket));
	netMsgLenBuf = buffer(&netMsgLen, sizeof(uint32_t));

}

void CloudDB::loadKey(std::string keyFileName) {
	FILE* keyFile = fopen(keyFileName.c_str(), "rb");
	if(!keyFile) {
		std::cerr << "Sodium Key file not found" << std::endl;
		exit(1);
	}
	fread(sodiumKey, 1, crypto_secretbox_KEYBYTES, keyFile);
	fclose(keyFile);
}

Record* CloudDB::get(std::string key) {
	opType = 0;
	keyLen = key.size();

	write(sock, buffer(&opType, sizeof(uint8_t)));
	write(sock, buffer(&keyLen, sizeof(uint8_t)));
	write(sock, buffer(key));

	read(sock, netMsgLenBuf);
	msgLen = ntohl(netMsgLen);

	uint32_t dataLeft = msgLen;

	uint32_t numPackets = msgLen / PACKET_SIZE + (msgLen % PACKET_SIZE != 0);

	uint32_t decryptedDataSize = msgLen - numPackets*(crypto_secretbox_NONCEBYTES + crypto_secretbox_MACBYTES);

	char* recvData = (char*)malloc(decryptedDataSize);

	char* dataIt = recvData;

	uint32_t dataThisIter;

	while(dataLeft > 0) {

		dataThisIter = (dataLeft > PACKET_SIZE) ? CHUNK_SIZE : dataLeft;

		read(sock, msgBuf, transfer_exactly(dataThisIter));

		if(crypto_secretbox_open_easy(plaintext, packetBuf.ciphertext, dataThisIter - crypto_secretbox_NONCEBYTES, packetBuf.nonce, sodiumKey)) {
			std::cerr << "Data has been altered at key " << key << std::endl;
		}

		memcpy(dataIt, plaintext, dataThisIter - crypto_secretbox_NONCEBYTES - crypto_secretbox_MACBYTES);

		dataLeft -= dataThisIter;
		dataIt += dataThisIter;
	}

	Record* record = new Record(key, recvData, decryptedDataSize, std::chrono::system_clock::now());

	return record;
}

void CloudDB::fakeGet(std::string key) {
	opType = 0;
	keyLen = key.size();

	write(sock, buffer(&opType, sizeof(uint8_t)));
	write(sock, buffer(&keyLen, sizeof(uint8_t)));
	write(sock, buffer(key));

	read(sock, netMsgLenBuf);
	msgLen = ntohl(netMsgLen);

	uint32_t dataLeft = msgLen;

	uint32_t dataThisIter;

	while(dataLeft > 0) {

		dataThisIter = (dataLeft > PACKET_SIZE) ? CHUNK_SIZE : dataLeft;

		read(sock, msgBuf, transfer_exactly(dataThisIter));

		dataLeft -= dataThisIter;
	}
}

void CloudDB::put(Record* record) {
	opType = 1;
	keyLen = record->key.size();

	write(sock, buffer(&opType, sizeof(uint8_t)));
	write(sock, buffer(&keyLen, sizeof(uint8_t)));
	write(sock, buffer(record->key));

	msgLen = record->size;
	msgLen += (record->size / CHUNK_SIZE + (record->size % CHUNK_SIZE != 0))*(crypto_secretbox_NONCEBYTES + crypto_secretbox_MACBYTES);

	netMsgLen = htonl(msgLen);
	write(sock, netMsgLenBuf);

	uint32_t dataLeft = record->size;

	uint32_t dataThisIter;

	const char* dataIt = record->data;

	while(dataLeft > 0) {

		dataThisIter = (dataLeft > CHUNK_SIZE) ? CHUNK_SIZE : dataLeft;
		memcpy(plaintext, dataIt, dataThisIter);

		randombytes_buf(packetBuf.nonce, crypto_secretbox_NONCEBYTES);
		crypto_secretbox_easy(packetBuf.ciphertext, plaintext, dataThisIter, packetBuf.nonce, sodiumKey);

		write(sock, msgBuf, transfer_exactly(crypto_secretbox_NONCEBYTES + crypto_secretbox_MACBYTES + dataThisIter));

		dataLeft -= dataThisIter;
		dataIt += dataThisIter;
	}
}