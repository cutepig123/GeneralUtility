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
		// ��ʼ�ȴ����ӣ���������
		boost::shared_ptr<tcp::socket> psocket(new tcp::socket(m_iosev));
		// �������¼�ֻ��error_code������������boost::bind��socket�󶨽�ȥ
		m_acceptor.async_accept(*psocket,
			boost::bind(&CHelloWorld_Service::accept_handler, this, psocket, _1)
			);
	}
	// �пͻ�������ʱaccept_handler����
	void accept_handler(boost::shared_ptr<tcp::socket> psocket, error_code ec)
	{
		if (ec) return;
		// �����ȴ�����
		start();
		// ��ʾԶ��IP
		std::cout << psocket->remote_endpoint().address() << std::endl;
		// ������Ϣ(������)
		boost::shared_ptr<std::string> pstr(new std::string("hello async world!"));
		psocket->async_write_some(buffer(*pstr),
			boost::bind(&CHelloWorld_Service::write_handler, this, pstr, _1, _2)
			);
	}
	// �첽д������ɺ�write_handler����
	void write_handler(boost::shared_ptr<std::string> pstr,
		error_code ec, size_t bytes_transferred)
	{
		if (ec)
			std::cout << "����ʧ��!" << std::endl;
		else
			std::cout << *pstr << " �ѷ���" << std::endl;
	}
private:
	io_service &m_iosev;
	ip::tcp::acceptor m_acceptor;
};
int asynServer()
{
	io_service iosev;
	CHelloWorld_Service sev(iosev);
	// ��ʼ�ȴ�����
	sev.start();
	iosev.run();
	return 0;
}