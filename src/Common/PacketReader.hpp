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
#include "Types.hpp"
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

class PacketContentException : public std::runtime_error {
	DEFAULT_EXCEPTION(PacketContentException, std::runtime_error);
};

class PacketReader {
public:
	PacketReader() = default;
	PacketReader(unsigned char *buffer, size_t length);

	template <typename TValue>
	auto get() -> TValue;
	template <>
	auto get<bool>() -> bool;
	template <typename TValue>
	auto getVector() -> vector_t<TValue>;
	template <typename TValue>
	auto getVector(size_t size) -> vector_t<TValue>;
	template <typename TValue>
	auto getClass() -> TValue;
	template <typename TValue>
	auto getClassVector() -> vector_t<TValue>;
	template <typename TValue>
	auto getClassVector(size_t size) -> vector_t<TValue>;

	auto skipBytes(int32_t len) -> void;
	auto getHeader(bool advanceBuffer = true) -> header_t;
	auto getString() -> string_t;
	auto getString(size_t len) -> string_t;
	auto getBuffer() const -> unsigned char *;
	auto getBufferLength() const -> size_t;
	auto getConsumedLength() const -> size_t;
	auto reset(int32_t len = 0) -> PacketReader &;
	auto toString() const -> string_t;
private:
	auto getSize() const -> size_t { return m_length; }
	friend auto operator <<(std::ostream &out, const PacketReader &packet) -> std::ostream &;

	unsigned char *m_buffer = nullptr;
	size_t m_length = 0;
	size_t m_pos = 0;
};

template <typename TValue>
auto PacketReader::get() -> TValue {
	if (sizeof(TValue) > getBufferLength()) {
		throw PacketContentException("Packet data longer than buffer allows");
	}
	TValue val = (*(TValue *)(m_buffer + m_pos));
	m_pos += sizeof(TValue);
	return val;
}

#ifdef DEBUG
class PacketDebugException : public std::runtime_error {
	DEFAULT_EXCEPTION(PacketDebugException, std::runtime_error);
};
#endif

template <>
auto PacketReader::get<bool>() -> bool {
#ifdef DEBUG
	// Address programming errors in debug
	int8_t byte = get<int8_t>();
	if (byte != 0 && byte != 1) throw PacketDebugException("Packet data inconsistent with bool type");
	return byte != 0;
#else
	return get<int8_t>() != 0;
#endif
}

template <typename TValue>
auto PacketReader::getVector() -> vector_t<TValue> {
	size_t size = get<uint32_t>();
	return getVector<TValue>(size);
}

template <typename TValue>
auto PacketReader::getVector(size_t size) -> vector_t<TValue> {
	vector_t<TValue> vec;
	for (size_t i = 0; i < size; i++) {
		vec.push_back(get<TValue>());
	}
	return vec;
}

template <typename TValue>
auto PacketReader::getClass() -> TValue {
	TValue obj;
	obj.read(*this);
	return obj;
}

template <typename TValue>
auto PacketReader::getClassVector() -> vector_t<TValue> {
	size_t size = get<uint32_t>();
	return getClassVector<TValue>(size);
}

template <typename TValue>
auto PacketReader::getClassVector(size_t size) -> vector_t<TValue> {
	vector_t<TValue> vec;
	for (size_t i = 0; i < size; i++) {
		vec.push_back(getClass<TValue>());
	}
	return vec;
}

inline
auto operator <<(std::ostream &out, const PacketReader &packet) -> std::ostream & {
	out << packet.toString();
	return out;
}