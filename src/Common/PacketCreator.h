/*
Copyright (C) 2008-2011 Vana Development Team

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
#pragma once

#include "Types.h"
#include <boost/shared_array.hpp>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

using boost::shared_array;
using std::string;
using std::vector;

class PacketReader;
struct Pos;

class PacketCreator {
public:
	PacketCreator();

	template <typename T>
	void add(T value);
	template <typename T>
	void set(T value, size_t pos);
	template <typename T>
	void addVector(const vector<T> &vec);
	void addHeader(header_t value);
	void addString(const string &str); // Dynamically-lengthed strings
	void addString(const string &str, size_t len); // Static-lengthed strings
	void addPos(const Pos &pos); // Positions
	void addPos(const Pos &pos, bool fullInts); // Positions
	void addBool(bool value);
	void addBytes(const char *hex);
	void addBuffer(const unsigned char *bytes, size_t len);
	void addBuffer(PacketCreator &packet);
	void addBuffer(PacketReader &packet);

	const unsigned char * getBuffer() const;
	size_t getSize() const;
	string toString() const;
private:
	static const size_t bufferLen = 1000; // Initial buffer length
	friend std::ostream & operator <<(std::ostream &out, const PacketCreator &packet);

	unsigned char * getBuffer(size_t pos, size_t len);
	unsigned char getHexByte(unsigned char input);

	size_t m_pos;
	shared_array<unsigned char> m_packet;
	size_t m_packetCapacity;
};

template <typename T>
void PacketCreator::add(T value) {
	(*(T *) getBuffer(m_pos, sizeof(T))) = value;
	m_pos += sizeof(T);
}

template <typename T>
void PacketCreator::set(T value, size_t pos) {
	(*(T *) getBuffer(pos, sizeof(T))) = value;
}

template <typename T>
void PacketCreator::addVector(const vector<T> &vec) {
	add<uint32_t>(vec.size());
	for (typename vector<T>::const_iterator iter = vec.begin(); iter != vec.end(); ++iter) {
		add(*iter);
	}
}

inline
const unsigned char * PacketCreator::getBuffer() const {
	return m_packet.get();
}

inline
void PacketCreator::addBuffer(const unsigned char *bytes, size_t len) {
	memcpy(getBuffer(m_pos, len), bytes, len);
	m_pos += len;
}

inline
size_t PacketCreator::getSize() const {
	return m_pos;
}

inline
std::ostream & operator <<(std::ostream &out, const PacketCreator &packet) {
	out << packet.toString();
	return out;
}