// messenger.hpp
// 
// header file for messenger class
// send message to another process
// 
// @author S.H.Lee
// 
// @version 1.0
// @since 2015-11-19
// First implementation

#ifndef _MESSENGER_HPP_
#define _MESSENGER_HPP_

#include <windows.h>

#define MESSAGE_SIZE 1024

class Messenger {
private:
	WSADATA wsaData;
	SOCKET sock;
	sockaddr_in recvAddr;
	char buf[MESSAGE_SIZE];
public:
	Messenger(const char *addr, int port);
	~Messenger();
	int send_message(void *buf, size_t size);
	int send_message(const char *format, ...);
	void print_buf(void);
};

#endif