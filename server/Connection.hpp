#ifndef PROXY_CONNECTION
#define PROXY_CONNECTION

#define BOOST_BIND_NO_PLACEHOLDERS

#include <string>
#include <unordered_map>
#include <cstdint>

#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include "Operation.hpp"

using namespace boost::asio;
using namespace boost::asio::ip;

class Connection {

private:

	int portNum;
    io_context ioc;
    tcp::acceptor acceptor;
    tcp::socket sock;

public:
	
	Connection(int portNum);

	Operation getOperation();

	void sendResult(std::string &result);

};

#endif