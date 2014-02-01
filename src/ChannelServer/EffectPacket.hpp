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

#include "PacketBuilder.hpp"
#include "SplitPacketBuilder.hpp"
#include "Types.hpp"
#include <string>

class Player;

namespace EffectPacket {
	PACKET(playMusic, const string_t &music);
	PACKET(sendEvent, const string_t &id);
	PACKET(sendEffect, const string_t &effect);
	PACKET(playPortalSoundEffect);
	PACKET(sendFieldSound, const string_t &sound);
	PACKET(sendMinigameSound, const string_t &sound);
	SPLIT_PACKET(sendMobItemBuffEffect, int32_t playerId, int32_t itemId);
}