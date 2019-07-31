/*===============================================================
*   Copyright (C) . All rights reserved.")
*   �ļ����ƣ�
*   �� �� �ߣ�zhang
*   �������ڣ�
*   ��    ����ͨ����װ��TcpSocket��ʵ������ͨ�ſͻ���
*           1. �����׽���
*           2. �����˷�����������
*           3. �շ�����
*           4. �ر��׽���
================================================================*/
#include "tcpsocket.hpp"
#include <signal.h>

void sigcb(int no) {
	std::cout << "recv signo:" << no << std::endl;
}
int main(int argc, char *argv[])
{
	if (argc != 3) {
		std::cout << "./tcp_cli ip port\n";
		return -1;
	}
	signal(SIGPIPE, sigcb);
	std::string ip = argv[1];
	uint16_t port = atoi(argv[2]);

	TcpSocket sock;
	CHECK_RET(sock.Socket());
	CHECK_RET(sock.Connect(ip, port));

	while (1) {
		std::string buf;
		std::cout << "client say:";
		fflush(stdout);
		std::cin >> buf;
		sock.Send(buf);
	}
	sock.Close();
	return 0;
}
