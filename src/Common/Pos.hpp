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
	Pos(coord_t x, coord_t y) : x(x), y(y) { }
	Pos() = default;

	auto isOrigin() const -> bool {
		return x == 0 && y == 0;
	}

	auto operator-(const Pos &p) const -> int32_t {
		return static_cast<int32_t>(sqrt(pow(static_cast<float>(x - p.x), 2) + pow(static_cast<float>(y - p.y), 2)));
	}

	coord_t x = 0;
	coord_t y = 0;
	friend auto operator <<(std::ostream &out, const Pos &pos) -> std::ostream &;
};

template <>
struct PacketSerialize<Pos> {
	auto read(PacketReader &reader) -> Pos {
		Pos ret;
		ret.x = reader.get<coord_t>();
		ret.y = reader.get<coord_t>();
		return ret;
	}
	auto write(PacketBuilder &builder, const Pos &obj) -> void {
		builder.add<coord_t>(obj.x);
		builder.add<coord_t>(obj.y);
	}
};

inline
auto operator <<(std::ostream &out, const Pos &pos) -> std::ostream & {
	return out << "{" << pos.x << ", " << pos.y << "}";
}