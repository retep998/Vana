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
#ifndef PACKETHANDLER_H
#define PACKETHANDLER_H

#include "../Decoder/Decoder.h"
#include "../Decoder/MapleEncryption.h"
#include "Selector.h"
#include "AbstractPlayer.h"

class PacketHandler: public Selector::SelectHandler {
public:
	PacketHandler(int socket, AbstractPlayer* player);
	void handle (int socket);
	void sendPacket(unsigned char* buf, int len);
	void disconnect();
private:
	unsigned char* buffer;
	int bytesInBuffer;
	AbstractPlayer* player;
	Decoder* decoder;
	int socket;
};

#endif