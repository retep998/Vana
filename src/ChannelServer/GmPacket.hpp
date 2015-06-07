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

#include "PacketBuilder.hpp"
#include "Types.hpp"
#include <string>

class Player;

namespace GmPacket {
	namespace HiredMerchantModes {
		enum {
			Map = 0x00,
			Channel = 0x01,
		};
	}

	PACKET(beginHide);
	PACKET(endHide);
	PACKET(warning, bool succeed);
	PACKET(block);
	PACKET(invalidCharacterName);
	PACKET(hiredMerchantPlace, int8_t mode, int32_t id);
	PACKET(setGetVarResult, const string_t &name, const string_t &variable, const string_t &value);
}