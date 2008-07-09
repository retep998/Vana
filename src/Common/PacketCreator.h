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

using std::vector;
using std::string;

class ReadPacket;
struct Pos;

class Packet {
public:
	Packet() : pos(0) { }

	void addHeader(short headerid);
	void addInt(int intg);
	void setInt(int intg, size_t pos);
	void addInt64(__int64 int64);
	void setInt64(__int64 int64, size_t pos);
	void addShort(short shrt);
	void setShort(short shrt, size_t pos);
	void addString(const char *str, int slen);
	void addString(const string &str); // Dynamically-lengthed strings
	void addString(const string &str, int len); // Static-lengthed strings
	void addPos(Pos pos); // Positions
	void addByte(unsigned char byte);
	void setByte(unsigned char byte, size_t pos);
	void addBytes(char *hex);
	void addBuffer(unsigned char *bytes, int len);
	void addBuffer(ReadPacket *packet);

	unsigned char * getBuffer();
	size_t getSize();

	template <class T>
	void sendTo(T *player, vector <T*> players, bool is);
	template <class T>
	void send(T *player);
private:
	size_t pos;
	unsigned char packet[MAX_LEN];
};

inline
void Packet::addHeader(short headerid) {
	addShort(headerid);
}

inline
void Packet::addInt(int intg) {
	(*(int*)(packet+pos)) = intg;
	pos += 4;
}

inline
void Packet::setInt(int intg, size_t pos) {
	(*(int*)(packet+pos)) = intg;
}

inline
void Packet::addInt64(__int64 int64) {
	(*(__int64*)(packet+pos)) = int64;
	pos += 8;
}

inline
void Packet::setInt64(__int64 int64, size_t pos) {
	(*(__int64*)(packet+pos)) = int64;
}

inline
void Packet::addShort(short shrt) {
	(*(short*)(packet+pos)) = shrt;
	pos += 2;
}

inline
void Packet::setShort(short shrt, size_t pos) {
	(*(short*)(packet+pos)) = shrt;
}

inline
void Packet::addString(const char *str, int slen) {
	int rlen = strlen(str);
	strncpy_s((char*)packet+pos, slen+1, str, slen);
	for (int i = rlen; i < slen; i++)
		packet[pos+i] = 0;
	pos += slen;
}

inline
void Packet::addString(const string &str) {
	int len = str.size();
	addShort(len);
	strcpy_s((char *) packet + pos, MAX_LEN - pos, str.c_str());
	pos += len;
}

inline
void Packet::addString(const string &str, int len) {
	int slen = str.size();
	if (len < slen) {
		std::cout << "ERROR: addString used with lenth shorter than string size." << std::endl; // TODO: Throw exception
	}
	strncpy_s((char *) packet + pos, MAX_LEN -pos, str.c_str(), slen);
	for (int i = slen; i < len; i++) {
		packet[pos+i] = 0;
	}
	pos += len;
}

inline
void Packet::addByte(unsigned char byte) {
	packet[pos++] = byte;
}

inline
void Packet::setByte(unsigned char byte, size_t pos) {
	packet[pos] = byte;
}

inline
void Packet::addBytes(char *hex) {
	for (int i=0; i<(int)strlen(hex)/2; i++) {
		unsigned char byte1 = hex[i*2];
		unsigned char byte2 = hex[i*2+1];
		if (byte1 >= 'A' && byte1 <= 'F')
			byte1 -= 'A' - 0xa;
		else if (byte1 >= 'a' && byte1 <= 'f')
			byte1 -= 'a' - 0xa;
		else if (byte1 >= '0' && byte1 <= '9')
			byte1 -= '0';
		if (byte2 >= 'A' && byte2 <= 'F')
			byte2 -= 'A' - 0xa;
		else if (byte2 >= 'a' && byte2 <= 'f')
			byte2 -= 'a' - 0xa;
		else if (byte2 >= '0' && byte2 <= '9')
			byte2 -= '0';
		unsigned char byte = byte1*0x10 + byte2;
		packet[pos++] = byte;	
	}
}

inline
void Packet::addBuffer(unsigned char *bytes, int len) {
	memcpy_s(packet+pos, len, bytes, len);
	pos += len;
}

inline
unsigned char * Packet::getBuffer() {
	return packet;
}

inline
size_t Packet::getSize() {
	return pos;
}

template <class T>
void Packet::sendTo(T *player, vector <T*> players, bool is) {
	for (unsigned int i=0; i<players.size(); i++) {
		if ((player != NULL && player->getPlayerid() != players[i]->getPlayerid() && !is) || is)
			this->send(players[i]);
	}
}

template <class T>
void Packet::send(T *player) {
	unsigned char tempbuf[MAX_LEN];
	memcpy_s(tempbuf, MAX_LEN, packet, MAX_LEN); // Copying to tempbuf so the packet doesn't get emptied on send and can be sent to other players
	player->sendPacket(tempbuf, pos);
}

#endif
