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
#include "PacketHandler.h"
//#include "Decoder.h"
#include <Winsock2.h>

#define HEADER_LEN 4
#define BUFFER_LEN 10000

PacketHandler::PacketHandler(int socket, AbstractPlayer* player) {
	this->socket = socket;
	buffer = new unsigned char[BUFFER_LEN];
	bytesInBuffer = 0;
	this->player = player;
	decoder = new Decoder();
	int l = send(socket, (char*)(decoder->getConnectPacket()), Decoder::CONNECT_LENGTH, 0);
	if (l < Decoder::CONNECT_LENGTH) {
		//TODO
	}
}

void PacketHandler::handle (int socket) {
	if (bytesInBuffer < HEADER_LEN) {
		// read header
		int l = recv(socket, (char*)(buffer + bytesInBuffer), HEADER_LEN - bytesInBuffer, 0);
		if (l <= 0) {
			disconnect();
		}
		bytesInBuffer += l;
	}

	if (bytesInBuffer >= HEADER_LEN) {
		int packetSize = Decoder::getLength(buffer);
		if (packetSize < 2) {
			disconnect();
		}
		int l = recv(socket, (char*)(buffer + bytesInBuffer), HEADER_LEN + packetSize - bytesInBuffer, 0);
		if (l <= 0) {
			disconnect();
		}
		bytesInBuffer += l;
		if (bytesInBuffer == packetSize + HEADER_LEN){
			decoder->decrypt(buffer + HEADER_LEN, packetSize);
			player->handleRequest(buffer + HEADER_LEN, packetSize);
			bytesInBuffer = 0;
		}
	}

}


void PacketHandler::sendPacket(unsigned char *buff, int size){
	unsigned char bufs[BUFFER_LEN];
	decoder->createHeader((unsigned char*)bufs, (short)(size));
	decoder->encrypt(buff, size);
	memcpy_s(bufs+4, size, buff, size);
	decoder->next();
	send(socket, (const char*)bufs, size+4, 0);
}

void PacketHandler::disconnect() {
	Selector::Instance()->unregisterSocket(socket);
	closesocket(socket);
	delete player;
}
