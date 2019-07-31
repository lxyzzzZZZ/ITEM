#include <sys/epoll.h>
#include <vector>
#include "tcpsocket.hpp"

class Epoll {
public:
	bool Init() {
		//int epoll_create(int size);
		//创建epoll；返回操作句柄
		_epfd = epoll_create(5);
		if (_epfd < 0) {
			perror("epoll create error");
			return false;
		}
		return true;
	}
	bool Add(TcpSocket &sock, uint32_t events = 0) {
		sock.SetNonBlock();
		int fd = sock.GetFd();
		struct epoll_event ev;
		ev.events = EPOLLIN | events;
		ev.data.fd = fd;
		int ret = epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &ev);
		if (ret < 0) {
			perror("epoll ctrol error");
			return false;
		}
		return true;
	}
	bool Del(TcpSocket &sock) {
		int fd = sock.GetFd();
		epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, NULL);
	}
	bool Wait(std::vector<TcpSocket> &list, int timeout = 3000) {
		struct epoll_event evs[10];
		int nfds = epoll_wait(_epfd, evs, 10, timeout);
		if (nfds < 0) {
			perror("epoll wait error");
			return false;
		}
		else if (nfds == 0) {
			std::cout << "epoll wait timeout\n";
			return false;
		}
		for (int i = 0; i < nfds; i++) {
			TcpSocket sock;
			sock.SetFd(evs[i].data.fd);
			list.push_back(sock);
		}
		return true;
	}
private:
	int _epfd;
};

int main()
{
	TcpSocket lst_sock;

	CHECK_RET(lst_sock.Socket());
	CHECK_RET(lst_sock.Bind("0.0.0.0", 9000));
	CHECK_RET(lst_sock.Listen());

	Epoll epoll;
	epoll.Init();
	epoll.Add(lst_sock, EPOLLET);
	while (1) {
		std::vector<TcpSocket> list;
		bool ret = epoll.Wait(list);
		if (ret == false) {
			continue;
		}
		for (int i = 0; i < list.size(); i++) {
			if (list[i].GetFd() == lst_sock.GetFd()) {
				TcpSocket cli_sock;
				ret = lst_sock.Accept(cli_sock);
				if (ret == false) {
					continue;
				}
				epoll.Add(cli_sock, EPOLLET);
			}
			else {
				std::string buf;
				ret = list[i].Recv(buf);
				if (ret == false) {
					epoll.Del(list[i]);
					list[i].Close();
				}
				std::cout << "client say: " << buf << std::endl;
			}
		}
	}
	lst_sock.Close();
	return 0;
}
