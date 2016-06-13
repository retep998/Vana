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

#include "common/types.hpp"
#include "common/i_packet.hpp"

namespace vana {
	namespace data {
		namespace type {
			enum class buff_source_type : int8_t {
				none = 0,
				skill,
				mob_skill,
				item,
			};
		}
	}

	template <>
	struct packet_serialize<data::type::buff_source_type> {
		auto read(packet_reader &reader) -> data::type::buff_source_type {
			return static_cast<data::type::buff_source_type>(reader.get<int8_t>());
		}
		auto write(packet_builder &builder, const data::type::buff_source_type &obj) -> void {
			builder.add<int8_t>(static_cast<int8_t>(obj));
		}
	};
}