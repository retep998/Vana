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
#include <boost/tr1/memory.hpp>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

using std::string;
using std::vector;

struct Pos;

class PacketReader {
public:
	PacketReader();
	PacketReader(unsigned char *buffer, size_t length);

	template<typename T>
	T get();
	template<typename T>
	vector<T> getVector();
	template<typename T>
	vector<T> getVector(size_t size);

	void skipBytes(int32_t len);
	header_t getHeader(bool advanceBuffer = true);
	string getString();
	string getString(size_t len);
	unsigned char * getBuffer() const;
	Pos getPos();
	bool getBool();
	size_t getBufferLength() const;
	size_t getSize() const { return m_length; }
	PacketReader & reset(int32_t len = 0);
	string toString() const;
private:
	friend std::ostream & operator <<(std::ostream &out, const PacketReader &packet);

	unsigned char *m_buffer;
	size_t m_length;
	size_t m_pos;
};

template<typename T>
T PacketReader::get() {
	if (sizeof(T) > getBufferLength()) {
		throw std::range_error("Packet data longer than buffer allows");
	}
	T val = (*(T *)(m_buffer + m_pos));
	m_pos += sizeof(T);
	return val;
}

template<typename T>
vector<T> PacketReader::getVector() {
	vector<T> vec;
	size_t size = get<uint32_t>();
	for (size_t i = 0; i < size; i++) {
		vec.push_back(get<T>());
	}
	return vec;
}

template<typename T>
vector<T> PacketReader::getVector(size_t size) {
	vector<T> vec;
	for (size_t i = 0; i < size; i++) {
		vec.push_back(get<T>());
	}
	return vec;
}

inline
std::ostream & operator <<(std::ostream &out, const PacketReader &packet) {
	out << packet.toString();
	return out;
}