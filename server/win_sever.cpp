#include"my_function.h"
#pragma comment (lib, "ws2_32.lib")  
int main() 
{
    unsigned short port = 1234;
    int server_sock=startup(&port);
    std::cout << "服务端已启动端口为:" << port << std::endl;

    //接收客户端请求
    struct sockaddr_in client_addr;
    int client_addr_len = sizeof(client_addr);

    while (1)
    {	//阻塞等待接收访问
        int client_sock = accept(server_sock,(struct sockaddr*)&client_addr,&client_addr_len);
        if (client_sock == -1)
        {
            error_die("accept");
        }
        //多线程
        DWORD threadld = 0;
        CreateThread(0, 0, accept_request,(void*)client_sock,0, &threadld);
    }
    closesocket(server_sock);
    //终止 DLL 的使用
    WSACleanup();
    return 0;
}