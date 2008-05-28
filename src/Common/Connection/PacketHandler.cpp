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
#include "AbstractPlayer.h"
#include "Decoder/Decoder.h"
#include <Winsock2.h>

PacketHandler::PacketHandler(int socket, AbstractPlayerFactory *abstractPlayerFactory, bool isSend) :
bytesInBuffer(0),
player(abstractPlayerFactory->createPlayer()),
decoder(new Decoder()),
socket(socket)
{
	int len;
	if (isSend) {
		len = recv(socket, (char *) buffer, Decoder::CONNECT_LENGTH, 0);
		decoder->setIvSend(buffer+6);
		decoder->setIvRecv(buffer+10);
	}
	else {
		len = send(socket, (char *) decoder->getConnectPacket(), Decoder::CONNECT_LENGTH, 0);
	}
	if (len < Decoder::CONNECT_LENGTH) {
		//TODO
	}
}

void PacketHandler::handle(int socket) {
	if (bytesInBuffer < HEADER_LEN) {
		// read header
		int l = recv(socket, (char *)(buffer + bytesInBuffer), HEADER_LEN - bytesInBuffer, 0);
		if (l <= 0) {
			disconnect();
			return;
		}
		bytesInBuffer += l;
	}

	if (bytesInBuffer >= HEADER_LEN) {
		int packetSize = Decoder::getLength(buffer);
		if (packetSize < 2) {
			disconnect();
			return;
		}
		int l = recv(socket, (char *)(buffer + bytesInBuffer), HEADER_LEN + packetSize - bytesInBuffer, 0);
		if (l <= 0) {
			disconnect();
			return;
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
	decoder->createHeader((unsigned char *) bufs, (short) size);
	decoder->encrypt(buff, size);
	memcpy_s(bufs+4, size, buff, size);
	decoder->next();
	send(socket, (const char *) bufs, size+4, 0);
}

void PacketHandler::disconnect() {
	setDestroy();
}
