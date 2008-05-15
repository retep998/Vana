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
#include <stdio.h>
#include <Winsock2.h>
#include "Acceptor.h"
#include "PacketHandler.h"

Acceptor::Acceptor(short port, AbstractPlayerFactory* apf) {
	abstractPlayerFactory = apf;

	int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != NO_ERROR)  printf("Error at WSAStartup()\n"); //TODO: Throw exception

	SOCKET acceptSocket = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (acceptSocket == INVALID_SOCKET) {
		printf ("socket error: %d\n", WSAGetLastError());
		exit(2);
	}

	sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = INADDR_ANY; //inet_addr("127.0.0.1");
	service.sin_port = htons(port);

	if (bind( acceptSocket, (SOCKADDR*) &service, sizeof(service)) == SOCKET_ERROR) {
		printf("bind() error: %d\n", WSAGetLastError());
		exit(2);
	}

	BOOL tcpnodelay = true;
	if(setsockopt(acceptSocket, IPPROTO_TCP, TCP_NODELAY, (char *) &tcpnodelay, sizeof(tcpnodelay)) != NO_ERROR) {
		printf("Warning: failed to disable nagle's algorithm: %d\n", WSAGetLastError());
	}

	if (listen( acceptSocket, 10 ) == SOCKET_ERROR) {
		printf("listen() error: %d\n", WSAGetLastError());
		closesocket(acceptSocket);
		exit(2);
	}

	Selector::Instance()->registerSocket(acceptSocket, true, false, true, this);

}

void Acceptor::handle (int socket) {
	sockaddr_in cli_addr;
	int cli_len = sizeof(cli_addr);
	SOCKET sock = accept(socket, (struct sockaddr *) &cli_addr, &cli_len);
	char ip[15];
	sprintf_s(ip, "%d.%d.%d.%d", cli_addr.sin_addr.S_un.S_un_b.s_b1, cli_addr.sin_addr.S_un.S_un_b.s_b2, cli_addr.sin_addr.S_un.S_un_b.s_b3, cli_addr.sin_addr.S_un.S_un_b.s_b4);
	printf ("accept\n");
	if (sock == INVALID_SOCKET) {
		printf("accept error: %d\n", WSAGetLastError());
		return;
	}

	AbstractPlayer* player = abstractPlayerFactory->createPlayer();
	PacketHandler* ph = new PacketHandler(sock, player);
	player->setPacketHandler(ph);
	player->setIP(ip);
	Selector::Instance()->registerSocket(sock, true, false, true, ph);
}
