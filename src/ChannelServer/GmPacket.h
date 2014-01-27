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

#include "Types.h"
#include <string>

class PacketCreator;
class Player;

namespace GmPacket {
	auto beginHide(Player *player) -> void;
	auto endHide(Player *player) -> void;
	auto warning(Player *player, bool succeed) -> void;
	auto block(Player *player) -> void;
	auto invalidCharacterName(Player *player) -> void;
	auto hiredMerchantPlace(Player *player, int8_t channel) -> void;
	auto hiredMerchantPlace(Player *player, int32_t mapId) -> void;
	auto setGetVarResult(Player *player, const string_t &name, const string_t &variable, const string_t &value) -> void;
}