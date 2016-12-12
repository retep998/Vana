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

#include "common/i_packet.hpp"
#include "common/types.hpp"
#include "common/util/shared_array.hpp"
#include <cstring>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

#define PACKET(TypeName, ...) \
	auto TypeName(__VA_ARGS__) -> packet_builder;

#define PACKET_IMPL(TypeName, ...) \
	auto TypeName(__VA_ARGS__) -> packet_builder

namespace vana {
	class packet_reader;

	class packet_builder {
	public:
		packet_builder();

		template <typename TValue>
		auto add(const TValue &value) -> packet_builder &;
		template <typename TValue>
		auto unk() -> packet_builder &;
		// Used in conjunction with packet information that's intended to be set later
		template <typename TValue>
		auto defer() -> packet_builder &;
		template <typename TValue>
		auto unk(const TValue &value) -> packet_builder &;
		auto unk(int32_t bytes) -> packet_builder &;

		template <typename TValue>
		auto add(const TValue &value, size_t count) -> packet_builder &;
		template <typename TValue>
		auto set(const TValue &value, size_t pos) -> packet_builder &;

		auto add_bytes(const string &hex) -> packet_builder &;
		auto add_bytes(const char *hex) -> packet_builder &;
		auto add_buffer(const unsigned char *bytes, size_t len) -> packet_builder &;
		auto add_buffer(const packet_builder &builder) -> packet_builder &;
		auto add_buffer(const packet_reader &reader) -> packet_builder &;

		auto get_buffer() const -> const unsigned char *;
		auto get_size() const -> size_t;
		auto to_string() const -> string;
	private:
		static const size_t default_buffer_len = 100; // Initial buffer length
		friend auto operator <<(std::ostream &out, const packet_builder &builder) -> std::ostream &;

		auto get_buffer(size_t pos, size_t len) -> unsigned char *;
		static auto get_hex_byte(unsigned char input) -> unsigned char;

		template <typename TValue>
		auto add_impl(const TValue &val) -> void;
		template <typename TValue>
		auto add_sized_impl(const TValue &val, size_t size) -> void;

		template <typename TValue>
		auto add_impl_default(const TValue &value) -> void;
		template <>
		auto add_impl<bool>(const bool &val) -> void;
		template <>
		auto add_impl<double>(const double &val) -> void;
		template <>
		auto add_impl<string>(const string &val) -> void;
		template <>
		auto add_impl<int8_t>(const int8_t &val) -> void;
		template <>
		auto add_impl<int16_t>(const int16_t &val) -> void;
		template <>
		auto add_impl<int32_t>(const int32_t &val) -> void;
		template <>
		auto add_impl<int64_t>(const int64_t &val) -> void;
		template <>
		auto add_impl<uint8_t>(const uint8_t &val) -> void;
		template <>
		auto add_impl<uint16_t>(const uint16_t &val) -> void;
		template <>
		auto add_impl<uint32_t>(const uint32_t &val) -> void;
		template <>
		auto add_impl<uint64_t>(const uint64_t &val) -> void;
		template <>
		auto add_impl<milliseconds>(const milliseconds &val) -> void;
		template <>
		auto add_impl<seconds>(const seconds &val) -> void;
		template <>
		auto add_impl<minutes>(const minutes &val) -> void;
		template <>
		auto add_impl<hours>(const hours &val) -> void;
		template <>
		auto add_impl<packet_date>(const packet_date &val) -> void;
		template <typename TElement>
		auto add_impl(const vector<TElement> &val) -> void;

		template <>
		auto add_sized_impl<string>(const string &val, size_t size) -> void;
		template <typename TElement>
		auto add_sized_impl(const vector<TElement> &val, size_t size) -> void;

		size_t m_pos = 0;
		size_t m_packet_capacity = 0;
		vana::util::shared_array<unsigned char> m_packet;
	};

	template <typename TValue>
	auto packet_builder::add(const TValue &value) -> packet_builder & {
		add_impl(value);
		return *this;
	}

	template <typename TValue>
	auto packet_builder::unk() -> packet_builder & {
		add_impl(TValue{});
		return *this;
	}

	template <typename TValue>
	auto packet_builder::defer() -> packet_builder & {
		add_impl(TValue{});
		return *this;
	}

	template <typename TValue>
	auto packet_builder::unk(const TValue &value) -> packet_builder & {
		add_impl(value);
		return *this;
	}

	template <typename TValue>
	auto packet_builder::add(const TValue &value, size_t size) -> packet_builder & {
		add_sized_impl(value, size);
		return *this;
	}

	template <typename TValue>
	auto packet_builder::add_impl(const TValue &val) -> void {
		packet_serialize<TValue> x;
		return x.write(*this, val);
	}

	template <typename TValue>
	auto packet_builder::add_sized_impl(const TValue &value, size_t size) -> void {
		static_assert(false, "T is not appropriately specialized for that type");
		throw std::logic_error{"T is not appropriately specialized for that type"};
	}

