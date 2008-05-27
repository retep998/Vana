/*
Copyright (C) 2008 Vana Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2
of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#include "Acceptor.h"
#include "PacketHandler.h"
#include "AbstractPlayer.h"
#include <iostream>
#include <sstream>
#include <string>

using std::string;
using std::ostringstream;

Acceptor::Acceptor(short port, AbstractPlayerFactory *apf) : abstractPlayerFactory(apf) {
	int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != NO_ERROR) std::cout << "Error at WSAStartup()" << std::endl; //TODO: Throw exception

	SOCKET acceptSocket = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (acceptSocket == INVALID_SOCKET) {
		std::cout << "Socket error: " << WSAGetLastError() << std::endl;
		exit(2);
	}

	sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = INADDR_ANY; //inet_addr("127.0.0.1");
	service.sin_port = htons(port);

	if (bind(acceptSocket, (SOCKADDR *) &service, sizeof(service)) == SOCKET_ERROR) {
		std::cout << "bind() error: " << WSAGetLastError() << std::endl;
		exit(2);
	}

	if (listen(acceptSocket, 10) == SOCKET_ERROR) {
		std::cout << "listen() error: " << WSAGetLastError() << std::endl;
		closesocket(acceptSocket);
		exit(2);
	}

	Selector::Instance()->registerSocket(acceptSocket, true, false, true, this);
}

void Acceptor::handle(int socket) {
	sockaddr_in cli_addr;
	int cli_len = sizeof(cli_addr);
	SOCKET sock = accept(socket, (struct sockaddr *) &cli_addr, &cli_len);

	ostringstream ipStream;
	ipStream << (short) cli_addr.sin_addr.S_un.S_un_b.s_b1 << "." << (short) cli_addr.sin_addr.S_un.S_un_b.s_b2 << "." << (short) cli_addr.sin_addr.S_un.S_un_b.s_b3 << "." << (short) cli_addr.sin_addr.S_un.S_un_b.s_b4;
	string ip = ipStream.str();

	std::cout << "Accepted connection from " << ip << std::endl;
	if (sock == INVALID_SOCKET) {
		std::cout << "Accept error: " << WSAGetLastError() << std::endl;
		return;
	}

	BOOL tcpnodelay = true;
	if(setsockopt(socket, IPPROTO_TCP, TCP_NODELAY, (char *) &tcpnodelay, sizeof(tcpnodelay)) != NO_ERROR) {
		std::cout << "Warning: failed to disable nagle's algorithm: " << WSAGetLastError() << std::endl;
	}

	PacketHandler *ph = new PacketHandler(sock, abstractPlayerFactory);
	AbstractPlayer *player = ph->getPlayer();
	player->setPacketHandler(ph);
	player->setIP(ip);
	Selector::Instance()->registerSocket(sock, true, false, true, ph);
}
