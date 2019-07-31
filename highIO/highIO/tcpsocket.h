/*===============================================================
*   Copyright (C) . All rights reserved.")
*   �ļ����ƣ�
*   �� �� �ߣ�zhang
*   �������ڣ�
*   ��    ������װTcpSocket�࣬�����ṩ����socket�ӿ�
*       1. �����׽���
*       2. δ�׽��ְ󶨵�ַ
*       3. �ͻ��������˷�����������
*       3. ����˿�ʼ����
*       4. ����˻�ȡһ���Ѿ����ӳɹ��ͻ��˵��½�socket
*       5. �ͻ����������շ�����
*       6. �ر��׽���
================================================================*/
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define CHECK_RET(q) if((q) == false){ return -1; }

class TcpSocket
{
public:
	TcpSocket() :_sockfd(-1) {
	}
	~TcpSocket() {
	}
	void SetFd(int sockfd) {
		_sockfd = sockfd;
	}
	int GetFd() {
		return _sockfd;
	}
	bool SetNonBlock() {
		//int fcntl(int fd, int cmd, ... /* arg */ );
		//fd:   ������
		//cmd�� ��������Ҫ���еĲ���
		//  F_GETFL/F_SETFL     ��ȡ/��������
		//arg�� Ҫ���õ�����/Ҫ��ȡ�����Դ��λ��
		//  O_NONBLOCK  ����������
		int flag = fcntl(_sockfd, F_GETFL, 0);
		fcntl(_sockfd, F_SETFL, flag | O_NONBLOCK);
		return true;
	}
	bool Socket() {
		_sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (_sockfd < 0) {
			std::cout << "socket error\n";
			return false;
		}
		return true;
	}
	bool Bind(const std::string &ip, const uint16_t port) {
		struct sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		addr.sin_addr.s_addr = inet_addr(ip.c_str());
		socklen_t len = sizeof(struct sockaddr_in);

		int ret = bind(_sockfd, (struct sockaddr*)&addr, len);
		if (ret < 0) {
			std::cout << "bind error\n";
			return false;
		}
		return true;
	}
	//��ʼ���������÷���˵�ͬһʱ����󲢷�������
	bool Listen(int baklog = 5) {
		//int listen(int sockfd, int backlog);
		//sockfd:   �׽���������
		//backlog�� backlog�����ں�����������Ӷ��е����ڵ�����
		int ret = listen(_sockfd, baklog);
		if (ret < 0) {
			std::cout << "listen error\n";
			return false;
		}
		return true;
	}
	//��ȡ���ӳɹ��ͻ���socket�����ҷ��ؿͻ��˵ĵ�ַ��Ϣ
	bool Accept(TcpSocket &sock, struct sockaddr_in *addr = NULL) {
		//int accept(int sockfd, sockaddr *addr,socklen_t *addrlen);
		//  sockfd: �׽���������
		//  addr��  ���ڴ洢�ͻ��˵�ַ��Ϣ
		//  addrlen������������Ҫ�ĵ�ַ���Ⱥͱ���ʵ�ʵĵ�ַ����
		//����ֵ��Ϊ�ͻ��������½���socket��������  ʧ��-1
		//��������ͻ��˵�ͨ�Ŷ���ͨ�����socket������ʵ�ֵ�
		struct sockaddr_in _addr;
		socklen_t len = sizeof(struct sockaddr_in);
		int newfd = accept(_sockfd, (struct sockaddr*)&_addr, &len);
		if (newfd < 0) {
			std::cout << "accept error\n";
			return false;
		}
		sock.SetFd(newfd);
		if (addr != NULL) {
			memcpy(addr, &_addr, len);
		}
		return true;
	}
	//�ͻ��������˷�����������
	bool Connect(std::string &ip, uint16_t port) {
		//int connect(int sockfd, struct sockaddr *addr,
		//  socklen_t addrlen);
		//  sockfd: �׽���������
		//  addr��  ����˼�����ַ��Ϣ
		//  addrlen����ַ��Ϣ����
		struct sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		addr.sin_addr.s_addr = inet_addr(ip.c_str());
		socklen_t len = sizeof(struct sockaddr_in);
		int ret = connect(_sockfd, (struct sockaddr*)&addr, len);
		if (ret < 0) {
			std::cout << "connect error\n";
			return false;
		}
		return true;
	}
	//ͨ�Žӿڣ�tcp�����Ҳ����ֱ���ȷ�������(��Ϊ�����Ѿ������ɹ�)
	bool Recv(std::string &buf) {
		//ssize_t recv(int sockfd, void *buf, size_t len, int flags)
		//sockfd:   �׽���������
		//buf��     �洢���յ�����
		//len��     ��Ҫ���յĳ���
		//flags:    0-��������   
		//          MSG_PEEK-�������ݵ������ݲ��ӽ��ջ������Ƴ�
		//����ֵ��ʵ�ʽ��յ��ֽ���/�Զ˹ر����ӷ���0    ����-1
		while (1) {
			char tmp[1024] = { 0 };
			int ret = recv(_sockfd, tmp, 5, 0);
			if (ret == 0) {
				std::cout << "peer shutdown\n";
				return false;
			}
			else if (ret < 0) {
				if (errno == EAGAIN || errno == EINTR) {
					break;
				}
				std::cout << "recv errno\n";
				return false;
			}
			tmp[ret] = '\0';
			buf += tmp;
		}
		return true;
	}
	bool Send(std::string &buf) {
		//ssize_t send(int sockfd, void *buf, size_t len, int flags)
		//����ֵ��ʵ�ʷ��͵��ֽ���      ʧ�ܣ�-1
		//�������Ѿ��Ͽ������ͻᴥ���쳣
		int ret = send(_sockfd, buf.c_str(), buf.size(), 0);
		if (ret < 0) {
			std::cout << "send error\n";
			return false;
		}
		return true;
	}
	bool Close() {
		close(_sockfd);
		_sockfd = -1;
		return true;
	}
private:
	int _sockfd;
};

