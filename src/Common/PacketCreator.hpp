/*
Copyright (C) 2008-2014 Vana Development Team

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

#include "IPacket.hpp"
#include "shared_array.hpp"
#include "Types.hpp"
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

class PacketReader;

class PacketCreator {
public:
	PacketCreator();

	template <typename TValue>
	auto add(TValue value) -> void;
	template <>
	auto add<bool>(bool value) -> void;
	template <typename TValue>
	auto addVector(const vector_t<TValue> &vec) -> void;
	template <typename TValue>
	auto addClass(const IPacketWritable &obj) -> void;
	template <typename TValue>
	auto addClassVector(const vector_t<TValue> &vec) -> void;
	template <typename TValue>
	auto set(TValue value, size_t pos) -> void;

	auto addString(const string_t &str) -> void;
	auto addString(const string_t &str, size_t len) -> void;
	auto addBytes(const char *hex) -> void;
	auto addBuffer(const unsigned char *bytes, size_t len) -> void;
	auto addBuffer(const PacketCreator &packet) -> void;
	auto addBuffer(const PacketReader &packet) -> void;

	auto getBuffer() const -> const unsigned char *;
	auto getSize() const -> size_t;
	auto toString() const -> string_t;
private:
	static const size_t bufferLen = 1000; // Initial buffer length
	friend auto operator <<(std::ostream &out, const PacketCreator &packet) -> std::ostream &;

	auto getBuffer(size_t pos, size_t len) -> unsigned char *;
	auto getHexByte(unsigned char input) -> unsigned char;

	size_t m_pos = 0;
	size_t m_packetCapacity = 0;
	MiscUtilities::shared_array<unsigned char> m_packet;
};

template <typename TValue>
auto PacketCreator::add(TValue value) -> void {
	(*(TValue *) getBuffer(m_pos, sizeof(TValue))) = value;
	m_pos += sizeof(TValue);
}

template <>
auto PacketCreator::add<bool>(bool value) -> void {
	add<int8_t>(value ? 1 : 0);
}

template <typename TValue>
auto PacketCreator::set(TValue value, size_t pos) -> void {
	(*(TValue *) getBuffer(pos, sizeof(TValue))) = value;
}

template <typename TValue>
auto PacketCreator::addVector(const vector_t<TValue> &vec) -> void {
	add<uint32_t>(vec.size());
	for (const auto &iter : vec) {
		add(iter);
	}
}

template <typename TValue>
auto PacketCreator::addClass(const IPacketWritable &obj) -> void {
	obj.write(*this);
}

template <typename TValue>
auto PacketCreator::addClassVector(const vector_t<TValue> &vec) -> void {
	add<uint32_t>(vec.size());
	for (const auto &iter : vec) {
		addClass<TValue>(iter);
	}
}

inline
auto PacketCreator::getBuffer() const -> const unsigned char * {
	return m_packet.get();
}

inline
auto PacketCreator::addBuffer(const unsigned char *bytes, size_t len) -> void {
	memcpy(getBuffer(m_pos, len), bytes, len);
	m_pos += len;
}

inline
auto PacketCreator::getSize() const -> size_t {
	return m_pos;
}

inline
auto operator <<(std::ostream &out, const PacketCreator &packet) -> std::ostream & {
	out << packet.toString();
	return out;
}