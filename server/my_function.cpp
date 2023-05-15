#include"my_function.h"
void error_die(const char* str)
{
	perror(str);
	exit(1);
}
int startup(unsigned short* port)//port�˿ڣ�Ϊ0ʱ��̬����
{
	//�����׽���
	WSADATA data;
	int ret = WSAStartup(MAKEWORD(1, 1), &data);
	if (ret)
	{
		error_die("WSAStartup");
	}
	int server_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server_socket == -1)
	{
		error_die("�׽���");
	}

	//���ö˿ڿɸ���,�����׽�������
	int opt = 1;//���ر�ʾ���Ը���
	ret = setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
	if (ret == -1)
	{
		error_die("setsockopt");
	}
	//���÷������˵������ַ
	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));//��0
	server_addr.sin_family = AF_INET;//Э����
	server_addr.sin_port = htons(*port);//��С��ת��
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	//���׽���
	if (bind(server_socket, (struct sockaddr*)&server_addr,
		sizeof(server_addr)) < 0) {
		error_die("bind");
	}
	//��̬����˿�
	int nameLen = sizeof(server_addr);
	if (*port == 0)
	{
		if (getsockname(server_socket, (struct sockaddr*)&server_addr, &nameLen) < 0) {
			error_die("getsockname");
		}
		*port = server_addr.sin_port;
	}
	//������������
	if (listen(server_socket, 5) < 0) {
		error_die("listen");
	}
	return server_socket;
}
DWORD WINAPI accept_request(LPVOID arg)//�����û����߳�
{
	char buff[1024] = {'1','3','2'};
	int client = (SOCKET)arg;
	while (1)
	{
		int len=recv(client,buff,1024, NULL);
		if (len < 0)
		return 0;
		std::cout << buff;
	//if (len > 0)
	//send(client,buff,1024, NULL);
	}
	
	

	closesocket(client);//�ر��׽���
	return 0;
}