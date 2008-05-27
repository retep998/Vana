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
#include "Connector.h"
#include "PacketHandler.h"
#include "AbstractPlayer.h"
#include <iostream>

Connector::Connector(const char *ip, short port, AbstractPlayerFactory *apf) {
	int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != NO_ERROR) std::cout << "Error at WSAStartup()" << std::endl; //TODO: Throw exception

	SOCKET sock = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET) {
		std::cout << "Socket error: " << WSAGetLastError() << std::endl;
		exit(2);
	}

	sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = inet_addr(ip);
	service.sin_port = htons(port);

	if (connect(sock, (SOCKADDR *) &service, sizeof(service)) == SOCKET_ERROR) {
		std::cout << "connect() error: " << WSAGetLastError() << std::endl;
		exit(2);
	}

	packetHandler = new PacketHandler(sock, apf, true);
	packetHandler->getPlayer()->setPacketHandler(packetHandler);
	Selector::Instance()->registerSocket(sock, true, false, true, packetHandler);
}

AbstractPlayer * Connector::getPlayer() {
	return packetHandler->getPlayer();
}