#include <iostream>
#include<WinSock2.h>
#include<string>
#include<sys/types.h>
#include<sys/stat.h>
#pragma comment(lib,"WS2_32.lib")
#define PRINTF(str) printf("[%s-%d]%s",__func__,__LINE__,str);
using namespace std;
void error_die(const char* str)
{
	perror(str);
	exit(1);
}
int startup(unsigned short *port)//port端口，为0时动态分配
{
	//创建套接字
	WSADATA data;
	int ret=WSAStartup(MAKEWORD(1, 1), &data);
	if (ret)
	{
		error_die("WSAStartup");
	}
	int server_socket=socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (server_socket == -1)
	{
		error_die("套接字");
	}

	//设置端口可复用,设置套接字属性
	int opt = 1;//开关表示可以复用
	ret=setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
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
	if(bind(server_socket,(struct sockaddr*)&server_addr,
		sizeof(server_addr))<0){
		error_die("bind");
	}
	//动态分配端口
	int nameLen = sizeof(server_addr);
	if (*port == 0)
	{
		if (getsockname(server_socket, (struct sockaddr*)&server_addr, &nameLen) < 0) {
			error_die("getsockname");
		}
		*port=server_addr.sin_port;
	}
	//创建监听队列
	if (listen(server_socket, 5) < 0) {
		error_die("listen");
	}
	return server_socket;
}
int get_line(int sock, char* buff, int size)//从指定的客户端套接字sock，读一行
{
	//报文结束格式为\r\n
	char c = '0';
	int i = 0;
	while (i<size-1&&c!='\n')
	{
		int n = recv(sock, &c, 1, 0);
		if (n > 0)
		{
			if (c == '\r')
			{
				n=recv(sock, &c, 1, MSG_PEEK);
				if (n > 0 && c == '\n')
				{
					recv(sock, &c, 1, 0); 
				}
				else
				{
					c = '\n';
				}
			}
			buff[i++] = c;
		}
		else
		{
			//to do
			c = '\n';
		}
	}
	return 0;
}
void unimpement(int client)//向指定套接字发送错误页面
{
	int i = 0;
}
void not_find(int client)
{
	char buff[1024] = "0";//发送响应头
	strcpy(buff, "HTTP/1.1 404 NOT FOUND\r\n");
	send(client, buff, strlen(buff), 0);
	strcpy(buff, "Server:hsqHTTP/0.1\r\n");
	send(client, buff, strlen(buff), 0);
	strcpy(buff, "Content-type:text/html\n");
	send(client, buff, strlen(buff), 0);
	strcpy(buff, "\r\n");
	send(client, buff, strlen(buff), 0);
	//发送404网页内容
	sprintf(buff, "<HTML>                 \
			<TITLE>Not Found</TITLE>               \
			 <BODY>                                 \
				<H2>The resource is unavailable.</H2>\
			 </BODY>\
				   </HTML>");
	send(client, buff, strlen(buff),0);
}
void headers(int client)
{
	char buff[1024]="0";//发送响应头
	strcpy(buff, "HTTP/1.1 200 OK\r\n");
	send(client, buff, strlen(buff), 0);
	strcpy(buff, "Server:hsqHTTP/0.1\r\n");
	send(client, buff, strlen(buff), 0);
	strcpy(buff, "Content-type:text/html\n");
	send(client, buff, strlen(buff), 0);
	strcpy(buff, "\r\n");
	send(client, buff, strlen(buff), 0);
}
void cat(int client, FILE* resourcc)
{
	//Tinyhttp(linux)是一次读一个字节发一个，很慢
	char buff[4096]="0";
	int count = 0;
	while (1)
	{
		int ret = fread(buff, sizeof(char),sizeof(buff), resourcc);
		if (ret <= 0)
		{
			break;
		}
		send(client, buff, ret, 0);
		count += ret;
	}
	printf("一共发送%d个字节给浏览器\n",count);
}
void server_file(int client, const char* fileName)
{
	int numchars = 1;
	char buff[1024]="0";
	while (numchars > 0 && strcmp(buff, "\n"))
	{
		numchars = get_line(client, buff, sizeof(buff));
		PRINTF(buff);
	}
	FILE* resourcc = NULL;
	if (strcmp(fileName, "htdocs/index.html") == 0)
	{
		resourcc = fopen(fileName, "r");
	}
	else
	{
		resourcc = fopen(fileName, "rb");
	}
	if (resourcc == NULL)
	{
		not_find(client);
	}
	else
	{
		//正式发送资源给浏览器,先发一个头文件给浏览器
		headers(client);
		//发送请求的资源信息
		cat(client, resourcc);
		printf("信息发送完毕");
	}
	fclose(resourcc);
}
DWORD WINAPI accept_request(LPVOID arg)//处理用户的线程
{
	char buff[1024] = {'0'};
	int client = (SOCKET)arg;
	//读取一行数据从GET请求里面
	//"GET/HTTP/1.1\n"
	int numchars=get_line(client, buff, sizeof(buff));
	PRINTF(buff);

	char method[255];
	int j = 0,i=0;
	while (!isspace(buff[j])&&i<sizeof(method)-1)
	{
		method[i++] = buff[j++];
	}
	method[i] = '\0';//'\0'
	PRINTF(method);
	//检测请求的方法本服务器是否支持
	if (stricmp(method, "GET") && stricmp(method, "POST"))//stricmp不区分大小写
	{
		//向浏览器发送错误页面
		unimpement(client);
		return 0;
	}

	//解析资源文件的路径
	//www.hsq.com/abc/sss.html
	//GET /abc/sss.html/HTTP/1.1\n
	char url[255];//存放完整路径
	i = 0;
	while (isspace(buff[j]) && j < sizeof(buff) - 1)//跳过空格
	{
		j++;
	}
	while (!isspace(buff[j]) && i < sizeof(sizeof(url) - 1)&& j < sizeof(buff) - 1)
	{
		url[i++] = buff[j++];
	}
	url[i] = 0;
	PRINTF(url);

	//url=www.hsq.com
	//htdocs/index.html拼接资源路径
	char path[512] = "";
	sprintf(path, "htdocs%s", url);
	if(path[strlen(path)-1]=='/')
	strcat(path, "index.html");
	PRINTF(path);
	struct stat status;
	if (stat(path, &status) == -1)
	{
		//读取剩下的数据包
		while(numchars>0&&strcmp(buff,"\n"))
		numchars=get_line(client, buff, sizeof(buff));
		not_find(client);
	}
	else
	{
		if ((status.st_mode & S_IFMT) == S_IFDIR)//文件检测
		{
			strcat(path, "/index.html");
		}
		server_file(client, path);
	}
	closesocket(client);//关闭套接字
	return 0;
}
int main()
{
	unsigned short port=8000;
	int server_sock = startup(&port);
	cout << "http 服务已将启动正在监听"<<port<<"端口" << endl;
	
	struct sockaddr_in client_addr;
	int client_addr_len = sizeof(client_addr);

	while (1)
	{	//阻塞等待接收访问
		int client_sock = accept(server_sock,
			(struct sockaddr*)&client_addr,
			&client_addr_len);
		if (client_sock == -1)
		{
			error_die("accept");
		}
		//多线程
		DWORD threadld = 0;
		CreateThread(0, 0, accept_request,
			(void*)client_sock,
			0, &threadld);
	}
	system("pause");
	return 0;
}