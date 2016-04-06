#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <cstdlib> 
#include <sys/poll.h>
#include <fstream>
#include <string>
#include <streambuf>

#include <functional>

#include "udpsocket.h"

std::string user_name;
std::string broadcast;
unsigned short port;
std::function<void(void)> signalhandlerf;

void signalHandl(int sign) {
	if(signalhandlerf)
		signalhandlerf();
	exit(0);
}

void setSignalHandler(std::function<void(void)> func) {
	signalhandlerf = func;
	signal(SIGINT, &signalHandl);
}

int main(int argc, char **argv) {

	if(argc < 3) {
		std::cout << "Usage : config username\n";
		exit(0);
	}

	std::ifstream cfg(argv[1]);
	cfg >> port;

	user_name = std::string(argv[2]);

	broadcast = "192.168.50.255";

	int timeout = -1;
	auto sock1 = std::make_shared<UDPSocket>();
	auto sock2 = std::make_shared<UDPSocket>();

	sock1->bind(port);

	setSignalHandler([sock2](){
		sock2->send("User [" + user_name + "] disconneced\n", broadcast, port);
	});

	sock2->send("User [" + user_name + "] connected\n", broadcast, port);

	while(1) {
		struct pollfd fd[2];
		fd[0].fd = 0;
		fd[0].events = POLLIN;
		fd[0].revents = 0;

		fd[1].fd = sock1->fd();
		fd[1].events = POLLIN;
		fd[1].revents = 0;

		int r = poll(fd, 2, timeout);
		if(r && (fd[0].revents & POLLIN)) {
			std::string s;
			std::cin >> s;
			if(s.size() > 100)
				std::cout << "Message is too big. It will be truncated to 100 char.\n";
			sock2->send("[" + user_name + "] : " + s, broadcast, port);
		}

		if(r && (fd[1].revents & POLLIN)) {
			std::cout << sock1->recv() << "\n";
		}
	}
	
	return 0;
}