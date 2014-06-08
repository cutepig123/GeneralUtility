#include "stdafx.h"
#include <algorithm>
#include <iostream>
#include <boost/asio.hpp>

using namespace boost::asio;
using boost::asio::ip::tcp;

tcp::endpoint dnsResolve(const char *url)
{
	boost::asio::io_service io_service;
	tcp::resolver resolver(io_service);
	tcp::resolver::query query(tcp::v4(), url, "http");
	auto it = resolver.resolve(query);
	tcp::endpoint receiver_endpoint = *it;
	return receiver_endpoint;
}

int httpClient()
{
	
	io_service iosev;
	
	ip::tcp::socket socket(iosev);
	
	tcp::endpoint ep(dnsResolve("www.google.com.hk"));
	//ip::tcp::endpoint ep(ip::address_v4::from_string(ep), MY_SRV_PORT);
	
	boost::system::error_code ec;
	socket.connect(ep, ec);
	
	if (ec)
	{
		std::cout << boost::system::system_error(ec).what() << std::endl;
		return -1;
	}

	char *cmd = "GET / HTTP/1.0\r\n"
		"Host: www.google.com.hk\r\n"
		"Accept: */*\r\n"
		"Content-Length: 0\r\n"
		"Content-Type: text/html;\r\n"
		"\r\n"
		"\r\n"
		"\r\n"
		"\r\n"
		"\r\n" ;

	socket.write_some(buffer(cmd, strlen(cmd)), ec);
	std::cout << "Write Data: " << cmd;

	char buf[1000];
	size_t len = socket.read_some(buffer(buf), ec);
	std::cout << "Read data:";
	std::cout.write(buf, len);

	return 0;
}
