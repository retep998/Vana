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
#include "PlayerPacket.hpp"
#include "ChannelServer.hpp"
#include "ClientIp.hpp"
#include "InterHeader.hpp"
#include "KeyMaps.hpp"
#include "PacketCreator.hpp"
#include "Pet.hpp"
#include "Player.hpp"
#include "PlayerDataProvider.hpp"
#include "PlayerPacketHelper.hpp"
#include "Session.hpp"
#include "SkillMacros.hpp"
#include "Skills.hpp"
#include "SmsgHeader.hpp"
#include "TimeUtilities.hpp"
#include <unordered_map>

auto PlayerPacket::connectData(Player *player) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_CHANGE_MAP);
	packet.add<int32_t>(ChannelServer::getInstance().getChannelId());
	packet.add<uint8_t>(player->getPortalCount(true));
	packet.add<bool>(true); // Is a connect packet
	packet.add<int16_t>(0); // Some amount for a funny message at the top of the screen
	if (false) {
		size_t lineAmount = 0;
		packet.addString("Message title");
		for (size_t i = 0; i < lineAmount; i++) {
			packet.addString("Line");
		}
	}

	player->initializeRng(packet);

	packet.add<int64_t>(-1);
	packet.add<int32_t>(player->getId());
	packet.addString(player->getName(), 13);
	packet.add<int8_t>(player->getGender());
	packet.add<int8_t>(player->getSkin());
	packet.add<int32_t>(player->getEyes());
	packet.add<int32_t>(player->getHair());

	player->getPets()->connectData(packet);
	player->getStats()->connectData(packet); // Stats

	packet.add<int32_t>(0); // Gachapon EXP

	packet.add<int32_t>(player->getMapId());
	packet.add<int8_t>(player->getMapPos());

	packet.add<int32_t>(0); // Unknown int32 added in .62

	packet.add<uint8_t>(player->getBuddyListSize());

	player->getInventory()->connectData(packet); // Inventory data
	player->getSkills()->connectData(packet); // Skills - levels and cooldowns
	player->getQuests()->connectData(packet); // Quests

	packet.add<int32_t>(0);
	packet.add<int32_t>(0);

	player->getInventory()->rockPacket(packet); // Teleport Rock/VIP Rock maps
	player->getMonsterBook()->connectData(packet);

	packet.add<int16_t>(0);

	// Party Quest data (quest needs to be added in the quests list)
	packet.add<int16_t>(0); // Amount of pquests
	// for every pquest: int16_t questId, string questdata

	packet.add<int16_t>(0);

	packet.add<int64_t>(TimeUtilities::getServerTime());
	player->getSession()->send(packet);
}

auto PlayerPacket::showKeys(Player *player, KeyMaps *keyMaps) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_KEYMAP);
	packet.add<int8_t>(0);
	for (size_t i = 0; i < KeyMaps::size; i++) {
		KeyMaps::KeyMap *keyMap = keyMaps->getKeyMap(i);
		if (keyMap != nullptr) {
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

auto PlayerPacket::showSkillMacros(Player *player, SkillMacros *macros) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_MACRO_LIST);
	packet.add<int8_t>(macros->getMax() + 1);
	for (int8_t i = 0; i <= macros->getMax(); i++) {
		SkillMacros::SkillMacro *macro = macros->getSkillMacro(i);
		if (macro != nullptr) {
			packet.addString(macro->name);
			packet.add<bool>(macro->shout);
			packet.add<int32_t>(macro->skill1);
			packet.add<int32_t>(macro->skill2);
			packet.add<int32_t>(macro->skill3);
		}
		else {
			packet.addString("");
			packet.add<bool>(false);
			packet.add<int32_t>(0);
			packet.add<int32_t>(0);
			packet.add<int32_t>(0);
		}
	}

	player->getSession()->send(packet);
}

