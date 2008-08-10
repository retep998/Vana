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
#include "Decoder.h"
#include "ReadPacket.h"
#include "PacketCreator.h"
#include <Winsock2.h>

PacketHandler::PacketHandler(int socket, AbstractPlayerFactory *abstractPlayerFactory, bool isSend, string ivUnknown) :
bytesInBuffer(0),
player(abstractPlayerFactory->createPlayer()),
decoder(new Decoder()),
socket(socket)
{
	if (isSend) {
		size_t len = 0;
		while (len != 2) {
			len += recv(socket, (char *) buffer, 2 - len, 0); // IV header indicating size
		}

		unsigned short ivLen = buffer[0] + buffer[1] * 0x100;

		len = 0;
		while (len != ivLen) {
			len += recv(socket, (char *) buffer, ivLen - len, 0);
		}
		ReadPacket packet(buffer, ivLen);

		unsigned short version = packet.getShort(); // Maple Version, TODO: Verify it
		packet.getString(); // Unknown

		unsigned char *rawpacket = packet.getBuffer();

		decoder->setIvSend(rawpacket);
		decoder->setIvRecv(rawpacket + 4);
	}
	else {
		size_t len;
		PacketCreator packet = decoder->getConnectPacket(ivUnknown);
		len = ::send(socket, (char *) packet.getBuffer(), packet.getSize(), 0);

		if (len < Decoder::CONNECT_LENGTH) {
			//TODO
		}
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
		if (bytesInBuffer == packetSize + HEADER_LEN) {
			decoder->decrypt(buffer + HEADER_LEN, packetSize);
			ReadPacket *packet = new ReadPacket(buffer + HEADER_LEN, packetSize);
			player->handleRequest(packet);
			delete packet;
			bytesInBuffer = 0;
		}
	}
}

bool PacketHandler::send(unsigned char *buff, int size) {
	if (getDestroy()) {
		return false;
	}
	else {
		unsigned char bufs[BUFFER_LEN];
		decoder->createHeader((unsigned char *) bufs, (short) size);
		decoder->encrypt(buff, size);
		memcpy_s(bufs+4, size, buff, size);
		decoder->next();
		::send(socket, (const char *) bufs, size+4, 0);
		return true;
	}
}

bool PacketHandler::send(const PacketCreator &packet) {
	unsigned char tempbuf[BUFFER_LEN];
	memcpy_s(tempbuf, BUFFER_LEN, packet.getBuffer(), BUFFER_LEN); // Copying to tempbuf so the packet doesn't get emptied on send and can be sent to other players
	
	return send(tempbuf, packet.getSize());
}

void PacketHandler::disconnect() {
	setDestroy();
}
