/*
Copyright (C) 2008-2015 Vana Development Team

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

DEFAULT_EXCEPTION(PacketContentException, std::runtime_error);

#ifdef DEBUG
DEFAULT_EXCEPTION(PacketDebugException, std::runtime_error);
#endif

class PacketReader {
public:
	PacketReader() = default;
	PacketReader(unsigned char *buffer, size_t length);

	template <typename TValue>
	auto get() -> TValue;
	template <typename TValue>
	auto unk() -> TValue;
	template <typename TValue>
	auto get(size_t size) -> TValue;
	template <typename TValue>
	auto peek() -> TValue;
	template <typename TValue>
	auto skip() -> PacketReader &;

	auto skip(int32_t len) -> PacketReader &;
	auto getBuffer() const -> unsigned char *;
	auto getBufferLength() const -> size_t;
	auto getConsumedLength() const -> size_t;
	auto reset(int32_t len = 0) -> PacketReader &;
	auto toString() const -> string_t;
private:
	friend auto operator <<(std::ostream &out, const PacketReader &reader) -> std::ostream &;
	auto getSize() const -> size_t { return m_length; }

	template <typename TValue>
	auto skipImpl(TValue *) -> void;
	template <>
	auto skipImpl<bool>(bool *) -> void;
	template <>
	auto skipImpl<string_t>(string_t *) -> void;
	template <typename TElement>
	auto skipImpl(vector_t<TElement> *) -> void;

	template <typename TValue>
	auto getImpl(TValue *) -> TValue;
	template <typename TValue>
	auto getSizedImpl(size_t size, TValue *) -> TValue;

	template <typename TValue>
	auto getImplDefault() -> TValue;
	template <>
	auto getImpl<bool>(bool *) -> bool;
	template <>
	auto getImpl<double>(double *) -> double;
	template <>
	auto getImpl<string_t>(string_t *) -> string_t;
	template <>
	auto getImpl<int8_t>(int8_t *) -> int8_t;
	template <>
	auto getImpl<int16_t>(int16_t *) -> int16_t;
	template <>
	auto getImpl<int32_t>(int32_t *) -> int32_t;
	template <>
	auto getImpl<int64_t>(int64_t *) -> int64_t;
	template <>
	auto getImpl<uint8_t>(uint8_t *) -> uint8_t;
	template <>
	auto getImpl<uint16_t>(uint16_t *) -> uint16_t;
	template <>
	auto getImpl<uint32_t>(uint32_t *) -> uint32_t;
	template <>
	auto getImpl<uint64_t>(uint64_t *) -> uint64_t;
	template <>
	auto getImpl<milliseconds_t>(milliseconds_t *) -> milliseconds_t;
	template <>
	auto getImpl<seconds_t>(seconds_t *) -> seconds_t;
	template <>
	auto getImpl<minutes_t>(minutes_t *) -> minutes_t;
	template <>
	auto getImpl<hours_t>(hours_t *) -> hours_t;
	template <typename TElement>
	auto getImpl(vector_t<TElement> *) -> vector_t<TElement>;

	template <>
	auto getSizedImpl<string_t>(size_t size, string_t *) -> string_t;
	template <typename TElement>
	auto getSizedImpl(size_t size, vector_t<TElement> *) -> vector_t<TElement>;

	unsigned char *m_buffer = nullptr;
	size_t m_length = 0;
	size_t m_pos = 0;
};

template <typename TValue>
auto PacketReader::get() -> TValue {
	auto v = getImpl(static_cast<TValue *>(nullptr));
	return v;
}

template <typename TValue>
auto PacketReader::unk() -> TValue {
	auto v = getImpl(static_cast<TValue *>(nullptr));
	return v;
}

template <typename TValue>
auto PacketReader::get(size_t size) -> TValue {
	auto v = getSizedImpl(size, static_cast<TValue *>(nullptr));
	return v;
}

template <typename TValue>
auto PacketReader::peek() -> TValue {
	size_t pos = getConsumedLength();
	auto val = get<TValue>();
	m_pos = pos;
	return val;
}

template <typename TValue>
auto PacketReader::skip() -> PacketReader & {
	skipImpl(static_cast<TValue *>(nullptr));
	return *this;
}

template <typename TValue>
auto PacketReader::skipImpl(TValue *) -> void {
	m_pos += sizeof(TValue);
}

template <>
auto PacketReader::skipImpl<bool>(bool *) -> void {
	m_pos += 1;
}

template <>
auto PacketReader::skipImpl<string_t>(string_t *) -> void {
	size_t length = get<uint16_t>();
	m_pos += length;
}

template <typename TElement>
auto PacketReader::skipImpl(vector_t<TElement> *) -> void {
	size_t length = get<uint32_t>();
	m_pos += length * sizeof(TElement);
}

template <typename TValue>
auto PacketReader::getImpl(TValue *) -> TValue {
	PacketSerialize<TValue> x;
	return x.read(*this);
}

template <typename TValue>
auto PacketReader::getSizedImpl(size_t size, TValue *) -> TValue {
	static_assert(false, "T is not appropriately specialized for that type");
	throw std::logic_error("T is not appropriately specialized for that type");
}

template <typename TValue>
auto PacketReader::getImplDefault() -> TValue {
	if (sizeof(TValue) > getBufferLength()) {
		throw PacketContentException("Packet data longer than buffer allows");
	}
	TValue val = *reinterpret_cast<TValue *>(m_buffer + m_pos);
	m_pos += sizeof(TValue);
	return val;
}

template <>
auto PacketReader::getImpl<bool>(bool *) -> bool {
#ifdef DEBUG
	// Address programming errors in debug
	int8_t byte = getImplDefault<int8_t>();
	if (byte != 0 && byte != 1) throw PacketDebugException("Packet data inconsistent with bool type");
	return byte != 0;
#else
	return getImplDefault<int8_t>() != 0;
#endif
}

template <>
auto PacketReader::getImpl<double>(double *) -> double {
	return getImplDefault<double>();
}

template <>
auto PacketReader::getImpl<int8_t>(int8_t *) -> int8_t {
	return getImplDefault<int8_t>();
}

template <>
auto PacketReader::getImpl<int16_t>(int16_t *) -> int16_t {
	return getImplDefault<int16_t>();
}

template <>
auto PacketReader::getImpl<int32_t>(int32_t *) -> int32_t {
	return getImplDefault<int32_t>();
}

template <>
auto PacketReader::getImpl<int64_t>(int64_t *) -> int64_t {
	return getImplDefault<int64_t>();
}

template <>
auto PacketReader::getImpl<uint8_t>(uint8_t *) -> uint8_t {
	return getImplDefault<uint8_t>();
}

template <>
auto PacketReader::getImpl<uint16_t>(uint16_t *) -> uint16_t {
	return getImplDefault<uint16_t>();
}

template <>
auto PacketReader::getImpl<uint32_t>(uint32_t *) -> uint32_t {
	return getImplDefault<uint32_t>();
}

template <>
auto PacketReader::getImpl<uint64_t>(uint64_t *) -> uint64_t {
	return getImplDefault<uint64_t>();
}

template <>
auto PacketReader::getImpl<milliseconds_t>(milliseconds_t *) -> milliseconds_t {
	return milliseconds_t(getImplDefault<int32_t>());
}

template <>
auto PacketReader::getImpl<seconds_t>(seconds_t *) -> seconds_t {
	return seconds_t(getImplDefault<int32_t>());
}

template <>
auto PacketReader::getImpl<minutes_t>(minutes_t *) -> minutes_t {
	return minutes_t(getImplDefault<int32_t>());
}

template <>
auto PacketReader::getImpl<hours_t>(hours_t *) -> hours_t {
	return hours_t(getImplDefault<int32_t>());
}

template <>
auto PacketReader::getImpl<string_t>(string_t *) -> string_t {
	size_t size = getImplDefault<uint16_t>();
	return get<string_t>(size);
}

template <typename TElement>
auto PacketReader::getImpl(vector_t<TElement> *) -> vector_t<TElement> {
	size_t size = getImplDefault<uint32_t>();
	return get<vector_t<TElement>>(size);
}

template <>
auto PacketReader::getSizedImpl<string_t>(size_t size, string_t *) -> string_t {
	if (size > getBufferLength()) {
		throw PacketContentException("Packet string longer than buffer allows");
	}
	string_t s{reinterpret_cast<char *>(m_buffer + m_pos), size};
	m_pos += size;
	return s;
}

template <typename TElement>
auto PacketReader::getSizedImpl(size_t size, vector_t<TElement> *) -> vector_t<TElement> {
	vector_t<TElement> vec;
	for (size_t i = 0; i < size; i++) {
		vec.push_back(get<TElement>());
	}
	return vec;
}

inline
auto operator <<(std::ostream &out, const PacketReader &reader) -> std::ostream & {
	out << reader.toString();
	return out;
}