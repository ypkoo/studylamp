// messenger.hpp
// 
// c file for messenger class
// send message to another process
// 
// @author S.H.Lee
// 
// @version 1.0
// @since 2015-11-19
// First implementation

#include "messenger.hpp"
#include <stdio.h>

Messenger::Messenger(const char *addr, int port){
	int res = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (res != 0) {
		printf("WSAStartup failed with error: %d\n", res);
		exit (1);
	}

	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock == INVALID_SOCKET){
		printf("socket creation failed\n");
		exit (1);
	}

	recvAddr.sin_family = AF_INET;
	recvAddr.sin_addr.s_addr = inet_addr(addr);
	recvAddr.sin_port = htons(port);
	// bind(sock, (SOCKADDR *)&service, sizeof(service));
}

Messenger::~Messenger(){
	closesocket(sock);
	WSACleanup();
}

int Messenger::send_message(void *buf, size_t size) {
	int res = sendto(sock, (char *)buf, size, 0, (SOCKADDR *) &recvAddr, sizeof(recvAddr));
	if (res == SOCKET_ERROR)
		return -1;
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

void Messenger::print_buf(void) {
	printf("%s\n", buf);
}