/*
Copyright (C) 2008-2011 Vana Development Team

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

class PacketCreator;
class PacketReader;
class Player;
class Summon;

class PlayerSummons {
public:
	PlayerSummons(Player *player) : m_player(player), m_summon(nullptr), m_puppet(nullptr) { }

	Summon * getSummon() { return m_summon; }
	Summon * getPuppet() { return m_puppet; }
	Summon * getSummon(int32_t summonid);

	void addSummon(Summon *summon, int32_t time);
	void removeSummon(bool puppet, bool fromTimer);

	// Packet marshaling
	void getSummonTransferPacket(PacketCreator &packet);
	void parseSummonTransferPacket(PacketReader &packet);
private:
	Player *m_player;
	Summon *m_summon;
	Summon *m_puppet;

	int32_t getSummonTimeRemaining();
};
