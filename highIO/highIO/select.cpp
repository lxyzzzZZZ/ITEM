/*===============================================================
*   Copyright (C) . All rights reserved.")
*   文件名称：
*   创 建 者：zhang
*   创建日期：
*   描    述：封装Select类实现select描述符监控的简单操作
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
		//清空集合
		FD_ZERO(&_rfds);
	}
	~Select() {
	}
	bool Add(TcpSocket &sock) {
		int fd = sock.GetFd();
		//void FD_SET(int fd, fd_set *set);
		//向集合中添加一个指定描述符
		FD_SET(fd, &_rfds);
		_max_fd = fd > _max_fd ? fd : _max_fd;
		return true;
	}
	bool Del(TcpSocket &sock) {
		int fd = sock.GetFd();
		//void FD_CLR(int fd, fd_set *set);
		//从集合中移除一个指定的描述符
		FD_CLR(fd, &_rfds);

		for (int i = _max_fd; i > 0; i--) {
			//int  FD_ISSET(int fd, fd_set *set);
			//判断描述符是否在集合中
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
		fd_set rfds = _rfds;//为什么select监控不直接使用_rfds？

		//int select(int nfds, fd_set *readfds, 
		//fd_set *writefds,fd_set *exceptfds, 
		//struct timeval *timeout);
		//返回值：>0 就绪描述符个数  ==0 等待超时   -1 出错
		//
		//因为select每次监控，返回时集合中保存的都是就绪描述符
		//会对集合造成修改，因此每次进行监控之前都需要用户重新
		//向集合中添加一遍所有的描述符
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

	//实例化监听套接字
	TcpSocket sock;
	//创建套接字
	sock.Socket();
	//为套接字绑定地址信息
	sock.Bind("0.0.0.0", 9000);
	//套接字开始监听
	sock.Listen();
	//将监听套接字放到select中进行监控
	s.Add(sock);

	while (1) {
		//select开始对集合中的描述符进行监控
		std::vector<TcpSocket> list;
		if (s.Wait(list) == false) {
			continue;
		}
		//获取到的列表中保存的都是就绪的套接字
		for (int i = 0; i < list.size(); i++) {
			//若就绪的描述符等于监听套接字的描述符
			//表示当前有新的客户端连接到来
			if (list[i].GetFd() == sock.GetFd()) {
				TcpSocket clisock;
				if (sock.Accept(clisock) == false) {
					continue;
				}
				s.Add(clisock);
			}
			else {
				//若不是监听套接字，表示这个套接字是客户端新建的套
				//接字，直接数据通信即可
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
