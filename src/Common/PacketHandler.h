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

#define HEADER_LEN 4
#define BUFFER_LEN 10000

#include "Selector.h"
#include <string>
#include <boost/scoped_ptr.hpp>

using std::string;

class AbstractPlayer;
class AbstractPlayerFactory;
class Decoder;
class PacketCreator;

class PacketHandler : public Selector::Handler {
public:
	PacketHandler(int socket, AbstractPlayerFactory *abstractPlayerFactory, bool isSend = false, string ivUnknown = ""); // isSend = packet is initiated by the server or not
	void handle(int socket);
	bool send(unsigned char *buf, int len);
	bool send(const PacketCreator &packet);
	void disconnect();

	AbstractPlayer * getPlayer() const { return player.get(); }
private:
	unsigned char buffer[BUFFER_LEN];
	int bytesInBuffer;
	boost::scoped_ptr<AbstractPlayer> player;
	boost::scoped_ptr<Decoder> decoder;
	int socket;
};

#endif
