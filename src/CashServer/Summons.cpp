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
#include "Summons.h"
#include "GameConstants.h"
#include "GameLogicUtilities.h"
#include "LoopingId.h"
#include "PacketReader.h"
#include "PlayerPacket.h"
#include "SkillDataProvider.h"

// Summon Class
Summon::Summon(int32_t id, int32_t summonid, uint8_t level) : id(id), summonid(summonid), level(level), hp(0) {

}

// Summons namespace
LoopingId Summons::summonids;

int32_t Summons::loopId() {
	return summonids.next();
}

void Summons::useSummon(Player *player, int32_t skillid, uint8_t level) {

}

void Summons::removeSummon(Player *player, bool puppet, bool animated, bool packetOnly, int8_t showMessage, bool fromTimer) {

}

void Summons::showSummon(Player *player) {

}

void Summons::showSummons(Player *ofplayer, Player *toplayer) {

}

void Summons::moveSummon(Player *player, PacketReader &packet) {

}

void Summons::damageSummon(Player *player, PacketReader &packet) {

}

string Summons::getSummonName(int32_t summonid) {
	return "wat";
}
