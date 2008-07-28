/*
Copyright (C) 2008 Vana Development Team

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
#include "PacketCreator.h"
#include "Player.h"
#include "Inventory.h"
#include "Skills.h"
#include "TimeUtilities.h"
#include "PlayerPacketHelper.h"
#include "Randomizer.h"
#include "SendHeader.h"
#include "KeyMaps.h"
#include "SkillMacros.h"

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
	packet.addInt(player->getPlayerid());
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
	packet.addShort(player->getAp());
	packet.addShort(player->getSp());
	packet.addInt(player->getExp());
	packet.addShort(player->getFame());
	packet.addInt(player->getMap());
	packet.addByte(player->getMappos());
	packet.addByte(0x14);
	packet.addInt(player->inv->getMesos());
	packet.addByte(player->inv->getMaxSlots(1));
	packet.addByte(player->inv->getMaxSlots(2));
	packet.addByte(player->inv->getMaxSlots(3));
	packet.addByte(player->inv->getMaxSlots(4));
	packet.addByte(player->inv->getMaxSlots(5));
	iteminventory *items = player->inv->getItems(1);
	for (iteminventory::iterator iter = items->begin(); iter != items->end(); iter++) {
		Item *equip = iter->second;
		if (iter->first < 0 && !Inventory::isCash(equip->id)) {
			PlayerPacketHelper::addEquip(packet, iter->first, equip);
		}
	}
	packet.addByte(0);
	for (iteminventory::iterator iter = items->begin(); iter != items->end(); iter++) {
		Item *equip = iter->second;
		if (iter->first < 0 && Inventory::isCash(equip->id)) {
			PlayerPacketHelper::addEquip(packet, iter->first, equip);
		}
	}
	packet.addByte(0);
	for (iteminventory::iterator iter = items->begin(); iter != items->end(); iter++) {
		Item *equip = iter->second;
		if (iter->first > 0) {
			PlayerPacketHelper::addEquip(packet, iter->first, equip);
		}
	}
	packet.addByte(0);
	for (char i = 2; i <= 5; i++) {
		for (short s = 1; s <= player->inv->getMaxSlots(i); s++) {
			Item *item = player->inv->getItem(i, s);
			if (item == 0)
				continue;
			PlayerPacketHelper::addItem(packet, s, item, false);
		}
		packet.addByte(0);
	}
	//Skills
	packet.addShort(player->skills->getSkillsNum());
	for (int i = 0; i < player->skills->getSkillsNum(); i++) {
		packet.addInt(player->skills->getSkillID(i));
		packet.addInt(player->skills->getSkillLevel(player->skills->getSkillID(i)));
		if (FORTHJOB_SKILL(player->skills->getSkillID(i)))
			packet.addInt(player->skills->getMaxSkillLevel(player->skills->getSkillID(i))); // Max Level for 4th job skills
	}
	//End
	packet.addInt(0);
	packet.addInt(0);
	packet.addInt(0);
	packet.addShort(0);
	for (int i = 0; i < 15; i++)
		packet.addBytes("FFC99A3B");
	packet.addInt(0);
	packet.addInt64(TimeUtilities::getServerTime());
	packet.send(player);
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
	packet.send(player);
}

void PlayerPacket::showSkillMacros(Player *player, SkillMacros *macros) {
	PacketCreator packet;
	packet.addShort(SEND_SKILL_MACRO);
	packet.addByte(macros->getMax() + 1);
	for (int i = 0; i <= macros->getMax();  i++) {
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
	
	packet.send(player);
}

void PlayerPacket::updateStat(Player *player, int id, int value, bool is) {
	PacketCreator packet;
	packet.addShort(SEND_UPDATE_STAT);
	packet.addByte(is);
	packet.addInt(id);
	packet.addInt(value);
	packet.send(player);
}

void PlayerPacket::updateStat(Player *player, int id, short value, bool is) {
	PacketCreator packet;
	packet.addShort(SEND_UPDATE_STAT);
	packet.addByte(is);
	packet.addInt(id);
	packet.addShort(value);
	packet.send(player);
}

void PlayerPacket::updateStat(Player *player, int id, char value, bool is) {
	PacketCreator packet;
	packet.addShort(SEND_UPDATE_STAT);
	packet.addByte(is);
	packet.addInt(id);
	packet.addByte(value);
	packet.send(player);
}

void PlayerPacket::changeChannel(Player *player, const string &ip, short port) {
	PacketCreator packet;
	packet.addShort(SEND_CHANGE_CHANNEL);
	packet.addByte(1);
	packet.addIP(ip);
	packet.addShort(port);
	packet.send(player);
}

void PlayerPacket::showMessage(Player *player, const string &msg, char type) {
	PacketCreator packet;
	packet.addShort(SEND_NOTICE); 
	packet.addByte(type);
	packet.addString(msg);
	packet.send(player);
}

void PlayerPacket::instructionBubble(Player *player, const string &msg, short width, short height) {
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

	packet.send(player);
}
