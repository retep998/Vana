#include <stdio.h>
#include <Winsock2.h>
#include "Acceptor.h"
#include "PacketHandler.h"

Acceptor::Acceptor(int port, Selector* selector, AbstractPlayerFactory* apf) {
	abstractPlayerFactory = apf;

	SOCKET acceptSocket = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (acceptSocket == INVALID_SOCKET) {
		printf ("socket error: %d\n", WSAGetLastError());
		return;
	}

	sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = INADDR_ANY; //inet_addr("127.0.0.1");
	service.sin_port = htons(port);

	if (bind( acceptSocket, (SOCKADDR*) &service, sizeof(service)) == SOCKET_ERROR) {
		printf("bind() error: %d\n", WSAGetLastError());
		return;
	}

	if (listen( acceptSocket, 10 ) == SOCKET_ERROR) {
		printf("listen() error: %d\n", WSAGetLastError());
		closesocket(acceptSocket);
		return;
	}

	selector->registerSocket (acceptSocket, true, false, true, this);

}

void Acceptor::handle (Selector* selector, int socket) {
	SOCKET sock = accept( socket, NULL, NULL );
	printf ("accept\n");
	if (sock == INVALID_SOCKET) {
		printf("accept error: %d\n", WSAGetLastError());
		return;
	}

	AbstractPlayer* player = abstractPlayerFactory->createPlayer();
	PacketHandler* ph = new PacketHandler(sock, player);
	player->setPacketHandler(ph);
	selector->registerSocket (sock, true, false, true, ph);
}
