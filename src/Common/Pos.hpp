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
#include "PacketBuilder.hpp"
#include "PacketReader.hpp"
#include "Types.hpp"
#include <cmath>
#include <ostream>

struct Pos {
	Pos(int16_t x, int16_t y) : x(x), y(y) { }
	Pos() = default;

	auto operator-(const Pos &p) const -> int32_t {
		return static_cast<int32_t>(sqrt(pow(static_cast<float>(x - p.x), 2) + pow(static_cast<float>(y - p.y), 2)));
	}

	int16_t x = 0;
	int16_t y = 0;
	friend auto operator <<(std::ostream &out, const Pos &pos) -> std::ostream &;
};

template <>
struct PacketSerialize<Pos> {
	auto read(PacketReader &reader) -> Pos {
		Pos ret;
		ret.x = reader.get<int16_t>();
		ret.y = reader.get<int16_t>();
		return ret;
	}
	auto write(PacketBuilder &builder, const Pos &obj) -> void {
		builder.add<int16_t>(obj.x);
		builder.add<int16_t>(obj.y);
	}
};

inline
auto operator <<(std::ostream &out, const Pos &pos) -> std::ostream & {
	return out << "{" << pos.x << ", " << pos.y << "}";
}