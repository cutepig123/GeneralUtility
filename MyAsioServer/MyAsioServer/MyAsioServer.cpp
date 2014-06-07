// MyAsioServer.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <algorithm>
#include <iostream>
#include <boost/asio.hpp>
using namespace boost::asio;

int synServer()
{
	// ����asio�඼��Ҫio_service����
	io_service iosev;
	ip::tcp::acceptor acceptor(iosev,
		ip::tcp::endpoint(ip::tcp::v4(), MY_SRV_PORT));
	for (;;)
	{
		// socket����
		ip::tcp::socket socket(iosev);
		// �ȴ�ֱ���ͻ������ӽ���
		acceptor.accept(socket);
		// ��ʾ���ӽ����Ŀͻ���
		std::cout << socket.remote_endpoint().address() << std::endl;
		// ��ͻ��˷���hello world!
		boost::system::error_code ec;
		socket.write_some(buffer("hello world!"), ec);
		// ���������ӡ������Ϣ
		if (ec)
		{
			std::cout <<
				boost::system::system_error(ec).what() << std::endl;
			break;
		}
		// �뵱ǰ�ͻ�������ɺ�ѭ�������ȴ���һ�ͻ�����
	}
	return 0;
}
