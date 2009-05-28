/*
Copyright (C) 2008-2009 Vana Development Team

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
#include "PlayersPacket.h"
#include "GameConstants.h"
#include "MapleSession.h"
#include "Maps.h"
#include "PacketCreator.h"
#include "Pets.h"
#include "Player.h"
#include "Players.h"
#include "InterHeader.h"
#include "SendHeader.h"

void PlayersPacket::showMoving(Player *player, unsigned char *buf, size_t size) {
	if (player->getActiveBuffs()->isUsingHide())
		return;
	PacketCreator packet;
	packet.add<int16_t>(SEND_MOVE_PLAYER);
	packet.add<int32_t>(player->getId());
	packet.add<int32_t>(0);
	packet.addBuffer(buf, size);
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void PlayersPacket::faceExpression(Player *player, int32_t face) {
	if (player->getActiveBuffs()->isUsingHide())
		return;
	PacketCreator packet;
	packet.add<int16_t>(SEND_FACE_EXPRESSION);
	packet.add<int32_t>(player->getId());
	packet.add<int32_t>(face);
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void PlayersPacket::showChat(Player *player, const string &msg, int8_t bubbleOnly) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_CHAT);
	packet.add<int32_t>(player->getId());
	packet.add<int8_t>(player->isGm());
	packet.addString(msg);
	packet.add<int8_t>(bubbleOnly);
	Maps::getMap(player->getMap())->sendPacket(packet);
}

void PlayersPacket::damagePlayer(Player *player, int32_t dmg, int32_t mob, uint8_t hit, uint8_t type, uint8_t stance, int32_t nodamageskill, PGMRInfo pgmr) {
	if (player->getActiveBuffs()->isUsingHide())
		return;
	PacketCreator packet;
	packet.add<int16_t>(SEND_DAMAGE_PLAYER);
	packet.add<int32_t>(player->getId());
	packet.add<int8_t>(type);
	switch (type) {
		case 0xFE:
			packet.add<int32_t>(dmg);
			packet.add<int32_t>(dmg);
			break;
		default:
			packet.add<int32_t>((pgmr.reduction > 0 ? pgmr.damage : dmg));
			packet.add<int32_t>(mob);
			packet.add<int8_t>(hit);
			if (pgmr.reduction > 0) {
				packet.add<int8_t>(pgmr.reduction);
				packet.add<int8_t>(pgmr.isphysical); // Maybe? No Mana Reflection on global to test with
				packet.add<int32_t>(pgmr.mapmobid);
				packet.add<int8_t>(6);
				packet.addPos(pgmr.pos);
			}
			else
				packet.add<int8_t>(0);
			packet.add<int8_t>(stance);
			packet.add<int32_t>(dmg);
			if (nodamageskill > 0)
				packet.add<int32_t>(nodamageskill);
			break;
	}
	Maps::getMap(player->getMap())->sendPacket(packet);
}

void PlayersPacket::showMessage(const string &msg, int8_t type) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_NOTICE); 
	packet.add<int8_t>(type);
	packet.addString(msg);
	if (type == 6)
		packet.add<int32_t>(0);
	Players::Instance()->sendPacket(packet);
}

void PlayersPacket::showMessageWorld(const string &msg, int8_t type) {
	PacketCreator packet;
	packet.add<int16_t>(INTER_TO_PLAYERS);
	packet.add<int16_t>(SEND_NOTICE);
	packet.add<int8_t>(type);
	packet.addString(msg);
	ChannelServer::Instance()->sendToWorld(packet);
}

void PlayersPacket::showInfo(Player *player, Player *getinfo, uint8_t isself) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_PLAYER_INFO);
	packet.add<int32_t>(getinfo->getId());
	packet.add<int8_t>(getinfo->getLevel());
	packet.add<int16_t>(getinfo->getJob());
	packet.add<int16_t>(getinfo->getFame());
	packet.add<int8_t>(0); // Married
	packet.addString("-"); // Guild
	packet.addString(""); // Guild Alliance
	packet.add<int8_t>(isself); // Is 1 when the character is clicking themselves
	for (int8_t i = 1; i <= Inventories::MaxPetCount; i++) {
		if (Pet *pet = getinfo->getPets()->getSummoned(i)) {
			packet.add<int8_t>(1);
			packet.add<int32_t>(pet->getItemId());
			packet.addString(pet->getName());
			packet.add<int8_t>(pet->getLevel());
			packet.add<int16_t>(pet->getCloseness());
			packet.add<int8_t>(pet->getFullness());
			packet.add<int16_t>(0);
			packet.add<int32_t>(getinfo->getInventory()->getItem(1,  -114 - (i == 1 ? 16 : (i == 2 ? 24 : 0))) != 0 ? getinfo->getInventory()->getItem(1, -114 - (i == 1 ? 16 : (i == 2 ? 24 : 0)))->id : 0);
		}
	}
	packet.add<int8_t>(0); // End of pets / start of taming mob
	packet.add<int8_t>(0); // End of taming mob / start of wish list
	vector<int32_t> wishlist = getinfo->getWishlist(); 
	packet.add<uint8_t>((uint8_t)(wishlist.size())); // Wish list count
	for (size_t i = 0; i < wishlist.size(); i++)
		packet.add<int32_t>(wishlist[i]);
	packet.add<int32_t>(1);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	player->getSession()->send(packet);
}

void PlayersPacket::whisperPlayer(Player *target, const string &whisperer_name, uint16_t channel, const string &message) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_COMMAND_RESPOND);
	packet.add<int8_t>(0x12);
	packet.addString(whisperer_name);
	packet.add<int16_t>(channel);
	packet.addString(message);
	target->getSession()->send(packet);
}

void PlayersPacket::findPlayer(Player *player, const string &name, int32_t map, uint8_t is, bool is_channel) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_COMMAND_RESPOND);
	if (map != -1) {
		packet.add<int8_t>(0x09);
		packet.addString(name);
		if (is_channel)
			packet.add<int8_t>(0x03);
		else
			packet.add<int8_t>(0x01);
		packet.add<int32_t>(map);
		packet.add<int32_t>(0);
		packet.add<int32_t>(0);
	}
	else {	
		packet.add<int8_t>(0x0A);
		packet.addString(name);
		packet.add<int8_t>(is);
	}

	player->getSession()->send(packet);
}

void PlayersPacket::sendToPlayers(unsigned char *data, int32_t len) {
	PacketCreator packet;
	packet.addBuffer(data, len);
	Players::Instance()->sendPacket(packet);
}
