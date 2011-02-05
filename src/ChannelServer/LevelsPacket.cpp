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
#include "LevelsPacket.h"
#include "MapleSession.h"
#include "Maps.h"
#include "PacketCreator.h"
#include "Player.h"
#include "SendHeader.h"

void LevelsPacket::showEXP(Player *player, int32_t exp, bool white, bool inChat) {
	PacketCreator packet;
	packet.addHeader(SMSG_NOTICE);
	packet.add<int8_t>(3);
	packet.addBool(white);
	packet.add<int32_t>(exp);
	packet.addBool(inChat);
	packet.add<int32_t>(0);
	packet.add<int8_t>(0);
	packet.add<int8_t>(0);
	packet.add<int32_t>(0);
	if (inChat) {
		packet.add<int8_t>(0);
	}
	packet.add<int8_t>(0);
	packet.add<int32_t>(0);

	player->getSession()->send(packet);
}

void LevelsPacket::levelUp(Player *player) {
	PacketCreator packet;
	packet.addHeader(SMSG_SKILL_SHOW);
	packet.add<int32_t>(player->getId());
	packet.add<int8_t>(0);
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void LevelsPacket::statOK(Player *player) {
	PacketCreator packet;
	packet.addHeader(SMSG_PLAYER_UPDATE);
	packet.add<int16_t>(1);
	packet.add<int32_t>(0);
	player->getSession()->send(packet);
}

void LevelsPacket::jobChange(Player *player) {
	PacketCreator packet;
	packet.addHeader(SMSG_SKILL_SHOW);
	packet.add<int32_t>(player->getId());
	packet.add<int8_t>(8);
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}
