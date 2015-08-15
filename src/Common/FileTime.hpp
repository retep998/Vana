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

#include "hash_combine.hpp"
#include "IPacket.hpp"
#include "PacketBuilder.hpp"
#include "PacketReader.hpp"
#include "Types.hpp"

class UnixTime;

class FileTime {
public:
	FileTime();
	FileTime(int64_t t);
	FileTime(const UnixTime &t);
	FileTime(const FileTime &t);

	auto getValue() const -> int64_t;
	auto operator =(const FileTime &right) -> FileTime & { m_value = right.m_value; return *this; }
	auto operator +=(const FileTime &right) -> FileTime & { m_value += right.m_value; return *this; }
	auto operator -=(const FileTime &right) -> FileTime & { m_value -= right.m_value; return *this; }
	auto operator +(const FileTime &right) const -> FileTime { return FileTime{m_value + right.m_value}; }
	auto operator -(const FileTime &right) const -> FileTime { return FileTime{m_value - right.m_value}; }
private:
	int64_t m_value;

	static auto convert(time_t time) -> int64_t;
	friend auto operator ==(const FileTime &a, const FileTime &b) -> bool;
	friend auto operator !=(const FileTime &a, const FileTime &b) -> bool;
	friend auto operator <(const FileTime &a, const FileTime &b) -> bool;
	friend auto operator <=(const FileTime &a, const FileTime &b) -> bool;
	friend auto operator >(const FileTime &a, const FileTime &b) -> bool;
	friend auto operator >=(const FileTime &a, const FileTime &b) -> bool;
};

inline
auto operator ==(const FileTime &a, const FileTime &b) -> bool {
	return a.m_value == b.m_value;
}

inline
auto operator !=(const FileTime &a, const FileTime &b) -> bool {
	return !(a == b);
}

inline
auto operator >(const FileTime &a, const FileTime &b) -> bool {
	return a.m_value > b.m_value;
}

inline
auto operator >=(const FileTime &a, const FileTime &b) -> bool {
	return a.m_value >= b.m_value;
}

inline
auto operator <(const FileTime &a, const FileTime &b) -> bool {
	return a.m_value < b.m_value;
}

inline
auto operator <=(const FileTime &a, const FileTime &b) -> bool {
	return a.m_value <= b.m_value;
}

template <>
struct PacketSerialize<FileTime> {
	auto read(PacketReader &reader) -> FileTime {
		return FileTime{reader.get<int64_t>()};
	}
	auto write(PacketBuilder &builder, const FileTime &obj) -> void {
		builder.add<int64_t>(obj.getValue());
	}
};

namespace std {

template <>
struct hash<FileTime> {
	auto operator()(const FileTime &v) const -> size_t {
		return MiscUtilities::hash_combinator(static_cast<int64_t>(v.getValue()));
	}
};

}