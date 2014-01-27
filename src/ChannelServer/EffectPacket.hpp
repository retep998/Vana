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
#include <string>

class Player;

namespace EffectPacket {
	auto playMusic(int32_t mapId, const string_t &music) -> void;
	auto playMusic(Player *player, const string_t &music) -> void;
	auto sendEvent(int32_t mapId, const string_t &id) -> void;
	auto sendEffect(int32_t mapId, const string_t &effect) -> void;
	auto playPortalSoundEffect(Player *player) -> void;
	auto sendFieldSound(int32_t mapId, const string_t &sound) -> void;
	auto sendFieldSound(Player *player, const string_t &sound) -> void;
	auto sendMinigameSound(int32_t mapId, const string_t &sound) -> void;
	auto sendMinigameSound(Player *player, const string_t &sound) -> void;
	auto sendMobItemBuffEffect(Player *player, int32_t itemId) -> void;
}