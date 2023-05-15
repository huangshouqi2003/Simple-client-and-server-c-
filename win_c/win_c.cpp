#include <stdio.h>
#include<iostream>
#include <stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")  //加载 ws2_32.dll
int main() {
    //初始化DLL
    char msg[1024]="";
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    SOCKET sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct sockaddr_in sockAddr;
    memset(&sockAddr, 0, sizeof(sockAddr));  //每个字节都用0填充
    sockAddr.sin_family = PF_INET;
    sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    sockAddr.sin_port = htons(1234);
    if (connect(sock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR)))
        std::cout << "连接服务器失败" << std::endl;
    else
        std::cout << "已经连接到服务器" << std::endl;
    while (1)
    {
        fgets(msg, 1024, stdin);
        printf("will send: %s", msg);
        if (send(sock, msg, 1024,0) < 0)	//发送数据
        {
            printf("write Error: %s (errno: %d)\n", strerror(errno), errno);
            exit(0);
        }
    }
    
    //关闭套接字
    closesocket(sock);
    //终止使用 DLL
    WSACleanup();

    system("pause");
    return 0;
}