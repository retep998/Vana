/*
Copyright (C) 2008-2012 Vana Development Team

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
#include "LevelsPacket.h"
#include "Maps.h"
#include "PacketCreator.h"
#include "Player.h"
#include "Session.h"
#include "SmsgHeader.h"

void LevelsPacket::showExp(Player *player, int32_t exp, bool white, bool inChat) {
	PacketCreator packet;
	packet.add<header_t>(SMSG_NOTICE);
	packet.add<int8_t>(3);
	packet.addBool(white);
	packet.add<int32_t>(exp);
	packet.addBool(inChat);
	packet.add<int32_t>(0); // Bonus Event EXP (+%d) (does not work with White on)
	packet.add<int8_t>(0);
	packet.add<int8_t>(0); // A (A bonus EXP %d%% is awarded for every 3rd monster defeated.)
	packet.add<int32_t>(0); // Wedding EXP

	/*
		if A > 0 then (Bonus EXP for hunting over %d hrs. (+%d)) 
			packet.add<int8_t>(0);
	*/

	if (inChat) {
		// Earned 'Spirit Week Event' bonus EXP. (+%d) (uses following byte as percentage)
		packet.add<int8_t>(0); // B
		/*
			if B > 0 then (The next %d completed quests will include additional Event Bonus EXP.)
				packet.add<int8_t>(0);
		*/
	}
	packet.add<int8_t>(0); // Party EXP bonus. -1 for default +30, or value / 100.0 = result
	packet.add<int32_t>(0); // Bonus EXP for PARTY (+%d) || Bonus Event Party EXP (+%d) x%d

	// ALL NEW LOLOLNEXON
	packet.add<int32_t>(0); // Equip Bonus Exp
	packet.add<int32_t>(0); // Internet Cafe EXP Bonus (+%d)
	packet.add<int32_t>(0); // Rainbow Week Bonus EXP (+%d)
	packet.add<int8_t>(0); // Monster Card Completion Set +exp
	packet.add<int32_t>(0); // Boom Up Bonus EXP (+%d)
	packet.add<int32_t>(0); // Potion Bonus EXP (+%d)
	packet.add<int32_t>(0); // %s Bonus EXP (+%d) (string bonus exp?)
	packet.add<int32_t>(0); // Buff Bonus EXP (+%d)
	packet.add<int32_t>(0); // Rest Bonus EXP (+%d)
	packet.add<int32_t>(0); // Item Bonus EXP (+%d)
	packet.add<int32_t>(0); // Party Ring Bonus EXP(+%d)
	packet.add<int32_t>(0); // Cake vs Pie Bonus EXP(+%d)
	
	if (inChat) {
		packet.add<int32_t>(0); // You have received extra EXP from Quest Booster (+%d)
	}
	player->getSession()->send(packet);
}

void LevelsPacket::levelUp(Player *player) {
	PacketCreator packet;
	packet.add<header_t>(SMSG_SKILL_SHOW);
	packet.add<int32_t>(player->getId());
	packet.add<int8_t>(0);
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void LevelsPacket::statOk(Player *player) {
	PacketCreator packet;
	packet.add<header_t>(SMSG_PLAYER_UPDATE);
	packet.add<int8_t>(1);
	packet.add<int64_t>(0);
	packet.addBool(false);
	packet.addBool(false);
	player->getSession()->send(packet);
}

void LevelsPacket::jobChange(Player *player) {
	PacketCreator packet;
	packet.add<header_t>(SMSG_SKILL_SHOW);
	packet.add<int32_t>(player->getId());
	packet.add<int8_t>(8);
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}