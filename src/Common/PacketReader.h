/*
Copyright (C) 2008-2013 Vana Development Team

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

#include "IPacket.h"
#include "Types.h"
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

using std::string;
using std::vector;

class PacketReader {
public:
	PacketReader();
	PacketReader(unsigned char *buffer, size_t length);

	template <typename T> T get();
	template <> bool get<bool>();
	template <typename T> vector<T> getVector();
	template <typename T> vector<T> getVector(size_t size);
	template <typename T> T getClass();
	template <typename T> vector<T> getClassVector();
	template <typename T> vector<T> getClassVector(size_t size);

	void skipBytes(int32_t len);
	header_t getHeader(bool advanceBuffer = true);
	string getString();
	string getString(size_t len);
	unsigned char * getBuffer() const;
	size_t getBufferLength() const;
	PacketReader & reset(int32_t len = 0);
	string toString() const;
private:
	size_t getSize() const { return m_length; }
	friend std::ostream & operator <<(std::ostream &out, const PacketReader &packet);

	unsigned char *m_buffer;
	size_t m_length;
	size_t m_pos;
};

template <typename T>
T PacketReader::get() {
	if (sizeof(T) > getBufferLength()) {
		throw std::range_error("Packet data longer than buffer allows");
	}
	T val = (*(T *)(m_buffer + m_pos));
	m_pos += sizeof(T);
	return val;
}

template <>
bool PacketReader::get<bool>() {
	return (get<int8_t>() != 0);
}

template <typename T>
vector<T> PacketReader::getVector() {
	size_t size = get<uint32_t>();
	return getVector<T>(size);
}

template <typename T>
vector<T> PacketReader::getVector(size_t size) {
	vector<T> vec;
	for (size_t i = 0; i < size; i++) {
		vec.push_back(get<T>());
	}
	return vec;
}

template <typename T>
T PacketReader::getClass() {
	T obj;
	obj.read(*this);
	return obj;
}

template <typename T>
vector<T> PacketReader::getClassVector() {
	size_t size = get<uint32_t>();
	return getClassVector<T>(size);
}

template <typename T>
vector<T> PacketReader::getClassVector(size_t size) {
	vector<T> vec;
	for (size_t i = 0; i < size; i++) {
		vec.push_back(getClass<T>());
	}
	return vec;
}

inline
std::ostream & operator <<(std::ostream &out, const PacketReader &packet) {
	out << packet.toString();
	return out;
}