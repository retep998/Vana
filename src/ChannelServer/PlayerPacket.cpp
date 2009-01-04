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
#include "PlayerPacket.h"
#include "KeyMaps.h"
#include "MapleSession.h"
#include "PacketCreator.h"
#include "Pets.h"
#include "Player.h"
#include "PlayerPacketHelper.h"
#include "Randomizer.h"
#include "SendHeader.h"
#include "SkillMacros.h"
#include "Skills.h"
#include "TimeUtilities.h"

void PlayerPacket::connectData(Player *player) {
	PacketCreator packet;
	packet.addShort(SEND_CHANGE_MAP);
	packet.addInt(ChannelServer::Instance()->getChannel()); // Channel
	packet.addByte(1);
	packet.addByte(1);
	packet.addShort(0);
	packet.addInt(Randomizer::Instance()->randInt()); //
	packet.addInt(Randomizer::Instance()->randInt()); // Possibly seeding maple's rng
	packet.addInt(Randomizer::Instance()->randInt()); //
	packet.addInt(-1);
	packet.addInt(-1);
	packet.addInt(player->getId());
	packet.addString(player->getName(), 12);
	packet.addByte(0);
	packet.addByte(player->getGender());
	packet.addByte(player->getSkin());
	packet.addInt(player->getEyes());
	packet.addInt(player->getHair());
	packet.addInt(0);
	packet.addInt(0);
	packet.addInt(0);
	packet.addInt(0);
	packet.addInt(0);
	packet.addInt(0);
	packet.addByte(player->getLevel());
	packet.addShort(player->getJob());
	packet.addShort(player->getStr());
	packet.addShort(player->getDex());
	packet.addShort(player->getInt());
	packet.addShort(player->getLuk());
	packet.addShort(player->getHP());
	packet.addShort(player->getMHP());
	packet.addShort(player->getMP());
	packet.addShort(player->getMMP());
	packet.addShort(player->getAP());
	packet.addShort(player->getSP());
	packet.addInt(player->getExp());
	packet.addShort(player->getFame());
	packet.addInt(0); // Unknown int32 added in .62
	packet.addInt(player->getMap());
	packet.addByte(player->getMappos());
	packet.addInt(0); // Unknown int32 added in .62
	packet.addByte(0x14);
	player->getInventory()->connectData(packet); // Inventory data
	player->getSkills()->connectData(packet); // Skills
	// End
	packet.addInt(0);
	packet.addInt(0);
	packet.addInt(0);
	packet.addShort(0);
	for (int32_t i = 0; i < 15; i++)
		packet.addBytes("FFC99A3B");
	packet.addInt(0);
	packet.addInt64(TimeUtilities::getServerTime());
	player->getSession()->send(packet);
}

void PlayerPacket::showKeys(Player *player, KeyMaps *keyMaps) {
	PacketCreator packet;
	packet.addShort(SEND_KEYMAP);
	packet.addByte(0);
	for (size_t i = 0; i < KeyMaps::size; i++) {
		KeyMaps::KeyMap *keyMap = keyMaps->getKeyMap(i);
		if (keyMap != 0) {
			packet.addByte(keyMap->type);
			packet.addInt(keyMap->action);
		}
		else {
			packet.addByte(0);
			packet.addInt(0);
		}
	}
	player->getSession()->send(packet);
}

void PlayerPacket::showSkillMacros(Player *player, SkillMacros *macros) {
	PacketCreator packet;
	packet.addShort(SEND_SKILL_MACRO);
	packet.addByte(macros->getMax() + 1);
	for (int8_t i = 0; i <= macros->getMax();  i++) {
		SkillMacros::SkillMacro *macro = macros->getSkillMacro(i);
		if (macro != 0) {
			packet.addString(macro->name);
			packet.addByte(macro->shout);
			packet.addInt(macro->skill1);
			packet.addInt(macro->skill2);
			packet.addInt(macro->skill3);
		}
		else {
			packet.addString("");
			packet.addByte(0);
			packet.addInt(0);
			packet.addInt(0);
			packet.addInt(0);
		}
	}
	
	player->getSession()->send(packet);
}

void PlayerPacket::updateStatInt(Player *player, int32_t id, int32_t value, bool is) {
	PacketCreator packet;
	packet.addShort(SEND_UPDATE_STAT);
	packet.addByte(is);
	packet.addInt(id);
	packet.addInt(value);
	player->getSession()->send(packet);
}

void PlayerPacket::updateStatShort(Player *player, int32_t id, int16_t value, bool is) {
	PacketCreator packet;
	packet.addShort(SEND_UPDATE_STAT);
	packet.addByte(is);
	packet.addInt(id);
	packet.addShort(value);
	player->getSession()->send(packet);
}

void PlayerPacket::updateStatChar(Player *player, int32_t id, int8_t value, bool is) {
	PacketCreator packet;
	packet.addShort(SEND_UPDATE_STAT);
	packet.addByte(is);
	packet.addInt(id);
	packet.addByte(value);
	player->getSession()->send(packet);
}

void PlayerPacket::changeChannel(Player *player, const string &ip, int16_t port) {
	PacketCreator packet;
	packet.addShort(SEND_CHANGE_CHANNEL);
	packet.addByte(1);
	packet.addIP(ip);
	packet.addShort(port);
	player->getSession()->send(packet);
}

void PlayerPacket::showMessage(Player *player, const string &msg, int8_t type) {
	PacketCreator packet;
	packet.addShort(SEND_NOTICE); 
	packet.addByte(type);
	packet.addString(msg);
	player->getSession()->send(packet);
}

void PlayerPacket::instructionBubble(Player *player, const string &msg, int16_t width, int16_t height) {
	if (width == -1) {
		width = msg.size() * 10;
		if (width < 40) {
			width = 40; // Anything lower crashes client/doesn't look good
		}
	}

	PacketCreator packet;
	packet.addShort(SEND_INSTRUCTION_BUBBLE);
	packet.addString(msg);
	packet.addShort(width);
	packet.addShort(height);
	packet.addByte(1);

	player->getSession()->send(packet);
}
