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
#include "ChannelServer.h"
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
	packet.add<int16_t>(SEND_CHANGE_MAP);
	packet.add<int32_t>(ChannelServer::Instance()->getChannel()); // Channel
	packet.add<int8_t>(1);
	packet.add<int8_t>(1);
	packet.add<int16_t>(0);
	packet.add<int32_t>(Randomizer::Instance()->randInt()); //
	packet.add<int32_t>(Randomizer::Instance()->randInt()); // Possibly seeding maple's rng
	packet.add<int32_t>(Randomizer::Instance()->randInt()); //
	packet.add<int32_t>(-1);
	packet.add<int32_t>(-1);
	packet.add<int32_t>(player->getId());
	packet.addString(player->getName(), 13);

	player->getStats()->connectData(packet);

	packet.add<int32_t>(0); // Unknown int32 added in .62
	packet.add<int32_t>(player->getMap());
	packet.add<int8_t>(player->getMappos());
	packet.add<int32_t>(0); // Unknown int32 added in .62
	packet.add<int8_t>(player->getBuddyListSize());

	player->getInventory()->connectData(packet); // Inventory data
	player->getSkills()->connectData(packet); // Skills - levels and cooldowns
	player->getQuests()->connectData(packet); // Quests

	packet.add<int32_t>(0);
	packet.add<int32_t>(0);

	player->getInventory()->rockPacket(packet); // Teleport Rock/VIP Rock maps
	player->getMonsterBook()->connectData(packet);

	packet.add<int64_t>(TimeUtilities::getServerTime());
	player->getSession()->send(packet);
}

void PlayerPacket::showKeys(Player *player, KeyMaps *keyMaps) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_KEYMAP);
	packet.add<int8_t>(0);
	for (size_t i = 0; i < KeyMaps::size; i++) {
		KeyMaps::KeyMap *keyMap = keyMaps->getKeyMap(i);
		if (keyMap != 0) {
			packet.add<int8_t>(keyMap->type);
			packet.add<int32_t>(keyMap->action);
		}
		else {
			packet.add<int8_t>(0);
			packet.add<int32_t>(0);
		}
	}
	player->getSession()->send(packet);
}

void PlayerPacket::showSkillMacros(Player *player, SkillMacros *macros) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_SKILL_MACRO);
	packet.add<int8_t>(macros->getMax() + 1);
	for (int8_t i = 0; i <= macros->getMax();  i++) {
		SkillMacros::SkillMacro *macro = macros->getSkillMacro(i);
		if (macro != 0) {
			packet.addString(macro->name);
			packet.add<int8_t>(macro->shout);
			packet.add<int32_t>(macro->skill1);
			packet.add<int32_t>(macro->skill2);
			packet.add<int32_t>(macro->skill3);
		}
		else {
			packet.addString("");
			packet.add<int8_t>(0);
			packet.add<int32_t>(0);
			packet.add<int32_t>(0);
			packet.add<int32_t>(0);
		}
	}
	
	player->getSession()->send(packet);
}

void PlayerPacket::updateStatInt(Player *player, int32_t id, int32_t value, bool is) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_UPDATE_STAT);
	packet.add<int8_t>(is);
	packet.add<int32_t>(id);
	packet.add<int32_t>(value);
	player->getSession()->send(packet);
}

void PlayerPacket::updateStatShort(Player *player, int32_t id, int16_t value, bool is) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_UPDATE_STAT);
	packet.add<int8_t>(is);
	packet.add<int32_t>(id);
	packet.add<int16_t>(value);
	player->getSession()->send(packet);
}

void PlayerPacket::updateStatChar(Player *player, int32_t id, int8_t value, bool is) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_UPDATE_STAT);
	packet.add<int8_t>(is);
	packet.add<int32_t>(id);
	packet.add<int8_t>(value);
	player->getSession()->send(packet);
}

void PlayerPacket::changeChannel(Player *player, uint32_t ip, int16_t port) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_CHANGE_CHANNEL);
	packet.add<int8_t>(1);
	packet.add<uint32_t>(htonl(ip)); // MapleStory accepts IP addresses in big-endian
	packet.add<int16_t>(port);
	player->getSession()->send(packet);
}

void PlayerPacket::showMessage(Player *player, const string &msg, int8_t type) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_NOTICE); 
	packet.add<int8_t>(type);
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
	packet.add<int16_t>(SEND_INSTRUCTION_BUBBLE);
	packet.addString(msg);
	packet.add<int16_t>(width);
	packet.add<int16_t>(height);
	packet.add<int8_t>(1);

	player->getSession()->send(packet);
}

void PlayerPacket::sendSound(Player *player, const string &soundname) { // Send Sound
	PacketCreator packet = PacketCreator();
	packet.add<int16_t>(SEND_MAP_EFFECT);
	packet.add<int8_t>(0x04);
	packet.addString(soundname);
	player->getSession()->send(packet);
}

void PlayerPacket::showHpBar(Player *player, Player *target) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_PARTY_HP_BAR);
	packet.add<int32_t>(player->getId());
	packet.add<int32_t>(player->getStats()->getHp());
	packet.add<int32_t>(player->getStats()->getMHp());
	target->getSession()->send(packet);
}