	template <typename TValue>
	auto packet_builder::add_impl_default(const TValue &value) -> void {
		*reinterpret_cast<TValue *>(get_buffer(m_pos, sizeof(TValue))) = value;
		m_pos += sizeof(TValue);
	}

	template <typename TValue>
	auto packet_builder::set(const TValue &value, size_t pos) -> packet_builder & {
		*reinterpret_cast<TValue *>(get_buffer(pos, sizeof(TValue))) = value;
		return *this;
	}

	template <>
	auto packet_builder::add_impl<bool>(const bool &value) -> void {
		add_impl_default<int8_t>(value ? 1 : 0);
	}

	template <>
	auto packet_builder::add_impl<double>(const double &value) -> void {
		add_impl_default<double>(value);
	}

	template <>
	auto packet_builder::add_impl<int8_t>(const int8_t &value) -> void {
		add_impl_default<int8_t>(value);
	}

	template <>
	auto packet_builder::add_impl<int16_t>(const int16_t &value) -> void {
		add_impl_default<int16_t>(value);
	}

	template <>
	auto packet_builder::add_impl<int32_t>(const int32_t &value) -> void {
		add_impl_default<int32_t>(value);
	}

	template <>
	auto packet_builder::add_impl<int64_t>(const int64_t &value) -> void {
		add_impl_default<int64_t>(value);
	}

	template <>
	auto packet_builder::add_impl<uint8_t>(const uint8_t &value) -> void {
		add_impl_default<uint8_t>(value);
	}

	template <>
	auto packet_builder::add_impl<uint16_t>(const uint16_t &value) -> void {
		add_impl_default<uint16_t>(value);
	}

	template <>
	auto packet_builder::add_impl<uint32_t>(const uint32_t &value) -> void {
		add_impl_default<uint32_t>(value);
	}

	template <>
	auto packet_builder::add_impl<uint64_t>(const uint64_t &value) -> void {
		add_impl_default<uint64_t>(value);
	}

	template <>
	auto packet_builder::add_impl<milliseconds>(const milliseconds &value) -> void {
		add_impl_default<int32_t>(static_cast<int32_t>(value.count()));
	}

	template <>
	auto packet_builder::add_impl<seconds>(const seconds &value) -> void {
		add_impl_default<int32_t>(static_cast<int32_t>(value.count()));
	}

	template <>
	auto packet_builder::add_impl<minutes>(const minutes &value) -> void {
		add_impl_default<int32_t>(static_cast<int32_t>(value.count()));
	}

	template <>
	auto packet_builder::add_impl<hours>(const hours &value) -> void {
		add_impl_default<int32_t>(static_cast<int32_t>(value.count()));
	}
	
	template <>
	auto packet_builder::add_impl<packet_date>(const packet_date &val) -> void {
		// The value will be YYYYMMDD
		uint32_t yyymmdd = 
			(val.day) +
			(val.month * 100) +
			(val.year * 10000);
		add_impl_default<uint32_t>(yyymmdd);
	}

	template <>
	auto packet_builder::add_impl<string>(const string &value) -> void {
		if (value.size() > static_cast<size_t>(std::numeric_limits<uint16_t>::max())) throw std::invalid_argument{"String is too large to be sent via packet"};
		add_impl_default<uint16_t>(static_cast<uint16_t>(value.size()));
		add<string>(value, value.size());
	}

	template <typename TElement>
	auto packet_builder::add_impl(const vector<TElement> &value) -> void {
		add_impl_default<uint32_t>(value.size());
		add<vector<TElement>>(value, value.size());
	}

	template <>
	auto packet_builder::add_sized_impl<string>(const string &value, size_t size) -> void {
		size_t slen = value.size();
		if (size < slen) {
			throw std::invalid_argument{"addString used with a length shorter than string size"};
		}
		strncpy(reinterpret_cast<char *>(get_buffer(m_pos, size)), value.c_str(), slen);
		for (size_t i = slen; i < size; i++) {
			m_packet[m_pos + i] = 0;
		}
		m_pos += size;
	}

	template <typename TElement>
	auto packet_builder::add_sized_impl(const vector<TElement> &value, size_t size) -> void {
		for (size_t iter = 0; iter < size; ++iter) {
			add<TElement>(value[iter]);
		}
	}

	inline
	auto packet_builder::get_buffer() const -> const unsigned char * {
		return m_packet.get();
	}

	inline
	auto packet_builder::add_buffer(const unsigned char *bytes, size_t len) -> packet_builder & {
		memcpy(get_buffer(m_pos, len), bytes, len);
		m_pos += len;
		return *this;
	}

	inline
	auto packet_builder::get_size() const -> size_t {
		return m_pos;
	}

	inline
	auto operator <<(std::ostream &out, const packet_builder &builder) -> std::ostream & {
		out << builder.to_string();
		return out;
	}
}