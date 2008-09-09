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
#include "Skills.h"
#include "TimeUtilities.h"
#include "PlayerPacketHelper.h"
#include "Randomizer.h"
#include "SendHeader.h"
#include "KeyMaps.h"
#include "SkillMacros.h"
#include "Pets.h"

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
	packet.addShort(player->getAp());
	packet.addShort(player->getSp());
	packet.addInt(player->getExp());
	packet.addShort(player->getFame());
	packet.addInt(player->getMap());
	packet.addByte(player->getMappos());
	packet.addByte(0x14);
	packet.addInt(player->getInventory()->getMesos());
	packet.addByte(player->getInventory()->getMaxSlots(1));
	packet.addByte(player->getInventory()->getMaxSlots(2));
	packet.addByte(player->getInventory()->getMaxSlots(3));
	packet.addByte(player->getInventory()->getMaxSlots(4));
	packet.addByte(player->getInventory()->getMaxSlots(5));
	iteminventory *items = player->getInventory()->getItems(1);
	for (iteminventory::iterator iter = items->begin(); iter != items->end(); iter++) {
		if (iter->first < 0 && iter->first > -100) {
			PlayerPacketHelper::addItemInfo(packet, iter->first, iter->second);
		}
	}
	packet.addByte(0);
	for (iteminventory::iterator iter = items->begin(); iter != items->end(); iter++) {
		if (iter->first < -100) {
			PlayerPacketHelper::addItemInfo(packet, iter->first, iter->second);
		}
	}
	packet.addByte(0);
	for (iteminventory::iterator iter = items->begin(); iter != items->end(); iter++) {
		if (iter->first > 0) {
			PlayerPacketHelper::addItemInfo(packet, iter->first, iter->second);
		}
	}
	packet.addByte(0);
	for (char i = 2; i <= 5; i++) {
		for (short s = 1; s <= player->getInventory()->getMaxSlots(i); s++) {
			Item *item = player->getInventory()->getItem(i, s);
			if (item == 0)
				continue;
			if (item->petid == 0) {
				PlayerPacketHelper::addItemInfo(packet, s, item);
			}
			else {
				Pet *pet = player->getPets()->getPet(item->petid);
				packet.addByte((char) s);
				packet.addByte(3);
				packet.addInt(item->id);
				packet.addByte(1);
				packet.addInt(pet->getId());
				packet.addInt(0);
				packet.addBytes("008005BB46E61702");
				packet.addString(pet->getName(), 13);
				packet.addByte(pet->getLevel());
				packet.addShort(pet->getCloseness());
				packet.addByte(pet->getFullness());
				packet.addByte(0);
				packet.addBytes("B8D56000CEC8"); // Most likely has expire date in it in korean time stamp
				packet.addByte(1); // Propapbly is it alive (1 Alive, 2 Dead)
				packet.addInt(0);
			}
		}
		packet.addByte(0);
	}
	//Skills
	unordered_map<int, PlayerSkillInfo> *playerskills = player->getSkills()->getSkills();
	packet.addShort((short) playerskills->size());
	for (unordered_map<int, PlayerSkillInfo>::iterator iter = playerskills->begin(); iter != playerskills->end(); iter++) {
		packet.addInt(iter->first);
		packet.addInt(iter->second.level);
		if (FORTHJOB_SKILL(iter->first))
			packet.addInt(iter->second.maxlevel); // Max Level for 4th job skills
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
	player->getPacketHandler()->send(packet);
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
	player->getPacketHandler()->send(packet);
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
	
	player->getPacketHandler()->send(packet);
}

void PlayerPacket::updateStat(Player *player, int id, int value, bool is) {
	PacketCreator packet;
	packet.addShort(SEND_UPDATE_STAT);
	packet.addByte(is);
	packet.addInt(id);
	packet.addInt(value);
	player->getPacketHandler()->send(packet);
}

void PlayerPacket::updateStat(Player *player, int id, short value, bool is) {
	PacketCreator packet;
	packet.addShort(SEND_UPDATE_STAT);
	packet.addByte(is);
	packet.addInt(id);
	packet.addShort(value);
	player->getPacketHandler()->send(packet);
}

void PlayerPacket::updateStat(Player *player, int id, char value, bool is) {
	PacketCreator packet;
	packet.addShort(SEND_UPDATE_STAT);
	packet.addByte(is);
	packet.addInt(id);
	packet.addByte(value);
	player->getPacketHandler()->send(packet);
}

void PlayerPacket::changeChannel(Player *player, const string &ip, short port) {
	PacketCreator packet;
	packet.addShort(SEND_CHANGE_CHANNEL);
	packet.addByte(1);
	packet.addIP(ip);
	packet.addShort(port);
	player->getPacketHandler()->send(packet);
}

void PlayerPacket::showMessage(Player *player, const string &msg, char type) {
	PacketCreator packet;
	packet.addShort(SEND_NOTICE); 
	packet.addByte(type);
	packet.addString(msg);
	player->getPacketHandler()->send(packet);
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

	player->getPacketHandler()->send(packet);
}
