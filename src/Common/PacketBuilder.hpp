/*
Copyright (C) 2008-2016 Vana Development Team

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

#include "Common/IPacket.hpp"
#include "Common/shared_array.hpp"
#include "Common/Types.hpp"
#include <cstring>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

#define PACKET(TypeName, ...) \
	auto TypeName(__VA_ARGS__) -> PacketBuilder;

#define PACKET_IMPL(TypeName, ...) \
	auto TypeName(__VA_ARGS__) -> PacketBuilder

namespace Vana {
	class PacketReader;

	class PacketBuilder {
	public:
		PacketBuilder();

		template <typename TValue>
		auto add(const TValue &value) -> PacketBuilder &;
		template <typename TValue>
		auto unk() -> PacketBuilder &;
		// Used in conjunction with packet information that's intended to be set later
		template <typename TValue>
		auto defer() -> PacketBuilder &;
		template <typename TValue>
		auto unk(const TValue &value) -> PacketBuilder &;
		auto unk(int32_t bytes) -> PacketBuilder &;

		template <typename TValue>
		auto add(const TValue &value, size_t count) -> PacketBuilder &;
		template <typename TValue>
		auto set(const TValue &value, size_t pos) -> PacketBuilder &;

		auto addBytes(const string_t &hex) -> PacketBuilder &;
		auto addBytes(const char *hex) -> PacketBuilder &;
		auto addBuffer(const unsigned char *bytes, size_t len) -> PacketBuilder &;
		auto addBuffer(const PacketBuilder &builder) -> PacketBuilder &;
		auto addBuffer(const PacketReader &reader) -> PacketBuilder &;

		auto getBuffer() const -> const unsigned char *;
		auto getSize() const -> size_t;
		auto toString() const -> string_t;
	private:
		static const size_t bufferLen = 100; // Initial buffer length
		friend auto operator <<(std::ostream &out, const PacketBuilder &builder) -> std::ostream &;

		auto getBuffer(size_t pos, size_t len) -> unsigned char *;
		auto getHexByte(unsigned char input) -> unsigned char;

		template <typename TValue>
		auto addImpl(const TValue &val) -> void;
		template <typename TValue>
		auto addSizedImpl(const TValue &val, size_t size) -> void;

		template <typename TValue>
		auto addImplDefault(const TValue &value) -> void;
		template <>
		auto addImpl<bool>(const bool &val) -> void;
		template <>
		auto addImpl<double>(const double &val) -> void;
		template <>
		auto addImpl<string_t>(const string_t &val) -> void;
		template <>
		auto addImpl<int8_t>(const int8_t &val) -> void;
		template <>
		auto addImpl<int16_t>(const int16_t &val) -> void;
		template <>
		auto addImpl<int32_t>(const int32_t &val) -> void;
		template <>
		auto addImpl<int64_t>(const int64_t &val) -> void;
		template <>
		auto addImpl<uint8_t>(const uint8_t &val) -> void;
		template <>
		auto addImpl<uint16_t>(const uint16_t &val) -> void;
		template <>
		auto addImpl<uint32_t>(const uint32_t &val) -> void;
		template <>
		auto addImpl<uint64_t>(const uint64_t &val) -> void;
		template <>
		auto addImpl<milliseconds_t>(const milliseconds_t &val) -> void;
		template <>
		auto addImpl<seconds_t>(const seconds_t &val) -> void;
		template <>
		auto addImpl<minutes_t>(const minutes_t &val) -> void;
		template <>
		auto addImpl<hours_t>(const hours_t &val) -> void;
		template <typename TElement>
		auto addImpl(const vector_t<TElement> &val) -> void;

		template <>
		auto addSizedImpl<string_t>(const string_t &val, size_t size) -> void;
		template <typename TElement>
		auto addSizedImpl(const vector_t<TElement> &val, size_t size) -> void;

		size_t m_pos = 0;
		size_t m_packetCapacity = 0;
		MiscUtilities::shared_array<unsigned char> m_packet;
	};

	template <typename TValue>
	auto PacketBuilder::add(const TValue &value) -> PacketBuilder & {
		addImpl(value);
		return *this;
	}

	template <typename TValue>
	auto PacketBuilder::unk() -> PacketBuilder & {
		addImpl(TValue{});
		return *this;
	}

	template <typename TValue>
	auto PacketBuilder::defer() -> PacketBuilder & {
		addImpl(TValue{});
		return *this;
	}

	template <typename TValue>
	auto PacketBuilder::unk(const TValue &value) -> PacketBuilder & {
		addImpl(value);
		return *this;
	}

	template <typename TValue>
	auto PacketBuilder::add(const TValue &value, size_t size) -> PacketBuilder & {
		addSizedImpl(value, size);
		return *this;
	}

	template <typename TValue>
	auto PacketBuilder::addImpl(const TValue &val) -> void {
		PacketSerialize<TValue> x;
		return x.write(*this, val);
	}

	template <typename TValue>
	auto PacketBuilder::addSizedImpl(const TValue &value, size_t size) -> void {
		static_assert(false, "T is not appropriately specialized for that type");
		throw std::logic_error{"T is not appropriately specialized for that type"};
	}

	template <typename TValue>
	auto PacketBuilder::addImplDefault(const TValue &value) -> void {
		*reinterpret_cast<TValue *>(getBuffer(m_pos, sizeof(TValue))) = value;
		m_pos += sizeof(TValue);
	}

	template <typename TValue>
	auto PacketBuilder::set(const TValue &value, size_t pos) -> PacketBuilder & {
		*reinterpret_cast<TValue *>(getBuffer(pos, sizeof(TValue))) = value;
		return *this;
	}

	template <>
	auto PacketBuilder::addImpl<bool>(const bool &value) -> void {
		addImplDefault<int8_t>(value ? 1 : 0);
	}

	template <>
	auto PacketBuilder::addImpl<double>(const double &value) -> void {
		addImplDefault<double>(value);
	}

	template <>
	auto PacketBuilder::addImpl<int8_t>(const int8_t &value) -> void {
		addImplDefault<int8_t>(value);
	}

	template <>
	auto PacketBuilder::addImpl<int16_t>(const int16_t &value) -> void {
		addImplDefault<int16_t>(value);
	}

	template <>
	auto PacketBuilder::addImpl<int32_t>(const int32_t &value) -> void {
		addImplDefault<int32_t>(value);
	}

	template <>
	auto PacketBuilder::addImpl<int64_t>(const int64_t &value) -> void {
		addImplDefault<int64_t>(value);
	}

	template <>
	auto PacketBuilder::addImpl<uint8_t>(const uint8_t &value) -> void {
		addImplDefault<uint8_t>(value);
	}

	template <>
	auto PacketBuilder::addImpl<uint16_t>(const uint16_t &value) -> void {
		addImplDefault<uint16_t>(value);
	}

	template <>
	auto PacketBuilder::addImpl<uint32_t>(const uint32_t &value) -> void {
		addImplDefault<uint32_t>(value);
	}

	template <>
	auto PacketBuilder::addImpl<uint64_t>(const uint64_t &value) -> void {
		addImplDefault<uint64_t>(value);
	}

	template <>
	auto PacketBuilder::addImpl<milliseconds_t>(const milliseconds_t &value) -> void {
		addImplDefault<int32_t>(static_cast<int32_t>(value.count()));
	}

	template <>
	auto PacketBuilder::addImpl<seconds_t>(const seconds_t &value) -> void {
		addImplDefault<int32_t>(static_cast<int32_t>(value.count()));
	}

	template <>
	auto PacketBuilder::addImpl<minutes_t>(const minutes_t &value) -> void {
		addImplDefault<int32_t>(static_cast<int32_t>(value.count()));
	}

	template <>
	auto PacketBuilder::addImpl<hours_t>(const hours_t &value) -> void {
		addImplDefault<int32_t>(static_cast<int32_t>(value.count()));
	}

	template <>
	auto PacketBuilder::addImpl<string_t>(const string_t &value) -> void {
		if (value.size() > static_cast<size_t>(std::numeric_limits<uint16_t>::max())) throw std::invalid_argument{"String is too large to be sent via packet"};
		addImplDefault<uint16_t>(static_cast<uint16_t>(value.size()));
		add<string_t>(value, value.size());
	}

	template <typename TElement>
	auto PacketBuilder::addImpl(const vector_t<TElement> &value) -> void {
		addImplDefault<uint32_t>(value.size());
		add<vector_t<TElement>>(value, value.size());
	}

	template <>
	auto PacketBuilder::addSizedImpl<string_t>(const string_t &value, size_t size) -> void {
		size_t slen = value.size();
		if (size < slen) {
			throw std::invalid_argument{"addString used with a length shorter than string size"};
		}
		strncpy(reinterpret_cast<char *>(getBuffer(m_pos, size)), value.c_str(), slen);
		for (size_t i = slen; i < size; i++) {
			m_packet[m_pos + i] = 0;
		}
		m_pos += size;
	}

	template <typename TElement>
	auto PacketBuilder::addSizedImpl(const vector_t<TElement> &value, size_t size) -> void {
		for (size_t iter = 0; iter < size; ++iter) {
			add<TElement>(value[iter]);
		}
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
}