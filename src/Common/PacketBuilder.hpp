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

#define PACKET(TypeName, ...) \
	auto TypeName(__VA_ARGS__) -> PacketBuilder;

#define PACKET_IMPL(TypeName, ...) \
	auto TypeName(__VA_ARGS__) -> PacketBuilder

class PacketBuilder {
public:
	PacketBuilder();

	template <typename TValue>
	auto add(TValue value) -> PacketBuilder &;
	template <>
	auto add<bool>(bool value) -> PacketBuilder &;
	template <typename TValue>
	auto addVector(const vector_t<TValue> &vec) -> PacketBuilder &;
	template <typename TValue>
	auto addVector(const vector_t<TValue> &vec, size_t count) -> PacketBuilder &;

	template <typename TValue>
	auto addClass(const IPacketWritable &obj) -> PacketBuilder &;
	template <typename TValue>
	auto addClassVector(const vector_t<TValue> &vec) -> PacketBuilder &;
	template <typename TValue>
	auto addClassVector(const vector_t<TValue> &vec, size_t count) -> PacketBuilder &;
	template <typename TValue>
	auto set(TValue value, size_t pos) -> void;

	auto addString(const string_t &str) -> PacketBuilder &;
	auto addString(const string_t &str, size_t len) -> PacketBuilder &;
	auto addBytes(const char *hex) -> PacketBuilder &;
	auto addBuffer(const unsigned char *bytes, size_t len) -> PacketBuilder &;
	auto addBuffer(const PacketBuilder &builder) -> PacketBuilder &;
	auto addBuffer(const PacketReader &reader) -> PacketBuilder &;

	auto getBuffer() const -> const unsigned char *;
	auto getSize() const -> size_t;
	auto toString() const -> string_t;
private:
	static const size_t bufferLen = 1000; // Initial buffer length
	friend auto operator <<(std::ostream &out, const PacketBuilder &builder) -> std::ostream &;

	auto getBuffer(size_t pos, size_t len) -> unsigned char *;
	auto getHexByte(unsigned char input) -> unsigned char;

	size_t m_pos = 0;
	size_t m_packetCapacity = 0;
	MiscUtilities::shared_array<unsigned char> m_packet;
};

template <typename TValue>
auto PacketBuilder::add(TValue value) -> PacketBuilder & {
	(*(TValue *) getBuffer(m_pos, sizeof(TValue))) = value;
	m_pos += sizeof(TValue);
	return *this;
}

template <>
auto PacketBuilder::add<bool>(bool value) -> PacketBuilder & {
	return add<int8_t>(value ? 1 : 0);
}

template <typename TValue>
auto PacketBuilder::set(TValue value, size_t pos) -> void {
	(*(TValue *) getBuffer(pos, sizeof(TValue))) = value;
}

template <typename TValue>
auto PacketBuilder::addVector(const vector_t<TValue> &vec) -> PacketBuilder & {
	add<uint32_t>(vec.size());
	for (const auto &elem : vec) {
		add<TValue>(elem);
	}
	return *this;
}

template <typename TValue>
auto PacketBuilder::addVector(const vector_t<TValue> &vec, size_t count) -> PacketBuilder & {
	for (size_t iter = 0; iter < count; ++iter) {
		add<TValue>(vec[iter]);
	}
	return *this;
}

template <typename TValue>
auto PacketBuilder::addClass(const IPacketWritable &obj) -> PacketBuilder & {
	obj.write(*this);
	return *this;
}

template <typename TValue>
auto PacketBuilder::addClassVector(const vector_t<TValue> &vec) -> PacketBuilder & {
	add<uint32_t>(vec.size());
	for (const auto &elem : vec) {
		addClass<TValue>(elem);
	}
	return *this;
}

template <typename TValue>
auto PacketBuilder::addClassVector(const vector_t<TValue> &vec, size_t count) -> PacketBuilder & {
	for (size_t iter = 0; iter < count; ++iter) {
		addClass<TValue>(vec[iter]);
	}
	return *this;
}

inline
auto PacketBuilder::getBuffer() const -> const unsigned char * {
	return m_packet.get();
}

inline
auto PacketBuilder::addBuffer(const unsigned char *bytes, size_t len) -> PacketBuilder & {
	memcpy(getBuffer(m_pos, len), bytes, len);
	m_pos += len;
	return *this;
}

inline
auto PacketBuilder::getSize() const -> size_t {
	return m_pos;
}

inline
auto operator <<(std::ostream &out, const PacketBuilder &builder) -> std::ostream & {
	out << builder.toString();
	return out;
}