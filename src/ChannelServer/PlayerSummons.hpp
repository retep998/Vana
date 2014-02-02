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
#include "Summon.hpp"
#include "Types.hpp"

class PacketBuilder;
class PacketReader;
class Player;

class PlayerSummons {
	NONCOPYABLE(PlayerSummons);
	NO_DEFAULT_CONSTRUCTOR(PlayerSummons);
public:
	PlayerSummons(Player *player) : m_player(player) { }

	auto getSummon() -> Summon * { return m_summon; }
	auto getPuppet() -> Summon * { return m_puppet; }
	auto getSummon(int32_t summonId) -> Summon *;

	auto addSummon(Summon *summon, int32_t time) -> void;
	auto removeSummon(bool puppet, bool fromTimer) -> void;

	auto getTransferPacket() const -> PacketBuilder;
	auto parseTransferPacket(PacketReader &reader) -> void;
private:
	auto getSummonTimeRemaining() const -> seconds_t;

	Player *m_player = nullptr;
	Summon *m_summon = nullptr;
	Summon *m_puppet = nullptr;
};