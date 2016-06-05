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

namespace vana {
	class packet_builder;
	class packet_reader;

	template <typename T>
	struct packet_serialize {
		auto read(packet_reader &reader) -> T {
			static_assert(false, "T is not appropriately specialized for that type");
			throw std::logic_error{"T is not appropriately specialized for that type"};
		};
		auto write(packet_builder &builder, const T &obj) -> void {
			static_assert(false, "T is not appropriately specialized for that type");
			throw std::logic_error{"T is not appropriately specialized for that type"};
		};
	};
}