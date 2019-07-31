/*===============================================================
*   Copyright (C) . All rights reserved.")
*   �ļ����ƣ�
*   �� �� �ߣ�zhang
*   �������ڣ�
*   ��    ������װSelect��ʵ��select��������صļ򵥲���
================================================================*/
#include <iostream>
#include <vector>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>
#include "tcpsocket.hpp"

class Select
{
public:
	Select() :_max_fd(-1) {
		//void FD_ZERO(fd_set *set);
		//��ռ���
		FD_ZERO(&_rfds);
	}
	~Select() {
	}
	bool Add(TcpSocket &sock) {
		int fd = sock.GetFd();
		//void FD_SET(int fd, fd_set *set);
		//�򼯺������һ��ָ��������
		FD_SET(fd, &_rfds);
		_max_fd = fd > _max_fd ? fd : _max_fd;
		return true;
	}
	bool Del(TcpSocket &sock) {
		int fd = sock.GetFd();
		//void FD_CLR(int fd, fd_set *set);
		//�Ӽ������Ƴ�һ��ָ����������
		FD_CLR(fd, &_rfds);

		for (int i = _max_fd; i > 0; i--) {
			//int  FD_ISSET(int fd, fd_set *set);
			//�ж��������Ƿ��ڼ�����
			if (FD_ISSET(i, &_rfds)) {
				_max_fd = i;
				break;
			}
		}
		return true;
	}
	bool Wait(std::vector<TcpSocket> &_list, int timeout = 3) {
		struct timeval tv;
		tv.tv_sec = timeout;
		tv.tv_usec = 0;
		fd_set rfds = _rfds;//Ϊʲôselect��ز�ֱ��ʹ��_rfds��

		//int select(int nfds, fd_set *readfds, 
		//fd_set *writefds,fd_set *exceptfds, 
		//struct timeval *timeout);
		//����ֵ��>0 ��������������  ==0 �ȴ���ʱ   -1 ����
		//
		//��Ϊselectÿ�μ�أ�����ʱ�����б���Ķ��Ǿ���������
		//��Լ�������޸ģ����ÿ�ν��м��֮ǰ����Ҫ�û�����
		//�򼯺������һ�����е�������
		int ret = select(_max_fd + 1, &rfds, NULL, NULL, &tv);
		if (ret < 0) {
			perror("select error");
			return false;
		}
		else if (ret == 0) {
			std::cout << "select timeout expired!!\n";
			return false;
		}
		for (int i = 0; i <= _max_fd; i++) {
			if (!FD_ISSET(i, &rfds)) {
				continue;
			}
			TcpSocket sock;
			sock.SetFd(i);
			_list.push_back(sock);
		}
		return true;
	}
private:
	int _max_fd;
	fd_set _rfds;
};


int main()
{
	Select s;

	//ʵ���������׽���
	TcpSocket sock;
	//�����׽���
	sock.Socket();
	//Ϊ�׽��ְ󶨵�ַ��Ϣ
	sock.Bind("0.0.0.0", 9000);
	//�׽��ֿ�ʼ����
	sock.Listen();
	//�������׽��ַŵ�select�н��м��
	s.Add(sock);

	while (1) {
		//select��ʼ�Լ����е����������м��
		std::vector<TcpSocket> list;
		if (s.Wait(list) == false) {
			continue;
		}
		//��ȡ�����б��б���Ķ��Ǿ������׽���
		for (int i = 0; i < list.size(); i++) {
			//�����������������ڼ����׽��ֵ�������
			//��ʾ��ǰ���µĿͻ������ӵ���
			if (list[i].GetFd() == sock.GetFd()) {
				TcpSocket clisock;
				if (sock.Accept(clisock) == false) {
					continue;
				}
				s.Add(clisock);
			}
			else {
				//�����Ǽ����׽��֣���ʾ����׽����ǿͻ����½�����
				//���֣�ֱ������ͨ�ż���
				std::string buf;
				if (list[i].Recv(buf) == false) {
					s.Del(list[i]);
					continue;
				}
				std::cout << "client say: " << buf << std::endl;

				buf.clear();
				std::cout << "server say: ";
				fflush(stdout);
				std::cin >> buf;
				if (list[i].Send(buf) == false) {
					s.Del(list[i]);
				}
			}
		}
	}
	sock.Close();
}
