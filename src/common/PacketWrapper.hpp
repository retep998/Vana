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

#include "common/PacketBuilder.hpp"
#include "common/SplitPacketBuilder.hpp"
#include "common/Types.hpp"

namespace vana {
	namespace packets {
		inline
		auto prepend(const packet_builder &builder, function<void(packet_builder &)> wrap_function) -> packet_builder {
			packet_builder packet;
			wrap_function(packet);
			packet.add_buffer(builder);
			return packet;
		}

		// Converts the type of packet to PacketBuilder
		inline
		auto identity(const packet_reader &reader) -> packet_builder {
			packet_builder builder;
			builder.add_buffer(reader);
			return builder;
		}
	}
}