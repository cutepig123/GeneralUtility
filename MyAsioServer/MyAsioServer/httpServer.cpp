//

#include "stdafx.h"
#include <algorithm>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>
using namespace boost::asio;

#define	CHK_EC(ec)	if (ec) { std::cout << "Error in Line " << __LINE__ << boost::system::system_error(ec).what() << std::endl; ok=0; goto Exit;}

int httpServer()
{
	// 所有asio类都需要io_service对象
	int ok = 1;
	io_service iosev;
	ip::tcp::acceptor acceptor(iosev,
		ip::tcp::endpoint(ip::tcp::v4(), MY_SRV_PORT));
	for (;;)
	{
		// socket对象
		ip::tcp::socket socket(iosev);
		// 等待直到客户端连接进来
		acceptor.accept(socket);
		// 显示连接进来的客户端
		std::cout << socket.remote_endpoint().address() << std::endl;
		// 向客户端发送hello world!
		boost::system::error_code ec;

#if 0
#define	BUF_SZ	1000
		boost::array<char, BUF_SZ> buf;

		while (1)
		{
#if 1
			std::size_t n = socket.read_some(buffer(buf), ec);
#else
			std::size_t n = boost::asio::read(
				socket, buffer(buf),
				boost::asio::transfer_all(), ec);
#endif
			CHK_EC(ec);

			if (n == BUF_SZ)
			{
				buf.data()[n - 1] = 0;
				std::cout << "ReceiveLength: " << n << "\n\tData: " << buf.data() << "\r\n";
			}
			else if (n == 0)
				break;
			else
			{
				buf.data()[n] = 0;
				std::cout << "ReceiveLength: " << n << "\n\tData: " << buf.data() << "\r\n";
				break;
			}
		}
#endif
		char content[1000];
		static int id = 0; 
		sprintf(content, "Nice to meet you %d", id++);

		char reply[1000];
		sprintf(reply, "HTTP/1.1 200 OK\r\n"
			"Content-Length: %u\r\n"
			"Content-Type: text/html;\r\n"
			"\r\n"
			"%s"
			"\r\n"
			"\r\n"
			"\r\n"
			"\r\n", strlen(content), content );

		size_t nSent =socket.write_some(buffer(reply, strlen(reply)+1), ec);
		CHK_EC(ec);
		std::cout << "Send reply:" << reply << "\n";
	}
Exit:
	return 0;
}
