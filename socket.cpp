//#include "stdafx.h"
#include <iostream>
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")
#include <Windows.h>
using namespace std;

#define  PORT 40085
#define  IP_ADDRESS "127.0.0.1"

DWORD WINAPI ClientThread(LPVOID IpParameter)
{
	SOCKET ClientSocket = (SOCKET)IpParameter;
	int Ret = 0;
	char RecvBuffer[MAX_PATH];

	while(true)
	{
		memset(RecvBuffer, 0x00, sizeof(RecvBuffer));
		Ret = recv(ClientSocket, RecvBuffer, MAX_PATH, 0);
		if(Ret == 0 || Ret == SOCKET_ERROR)
		{
			cout<<"客户端退出!"<<endl;
			break;
		}
		cout<<"接收客户端信息为："<<RecvBuffer<<endl;
	}

	return 0;
}

int main(int argc, char* argv[])
{
	WSADATA ws;
	SOCKET ServerSocket, ClientSocket;
	struct sockaddr_in LocalAddr, ClientAddr;
	int Ret = 0;
	int AddrLen = 0;
	HANDLE hThread = NULL;

	// 初始化socket
	if(WSAStartup(MAKEWORD(2,2), &ws) != 0)
	{
		cout<<"Init windows socket Failed:("<<GetLastError()<<endl;
		return -1;
	}

	// socket赋值
	ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(ServerSocket == INVALID_SOCKET)
	{
		cout<<"Creat socket failed:("<<GetLastError()<<endl;
		return -1;
	}

	// 对localaddr初始化数据结构
	LocalAddr.sin_addr.S_un.S_addr = inet_addr(IP_ADDRESS);
	LocalAddr.sin_port = htons(PORT);
	LocalAddr.sin_family = AF_INET;
	memset(LocalAddr.sin_zero, 0x00, 8);

	// 绑定socket和localaddr
	Ret = bind(ServerSocket, (struct sockaddr *)&LocalAddr, sizeof(LocalAddr));
	if(Ret != 0)
	{
		cout<<"Bind socket failed::"<<GetLastError()<<endl;
		return -1;
	}
	// 监听socket
	Ret = listen(ServerSocket, 10);
	if(Ret != 0)
	{
		cout<<"Bind socket failed::"<<GetLastError()<<endl;
		return -1;
	}

	cout<<"服务器端已经启动...."<<endl;

	while(true)
	{
		AddrLen = sizeof(ClientAddr);
		ClientSocket = accept(ServerSocket, (struct sockaddr *)&ClientAddr, &AddrLen);
		if(ClientSocket == INVALID_SOCKET)
		{
			cout<<"Accept Failed::"<<GetLastError()<<endl;
			break;
		}
		cout<<"客户端连接::"<<inet_ntoa(ClientAddr.sin_addr)<<":"<<ClientAddr.sin_port<<endl;

		hThread = CreateThread(NULL, 0, ClientThread, (LPVOID)ClientSocket, 0, NULL);
		if(hThread == NULL)
		{
			cout<<"Create Thread Failed"<<endl;
			break;
		}
		CloseHandle(hThread);
	}

	closesocket(ServerSocket);
	closesocket(ClientSocket);
	WSACleanup();
}