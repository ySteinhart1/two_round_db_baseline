#include "Connection.hpp"

#include "Operation.hpp"

#include <iostream>

Connection::Connection(int port)
	: ioc(),
	sock(ioc),
	acceptor(ioc, tcp::endpoint(tcp::v4(), port))
{
	this->portNum = port;
	acceptor.accept(sock);
}

Operation Connection::getOperation() {
	Operation op;

	std::istreambuf_iterator<char> eos; 

	// get operation type
	uint8_t opType;
	mutable_buffer opTypeBuf = buffer(&opType, sizeof(uint8_t));
	read(sock, opTypeBuf);
	op.type = opType ? PUT : GET;

	// get key
	uint8_t keyLen;
	mutable_buffer keyLenBuf = buffer(&keyLen, sizeof(uint8_t));
	read(sock, keyLenBuf);

	std::string key;
	key.resize(keyLen);

	read(sock, buffer(&key[0], keyLen));
	op.key = key;

	if(op.type == PUT) {

		//get rest of serialized data;
		uint32_t dataLen;
		uint32_t netDataLen;
		read(sock, buffer(&netDataLen, sizeof(uint32_t)));
		dataLen = ntohl(netDataLen);

		char* data = (char*)malloc(dataLen);
		mutable_buffer dataBuf = buffer(data, dataLen);
		read(sock, dataBuf);
		op.value = Blob(data, dataLen);

	} else {
		op.value = Blob();
	}

	return op;

}

void Connection::sendResult(std::string &result) {

	uint32_t msgLen = result.size();
	uint32_t netMsgLen = htonl(msgLen);

	write(sock, buffer(&netMsgLen, 4));
	write(sock, buffer(result));
}