auto PlayerPacket::updateStat(Player *player, int32_t updateBits, int32_t value, bool itemResponse) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_PLAYER_UPDATE);
	packet.add<bool>(itemResponse);
	packet.add<int32_t>(updateBits);
	switch (updateBits) {
		// For now it only accepts updateBits as a single unit, might be a collection later
		case Stats::Pet:
		case Stats::Level:
		case Stats::Job:
		case Stats::Str:
		case Stats::Dex:
		case Stats::Int:
		case Stats::Luk:
		case Stats::Hp:
		case Stats::MaxHp:
		case Stats::Mp:
		case Stats::MaxMp:
		case Stats::Ap:
		case Stats::Sp:
			packet.add<int16_t>(static_cast<int16_t>(value));
			break;
		case Stats::Skin:
		case Stats::Eyes:
		case Stats::Hair:
		case Stats::Exp:
		case Stats::Fame:
		case Stats::Mesos:
			packet.add<int32_t>(value);
			break;
	}
	packet.add<int32_t>(value);
	player->getSession()->send(packet);
}

auto PlayerPacket::changeChannel(Player *player, const Ip &ip, port_t port) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_CHANNEL_CHANGE);
	packet.add<bool>(true);
	packet.addClass<ClientIp>(ClientIp(ip)); // MapleStory accepts IP addresses in big-endian
	packet.add<port_t>(port);
	player->getSession()->send(packet);
}

auto PlayerPacket::showMessage(Player *player, const string_t &msg, int8_t type) -> void {
	PacketCreator packet;
	showMessagePacket(packet, msg, type);
	player->getSession()->send(packet);
}

auto PlayerPacket::showMessageChannel(const string_t &msg, int8_t type) -> void {
	PacketCreator packet;
	showMessagePacket(packet, msg, type);
	PlayerDataProvider::getInstance().sendPacket(packet);
}

auto PlayerPacket::showMessageWorld(const string_t &msg, int8_t type) -> void {
	PacketCreator packet;
	packet.add<header_t>(IMSG_TO_ALL_CHANNELS);
	packet.add<header_t>(IMSG_TO_ALL_PLAYERS);
	showMessagePacket(packet, msg, type);
	ChannelServer::getInstance().sendPacketToWorld(packet);
}

auto PlayerPacket::showMessageGlobal(const string_t &msg, int8_t type) -> void {
	PacketCreator packet;
	packet.add<header_t>(IMSG_TO_LOGIN);
	packet.add<header_t>(IMSG_TO_ALL_WORLDS);
	packet.add<header_t>(IMSG_TO_ALL_CHANNELS);
	packet.add<header_t>(IMSG_TO_ALL_PLAYERS);
	showMessagePacket(packet, msg, type);
	ChannelServer::getInstance().sendPacketToWorld(packet);
}

auto PlayerPacket::showMessagePacket(PacketCreator &packet, const string_t &msg, int8_t type) -> void {
	packet.add<header_t>(SMSG_MESSAGE);
	packet.add<int8_t>(type);
	packet.addString(msg);
	if (type == NoticeTypes::Blue) {
		packet.add<int32_t>(0);
	}
}

auto PlayerPacket::instructionBubble(Player *player, const string_t &msg, int16_t width, int16_t time, bool isStatic, int32_t x, int32_t y) -> void {
	if (width == -1) {
		width = msg.size() * 10;
		if (width < 40) {
			width = 40; // Anything lower crashes client/doesn't look good
		}
	}

	PacketCreator packet;
	packet.add<header_t>(SMSG_BUBBLE);
	packet.addString(msg);
	packet.add<int16_t>(width);
	packet.add<int16_t>(time);
	packet.add<bool>(!isStatic);

	if (isStatic) {
		packet.add<int32_t>(x);
		packet.add<int32_t>(y);
	}

	player->getSession()->send(packet);
}

auto PlayerPacket::showHpBar(Player *player, Player *target) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_PARTY_HP_DISPLAY);
	packet.add<int32_t>(player->getId());
	packet.add<int32_t>(player->getStats()->getHp());
	packet.add<int32_t>(player->getStats()->getMaxHp());
	target->getSession()->send(packet);
}

auto PlayerPacket::sendBlockedMessage(Player *player, int8_t type) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_CHANNEL_BLOCKED);
	packet.add<int8_t>(type);
	player->getSession()->send(packet);
}

auto PlayerPacket::sendYellowMessage(Player *player, const string_t &msg) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_YELLOW_MESSAGE);
	packet.add<bool>(true);
	packet.addString(msg);
	player->getSession()->send(packet);
}