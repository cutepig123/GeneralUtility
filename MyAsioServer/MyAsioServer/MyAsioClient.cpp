#include "stdafx.h"
#include <algorithm>
#include <iostream>
#include <boost/asio.hpp>

using namespace boost::asio;

int client()
{
	// 所有asio类都需要io_service对象
	io_service iosev;
	// socket对象
	ip::tcp::socket socket(iosev);
	// 连接端点，这里使用了本机连接，可以修改IP地址测试远程连接
	ip::tcp::endpoint ep(ip::address_v4::from_string(MY_SRV_IP), MY_SRV_PORT);
	// 连接服务器
	boost::system::error_code ec;
	socket.connect(ep, ec);
	// 如果出错，打印出错信息
	if (ec)
	{
		std::cout << boost::system::system_error(ec).what() << std::endl;
		return -1;
	}
	// 接收数据
	char buf[100];
	size_t len = socket.read_some(buffer(buf), ec);
	std::cout.write(buf, len);
	return 0;
}
