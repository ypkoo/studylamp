// messenger.hpp
// 
// c file for messenger class
// send message to another process
// 
// @author S.H.Lee
// 
// @version 1.1
// @since 2015-11-27
// Implementation for receiving message. (receive_message)
//
// @version 1.0
// @since 2015-11-19
// First implementation

#include "messenger.hpp"
#include <stdio.h>

Messenger::Messenger(const char *addr, int sendPort, int recvPort){
	int res = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (res != 0) {
		fprintf(stderr, "WSAStartup failed with error: %d\n", res);
		exit (1);
	}

	sendSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	recvSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sendSock == INVALID_SOCKET || recvSock == INVALID_SOCKET){
		fprintf(stderr, "socket creation failed\n");
		exit (1);
	}

	sendAddr.sin_family = AF_INET;
	sendAddr.sin_addr.s_addr = inet_addr(addr);
	sendAddr.sin_port = htons(sendPort);
	

	recvAddr.sin_family = AF_INET;
	// recvAddr.sin_addr.s_addr = inet_addr(addr);
	recvAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	recvAddr.sin_port = htons(recvPort);
	recvAddrSize = sizeof(sockaddr_in);
	
	if (bind(recvSock, (SOCKADDR *)&recvAddr, sizeof(recvAddr)) == -1){
		fprintf(stderr, "socket binding error\n");
		exit (1);
	}
}

Messenger::~Messenger(){
	closesocket(sendSock);
	closesocket(recvSock);
	WSACleanup();
}

int Messenger::send_message(void *buf, size_t size) {
	int res = sendto(sendSock, (char *) buf, size, 0, (SOCKADDR *) &sendAddr, sizeof(sendAddr));
	if (res == SOCKET_ERROR){
		wprintf(L"sendto failed with error %d\n", WSAGetLastError());
		return -1;
	}
	return res;
}
int Messenger::send_message(const char *format, ...) {
	int sz;

	va_list args;
	va_start (args, format);
	sz = vsnprintf(buf, MESSAGE_SIZE-1, format, args);
	va_end (args);

	if (sz == -1) {
		printf("message buffer should be bigger.\n");
		exit (1);
	}

	// buf[sz] = 0;
	send_message((void *)buf, sz);
	return sz;
}
int Messenger::receive_message(void *buf, size_t size) {
	int res = recvfrom(recvSock, (char *) buf, size, 0, (SOCKADDR *) &recvAddr, &recvAddrSize);
	if (res == SOCKET_ERROR){
		wprintf(L"recvfrom failed with error %d\n", WSAGetLastError());
		return -1;
	}
	return res;
}

void Messenger::print_buf(void) {
	printf("%s\n", buf);
}