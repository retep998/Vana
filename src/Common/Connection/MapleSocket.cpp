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

#include "MapleSocket.h"
#include "AbstractPlayer.h"
#include "RecvHeader.h"
#include <iostream>
#include <string>

MapleSocket::MapleSocket(ISocketHandler &h) : TcpSocket(h) {
	ready = false;
	bytesInBuffer = 0;
	buffer = new unsigned char[BUFFER_LEN];
	decoder = new Decoder();
	SetDeleteByHandler();
}

Socket * MapleSocket::Create() {
	MapleSocket * sock = new MapleSocket(Handler());
	sock->SetAbstractPlayerFactory(abstractPlayerFactory);
	return dynamic_cast<Socket *>(sock);
}

void MapleSocket::OnAccept() {
	std::cout << "Accepted connection." << std::endl;
	SetTcpNodelay();
	player = abstractPlayerFactory->createPlayer();
	player->setSocket(this);
	player->setIP(this->GetRemoteAddress().c_str());
	SendBuf((const char *) decoder->getConnectPacket(), Decoder::CONNECT_LENGTH);
	ready = true;
}

void MapleSocket::OnConnect() {
	SetTcpNodelay();
	player = abstractPlayerFactory->createPlayer();
	player->setSocket(this);
	player->setIP(this->GetRemoteAddress().c_str());
	ready = true;
}

void MapleSocket::OnRead() {
	TcpSocket::OnRead();
	size_t n = ibuf.GetLength();

	if (bytesInBuffer < HEADER_LEN) { // Read header
		size_t left = HEADER_LEN - bytesInBuffer;
		size_t read = (n < left) ? n : left;
		ibuf.Read((char *) buffer + bytesInBuffer, read);
		bytesInBuffer += read;
	}

	if ((short) buffer == RECV_IV) { // IV Packet
		ibuf.Read((char *) buffer + HEADER_LEN, Decoder::CONNECT_LENGTH - HEADER_LEN);
		decoder->setIvSend(buffer+6);
		decoder->setIvRecv(buffer+10);
	}

	if (bytesInBuffer >= HEADER_LEN) {
		int packetSize = Decoder::getLength(buffer);

		if (packetSize < 2) {
			SetCloseAndDelete();
		}

		size_t left = HEADER_LEN + packetSize - bytesInBuffer;
		size_t read = (n < left) ? n : left;
		ibuf.Read((char *) buffer + bytesInBuffer, read);
		bytesInBuffer += read;

		if (bytesInBuffer == packetSize + HEADER_LEN){
			decoder->decrypt(buffer + HEADER_LEN, packetSize);
			player->handleRequest(buffer + HEADER_LEN, packetSize);
			bytesInBuffer = 0;
		}
	}
}

void MapleSocket::OnDelete() {
	delete player;
}

void MapleSocket::SendEncrypted(unsigned char *buf, short len) {
	unsigned char encrypted[BUFFER_LEN];
	decoder->createHeader((unsigned char *) encrypted, len);
	decoder->encrypt(buf, len);
	memcpy_s(encrypted+4, BUFFER_LEN-4, buf, len);
	decoder->next();
	SendBuf((const char *) encrypted, len + HEADER_LEN);
}
