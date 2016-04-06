#ifndef UDPSOCKET_H
#define UDPSOCKET_H

#include <memory>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include <string>
#include <cstring>

#define BUFF_LENGTH 100

void printerr(std::string s) {
	perror(s.c_str());
	exit(1);
}

class UDPSocket {
public:
	UDPSocket() {
		int broadcs = 1;
		m_socket = socket(AF_INET, SOCK_DGRAM, 0);
		setsockopt(m_socket, SOL_SOCKET, SO_BROADCAST, &broadcs, sizeof(broadcs));
		int h = 1;
		setsockopt(m_socket, SOL_SOCKET, SO_REUSEPORT, &h, sizeof(h));
	}
	~UDPSocket() { close(m_socket); }

	void bind(unsigned short port) {
		struct sockaddr_in sin_;
		memset((char *)&sin_, 0, sizeof(sin_));
		sin_.sin_family = AF_INET;
		sin_.sin_port = htons(port);
		sin_.sin_addr.s_addr = htonl(INADDR_ANY);

		int r = ::bind(m_socket, (sockaddr *)&sin_, sizeof(sin_));
		if(r == -1)
			printerr("bind");
	}

	int fd() {return m_socket;};

	void send(const std::string &s, const std::string &a, unsigned short port) {
		struct sockaddr_in sin_;
		memset((char *)&sin_, 0, sizeof(sin_));
		sin_.sin_family = AF_INET;
		sin_.sin_port = htons(port);
		inet_aton(a.c_str(), &sin_.sin_addr);
		char buff[BUFF_LENGTH];
		std::string e = s;
		if(s.size() > BUFF_LENGTH - 1)
			e = s.substr(BUFF_LENGTH-1);
		memcpy(buff, e.c_str(), e.size()+1);
		int r = sendto(m_socket, buff, BUFF_LENGTH, 0, (sockaddr *)&sin_, sizeof(sin_));
		if(r == -1)
			printerr("sendto");
	}

	std::string recv() {
		struct sockaddr_in sin_;
		memset((char *)&sin_, 0, sizeof(sin_));
		char buff[BUFF_LENGTH];
		unsigned int l = sizeof(sin_);
		int r = recvfrom(m_socket, buff, BUFF_LENGTH, 0, (sockaddr *)&sin_, &l);
		if(r == -1)
			printerr("recvfrom");
		return std::string(buff);
	}


private:
	UDPSocket (const UDPSocket&);
	UDPSocket &operator=(const UDPSocket&);
	int m_socket;
};

#endif