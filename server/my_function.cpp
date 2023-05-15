#include"my_function.h"
void error_die(const char* str)
{
	perror(str);
	exit(1);
}
int startup(unsigned short* port)//port端口，为0时动态分配
{
	//创建套接字
	WSADATA data;
	int ret = WSAStartup(MAKEWORD(1, 1), &data);
	if (ret)
	{
		error_die("WSAStartup");
	}
	int server_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server_socket == -1)
	{
		error_die("套接字");
	}

	//设置端口可复用,设置套接字属性
	int opt = 1;//开关表示可以复用
	ret = setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
	if (ret == -1)
	{
		error_die("setsockopt");
	}
	//配置服务器端的网络地址
	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));//置0
	server_addr.sin_family = AF_INET;//协议族
	server_addr.sin_port = htons(*port);//大小端转换
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	//绑定套接字
	if (bind(server_socket, (struct sockaddr*)&server_addr,
		sizeof(server_addr)) < 0) {
		error_die("bind");
	}
	//动态分配端口
	int nameLen = sizeof(server_addr);
	if (*port == 0)
	{
		if (getsockname(server_socket, (struct sockaddr*)&server_addr, &nameLen) < 0) {
			error_die("getsockname");
		}
		*port = server_addr.sin_port;
	}
	//创建监听队列
	if (listen(server_socket, 5) < 0) {
		error_die("listen");
	}
	return server_socket;
}
DWORD WINAPI accept_request(LPVOID arg)//处理用户的线程
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
	
	

	closesocket(client);//关闭套接字
	return 0;
}