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

#include "Common/Types.hpp"
#include "Common/IPacket.hpp"

namespace Vana {
	enum class BuffSourceType : int8_t {
		None,
		Skill,
		MobSkill,
		Item,
	};

	template <>
	struct PacketSerialize<BuffSourceType> {
		auto read(PacketReader &reader) -> BuffSourceType {
			return static_cast<BuffSourceType>(reader.get<int8_t>());
		}
		auto write(PacketBuilder &builder, const BuffSourceType &obj) -> void {
			builder.add<int8_t>(static_cast<int8_t>(obj));
		}
	};
}