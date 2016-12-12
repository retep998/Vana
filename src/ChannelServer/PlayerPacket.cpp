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

namespace PlayerPacket {

PACKET_IMPL(connectData, Player *player) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_CHANGE_MAP)
		.add<int32_t>(ChannelServer::getInstance().getChannelId())
		.add<uint8_t>(player->getPortalCount(true))
		.add<bool>(true) // Is a connect packet
		.add<int16_t>(0); // Some amount for a funny message at the top of the screen

	if (false) {
		size_t lineAmount = 0;
		builder.add<string_t>("Message title");
		for (size_t i = 0; i < lineAmount; i++) {
			builder.add<string_t>("Line");
		}
	}

	player->initializeRng(builder);

	builder
		.add<int64_t>(-1) // Refers to packet sections that are included in here - all of them
		.add<player_id_t>(player->getId())
		.add<string_t>(player->getName(), 13)
		.add<gender_id_t>(player->getGender())
		.add<skin_id_t>(player->getSkin())
		.add<face_id_t>(player->getFace())
		.add<hair_id_t>(player->getHair());

	player->getPets()->connectData(builder);
	player->getStats()->connectData(builder); // Stats

	builder
		.add<int32_t>(0) // Gachapon EXP
		.add<map_id_t>(player->getMapId())
		.add<portal_id_t>(player->getMapPos())
		.add<int32_t>(0) // Unknown int32 added in .62
		.add<uint8_t>(player->getBuddyListSize());

	player->getSkills()->connectDataForBlessing(builder);
	player->getInventory()->connectData(builder);
	player->getSkills()->connectData(builder);
	player->getQuests()->connectData(builder);

	// Must do significant testing on all of the following to verify
	builder
		.add<int16_t>(0) // I think this is the minigame record
		.add<int16_t>(0) // I think this is the couple ring record
		.add<int16_t>(0) // I think this is the friendship ring record
		.add<int16_t>(0); // I think this is the marriage ring record

	player->getInventory()->rockPacket(builder); // Teleport Rock/VIP Rock maps
	player->getMonsterBook()->connectData(builder);

	builder
		.add<int16_t>(0) 
		.add<int16_t>(0) // Amount of pquests (or extended quests? Maybe related to Battleship?), for every quest: quest_id_t questId, string_t questdata
		.add<int16_t>(0)
		.add<int64_t>(TimeUtilities::getServerTime());
	return builder;
}

PACKET_IMPL(showKeys, KeyMaps *keyMaps) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_KEYMAP)
		.add<int8_t>(0);

	for (size_t i = 0; i < KeyMaps::size; i++) {
		KeyMaps::KeyMap *keyMap = keyMaps->getKeyMap(i);
		builder
			.add<int8_t>(keyMap == nullptr ? 0 : keyMap->type)
			.add<int32_t>(keyMap == nullptr ? 0 : keyMap->action);
	}
	return builder;
}

PACKET_IMPL(showSkillMacros, SkillMacros *macros) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_MACRO_LIST)
		.add<int8_t>(macros->getMax() + 1);

	for (int8_t i = 0; i <= macros->getMax(); i++) {
		SkillMacros::SkillMacro *macro = macros->getSkillMacro(i);
		if (macro != nullptr) {
			builder
				.add<string_t>(macro->name)
				.add<bool>(macro->shout)
				.add<int32_t>(macro->skill1)
				.add<int32_t>(macro->skill2)
				.add<int32_t>(macro->skill3);
		}
		else {
			builder
				.add<string_t>("")
				.add<bool>(false)
				.add<int32_t>(0)
				.add<int32_t>(0)
				.add<int32_t>(0);
		}
	}
	return builder;
}

PACKET_IMPL(updateStat, int32_t updateBits, int32_t value, bool itemResponse) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_PLAYER_UPDATE)
		.add<bool>(itemResponse)
		.add<int32_t>(updateBits);

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
			builder.add<int16_t>(static_cast<int16_t>(value));
			break;
		case Stats::Skin:
		case Stats::Face:
		case Stats::Hair:
		case Stats::Exp:
		case Stats::Fame:
		case Stats::Mesos:
			builder.add<int32_t>(value);
			break;
	}
	builder.add<int32_t>(value);
	return builder;
}

PACKET_IMPL(changeChannel, const Ip &ip, port_t port) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_CHANNEL_CHANGE)
		.add<bool>(true)
		.add<ClientIp>(ClientIp(ip))
		.add<port_t>(port);
	return builder;
}

PACKET_IMPL(showMessage, const chat_t &msg, int8_t type) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_MESSAGE)
		.add<int8_t>(type)
		.add<chat_t>(msg);

	if (type == NoticeTypes::Blue) {
		builder.add<int32_t>(0);
	}
	return builder;
}

PACKET_IMPL(groupChat, const string_t &name, const chat_t &msg, int8_t type) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_MESSAGE_GROUP)
		.add<int8_t>(type)
		.add<string_t>(name)
		.add<chat_t>(msg);
	return builder;
}

PACKET_IMPL(instructionBubble, const chat_t &msg, coord_t width, int16_t time, bool isStatic, int32_t x, int32_t y) {
	PacketBuilder builder;

	if (width == -1) {
		width = msg.size() * 10;
		if (width < 40) {
			width = 40; // Anything lower crashes client/doesn't look good
		}
	}

	builder
		.add<header_t>(SMSG_BUBBLE)
		.add<chat_t>(msg)
		.add<coord_t>(width)
		.add<int16_t>(time)
		.add<bool>(!isStatic);

	if (isStatic) {
		builder
			.add<int32_t>(x)
			.add<int32_t>(y);
	}
	return builder;
}

PACKET_IMPL(showHpBar, player_id_t playerId, int32_t hp, int32_t maxHp) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_PARTY_HP_DISPLAY)
		.add<player_id_t>(playerId)
		.add<int32_t>(hp)
		.add<int32_t>(maxHp);
	return builder;
}

PACKET_IMPL(sendBlockedMessage, int8_t type) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_CHANNEL_BLOCKED)
		.add<int8_t>(type);
	return builder;
}

PACKET_IMPL(sendYellowMessage, const chat_t &msg) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_YELLOW_MESSAGE)
		.add<bool>(true)
		.add<chat_t>(msg);
	return builder;
}

}