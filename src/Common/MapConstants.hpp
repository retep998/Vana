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

#include "Types.hpp"

namespace Maps {
	enum : map_id_t {
		GmMap = 180000000,
		OriginOfClockTower = 220080001,
		SorcerersRoom = 270020211,
		NoMap = 999999999
	};
}

namespace ShipKind {
	enum : int8_t {
		Regular = 0x00,
		Balrog = 0x01,
	};
}