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
#include "PacketCreator.h"
#include "Pos.h"
#include "ReadPacket.h"
#include "StringUtilities.h"
#include <boost/tokenizer.hpp>

void Packet::addPos(Pos pos) {
	addShort(pos.x);
	addShort(pos.y);
}

void Packet::addBuffer(ReadPacket *packet) {
	addBuffer(packet->getBuffer(), packet->getBufferLength());
}

void Packet::addIP(const string &ip) { 
	typedef boost::tokenizer<boost::char_separator<char>> tokenizer;
	typedef boost::char_separator<char> separator;

	separator sep(".");
	tokenizer tokens(ip, sep);

	for (tokenizer::iterator iter = tokens.begin(); iter != tokens.end(); iter++) {
		addByte((unsigned char) StringUtilities::toType<short>(*iter)); // Using toType<unsigned char> would cause it to spit ASCII
	}
}
