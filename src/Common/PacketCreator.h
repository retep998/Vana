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
#ifndef PACKETCREATOR_H
#define PACKETCREATOR_H

#define MAX_LEN 10000

#include <vector>
#include <string>
#include <iostream>

using std::string;
using std::vector;

class ReadPacket;
struct Pos;

class PacketCreator {
public:
	PacketCreator() : pos(0) { }

	void addHeader(short headerid);
	void addInt(int intg);
	void setInt(int intg, size_t pos);
	void addInt64(__int64 int64);
	void setInt64(__int64 int64, size_t pos);
	void addShort(short shrt);
	void setShort(short shrt, size_t pos);
	void addString(const string &str); // Dynamically-lengthed strings
	void addString(const string &str, size_t len); // Static-lengthed strings
	void addPos(Pos pos); // Positions
	void addByte(unsigned char byte);
	void setByte(unsigned char byte, size_t pos);
	void addBytes(char *hex);
	void addBuffer(unsigned char *bytes, size_t len);
	void addBuffer(ReadPacket *packet);
	void addIP(const string &ip);

	unsigned char * getBuffer();
	size_t getSize();

	template <class T>
	void send(T *player);
private:
	size_t pos;
	unsigned char packet[MAX_LEN];
};

inline
void PacketCreator::addHeader(short headerid) {
	addShort(headerid);
}

inline
void PacketCreator::addInt(int intg) {
	(*(int*)(packet+pos)) = intg;
	pos += 4;
}

inline
void PacketCreator::setInt(int intg, size_t pos) {
	(*(int*)(packet+pos)) = intg;
}

inline
void PacketCreator::addInt64(__int64 int64) {
	(*(__int64*)(packet+pos)) = int64;
	pos += 8;
}

inline
void PacketCreator::setInt64(__int64 int64, size_t pos) {
	(*(__int64*)(packet+pos)) = int64;
}

inline
void PacketCreator::addShort(short shrt) {
	(*(short*)(packet+pos)) = shrt;
	pos += 2;
}

inline
void PacketCreator::setShort(short shrt, size_t pos) {
	(*(short*)(packet+pos)) = shrt;
}

inline
void PacketCreator::addByte(unsigned char byte) {
	packet[pos++] = byte;
}

inline
void PacketCreator::setByte(unsigned char byte, size_t pos) {
	packet[pos] = byte;
}

inline
unsigned char * PacketCreator::getBuffer() {
	return packet;
}

inline
size_t PacketCreator::getSize() {
	return pos;
}

template <class T>
void PacketCreator::send(T *player) {
	unsigned char tempbuf[MAX_LEN];
	memcpy_s(tempbuf, MAX_LEN, packet, MAX_LEN); // Copying to tempbuf so the packet doesn't get emptied on send and can be sent to other players
	player->sendPacket(tempbuf, pos);
}

#endif
