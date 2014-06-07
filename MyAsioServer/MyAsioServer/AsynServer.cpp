#include "stdafx.h"
#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/smart_ptr.hpp>

using namespace boost::asio;
using boost::system::error_code;
using ip::tcp;

struct CHelloWorld_Service{
	CHelloWorld_Service(io_service &iosev)
	:m_iosev(iosev), m_acceptor(iosev, tcp::endpoint(tcp::v4(), MY_SRV_PORT))
	{
	}
	void start()
	{
		// 开始等待连接（非阻塞）
		boost::shared_ptr<tcp::socket> psocket(new tcp::socket(m_iosev));
		// 触发的事件只有error_code参数，所以用boost::bind把socket绑定进去
		m_acceptor.async_accept(*psocket,
			boost::bind(&CHelloWorld_Service::accept_handler, this, psocket, _1)
			);
	}
	// 有客户端连接时accept_handler触发
	void accept_handler(boost::shared_ptr<tcp::socket> psocket, error_code ec)
	{
		if (ec) return;
		// 继续等待连接
		start();
		// 显示远程IP
		std::cout << psocket->remote_endpoint().address() << std::endl;
		// 发送信息(非阻塞)
		boost::shared_ptr<std::string> pstr(new std::string("hello async world!"));
		psocket->async_write_some(buffer(*pstr),
			boost::bind(&CHelloWorld_Service::write_handler, this, pstr, _1, _2)
			);
	}
	// 异步写操作完成后write_handler触发
	void write_handler(boost::shared_ptr<std::string> pstr,
		error_code ec, size_t bytes_transferred)
	{
		if (ec)
			std::cout << "发送失败!" << std::endl;
		else
			std::cout << *pstr << " 已发送" << std::endl;
	}
private:
	io_service &m_iosev;
	ip::tcp::acceptor m_acceptor;
};
int asynServer()
{
	io_service iosev;
	CHelloWorld_Service sev(iosev);
	// 开始等待连接
	sev.start();
	iosev.run();
	return 0;
}