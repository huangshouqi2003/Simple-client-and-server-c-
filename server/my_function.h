#pragma once
#include<iostream>
#include<winsock2.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
void error_die(const char* str);
int startup(unsigned short* port);
DWORD WINAPI accept_request(LPVOID arg);
