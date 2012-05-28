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
#include "PlayerPacket.h"
#include "ChannelServer.h"
#include "InterHeader.h"
#include "KeyMaps.h"
#include "PacketCreator.h"
#include "Pet.h"
#include "Player.h"
#include "PlayerDataProvider.h"
#include "PlayerPacketHelper.h"
#include "Session.h"
#include "SkillMacros.h"
#include "Skills.h"
#include "SmsgHeader.h"
#include "TimeUtilities.h"
#include <unordered_map>

using std::unordered_map;

void PlayerPacket::connectData(Player *player) {
	PacketCreator packet;
	packet.add<header_t>(SMSG_CHANGE_MAP);
	{
		packet.add<int16_t>(2); // Options
		packet.add<int32_t>(1);
		packet.add<int32_t>(0);
		packet.add<int32_t>(2); // This one is actually used
		packet.add<int32_t>(0);
	}

	packet.add<int32_t>(ChannelServer::Instance()->getChannel());
	packet.add<uint8_t>(0);
	packet.add<int32_t>(0);
	packet.add<uint8_t>(player->getPortalCount(true));

	packet.add<int32_t>(0); // New

	packet.addBool(true); // Is a connect packet

	packet.add<int16_t>(0); // Some amount for a funny message at the top of the screen
	if (false) {
		size_t lineAmount = 0;
		packet.addString("Message title");
		for (size_t i = 0; i < lineAmount; i++) {
			packet.addString("Line");
		}
	}

	player->initializeRng(packet);

	packet.addBytes("FFFFFFFFFDFFFFFF"); // No more -1!

	{
		packet.add<int8_t>(0);

		int8_t amount = 0;
		packet.add<int8_t>(amount);
		for (int8_t i = 0; i < amount; i++) {
			packet.add<int32_t>(0);
		}
	}

	{
		size_t amount = 0;
		packet.add<int32_t>(amount);
		for (size_t i = 0; i < amount; i++) {
			packet.add<int32_t>(0);
			packet.add<int64_t>(0);
		}
	}

	{
		bool value = false;
		packet.addBool(value);
		if (value) {
			packet.add<int8_t>(0); // Not used

			size_t amount = 0;
			packet.add<int32_t>(amount);
			for (size_t i = 0; i < amount; i++) {
				packet.add<int64_t>(0);
			}

			amount = 0;
			packet.add<int32_t>(amount);
			for (size_t i = 0; i < amount; i++) {
				packet.add<int64_t>(0);
			}
		}
	}

	packet.add<int32_t>(player->getId());
	packet.addString(player->getName(), 13);
	packet.add<int8_t>(player->getGender());
	packet.add<int8_t>(player->getSkin());
	packet.add<int32_t>(player->getEyes());
	packet.add<int32_t>(player->getHair());

	player->getPets()->connectData(packet);
	player->getStats()->connectData(packet);

	packet.add<int32_t>(0); // Gachapon EXP

	packet.add<int32_t>(player->getMap());
	packet.add<int8_t>(player->getMappos());

	packet.add<int32_t>(0); // Unknown int32 added in .62

	// New
	packet.add<int16_t>(0);//player->getStats()->getJobType()); // For Cannoneer and Dual Blader
	// TODO: Implement
	if (player->getStats()->getJob() / 100 == Jobs::JobTracks::DemonSlayer || player->getStats()->getJob() == Jobs::JobIds::DemonSlayer) {
		packet.add<int32_t>(1012280);
	}

	packet.add<uint8_t>(player->getBuddyListSize());

	{
		time_t rawtime = time(NULL);
		struct tm * timeinfo;
		timeinfo = localtime ( &rawtime );

		char buff[15];
		sprintf(buff, "%04d%02d%02d%02d", (timeinfo->tm_year + 1900), (timeinfo->tm_mon + 1), timeinfo->tm_mday, timeinfo->tm_hour);
		packet.add<int32_t>(atoi(buff)); // YYYYMMDDHH
	}

	packet.add<int32_t>(0);
	packet.add<int32_t>(0);

	packet.add<int32_t>(0);
	packet.add<int32_t>(0);

	packet.add<int32_t>(0);
	packet.add<int32_t>(0);

	// 12 bytes?
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	
	packet.add<int32_t>(0);

	packet.add<int8_t>(10);
	packet.add<int32_t>(0);
	packet.add<int8_t>(5);
	packet.add<int32_t>(0);

	packet.add<uint64_t>(18293692055145101115); // Filetime thing?

	// BACK TO THE MAIN FUNC

	packet.add<int8_t>(20);

	{
		bool value = false;
		packet.addBool(value);
		if (value) {
			packet.addString("Meerchars4");
		}
	}
	{
		bool value = false;
		packet.addBool(value);
		if (value) {
			packet.addString("Meerchars4");
		}
	}
	{
		bool value = false;
		packet.addBool(value);
		if (value) {
			packet.addString("?????????");
		}
	}

	player->getInventory()->connectData(packet); // Inventory data

	{
		packet.add<int32_t>(-1); // END IT ALREADY (It's a loop)
	}
	
	{
		size_t amount = 0;
		packet.add<int32_t>(amount);
		for (size_t i = 0; i < amount; i++) {
			packet.add<int32_t>(0);
			packet.add<int64_t>(0);
		}
	}
	
	{
		size_t amount = 0;
		packet.add<int32_t>(amount);
		for (size_t i = 0; i < amount; i++) {
			packet.add<int64_t>(0);
			packet.add<int64_t>(0);
		}
	}

	{
		{
			/*
			packet.add<int8_t>(!0);

			packet.add<int32_t>(0);
			packet.add<int8_t>(0);
			packet.add<int8_t>(0);
			packet.add<int32_t>(0);
			packet.add<int32_t>(0);
			packet.add<int32_t>(0);
			packet.add<int32_t>(0);
			packet.add<int8_t>(0);
			packet.add<int32_t>(0);
			packet.add<int64_t>(0);
			packet.add<int64_t>(0);
			packet.add<int64_t>(0);
			packet.add<int64_t>(0);
			*/
		}
		packet.add<int8_t>(0);
	}

	player->getSkills()->connectData(packet); // Skills - levels and cooldowns
	player->getQuests()->connectData(packet); // Quests

	packet.add<int16_t>(0); // Match Record
	packet.add<int16_t>(0); // Couple Record
	packet.add<int16_t>(0); // 'Friend' Record
	packet.add<int16_t>(0); // Marriage Record

	player->getInventory()->rockPacket(packet); // Teleport Rock maps
	player->getMonsterBook()->connectData(packet);
	
	packet.add<uint32_t>(0xFFFFFFFF); // ??

	packet.add<int16_t>(0);
	// Foreach: short

	packet.add<int16_t>(0); // Newyear giftcard data
	/*
	Foreach
		packet.add<int32_t>(0);
		packet.add<int32_t>(0);
		packet.addString("Probably Name");
		packet.add<int8_t>(0);
		packet.add<int64_t>(0); // Sent time?
		packet.add<int32_t>(0);
		packet.addString("Message?");
		packet.add<int8_t>(0);
		packet.add<int8_t>(0);
		packet.add<int64_t>(0);
		packet.addString("????");

	*/

	// Party Quest data (quest needs to be added in the quests list)
	packet.add<int16_t>(0); // Amount of pquests
	// Foreach: Short (questid) + String (content)

	if (GameLogicUtilities::isWildHunter(player->getStats()->getJob())) {
		packet.add<int8_t>(0); // ?
		packet.add<int32_t>(0);
		packet.add<int32_t>(0);
		packet.add<int32_t>(0);
		packet.add<int32_t>(0);
		packet.add<int32_t>(0);
	}

	packet.add<int16_t>(0);
	// Foreach: Short + Long (filetime) ?
	
	packet.add<int16_t>(0);
	/*
	Foreach:
		packet.add<int16_t>(0);
		packet.add<int32_t>(0);
		????
	*/

	packet.add<int16_t>(0);
	
	for (int i = 0; i < 17; i++) {
		packet.add<int32_t>(0); // UNKNOWN
	}
	packet.add<int16_t>(0);
	packet.add<int16_t>(1);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);

	packet.add<int8_t>(0);
	packet.add<int16_t>(0);
	packet.add<int16_t>(0);

	packet.add<int16_t>(0);

	packet.add<int32_t>(0);

	packet.add<int64_t>(TimeUtilities::getServerTime());
	packet.add<int32_t>(100);
	packet.add<int8_t>(0);
	packet.add<int8_t>(1);
	player->getSession()->send(packet);
}

void PlayerPacket::showKeys(Player *player, KeyMaps *keyMaps) {
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

void PlayerPacket::showSkillMacros(Player *player, SkillMacros *macros) {
	PacketCreator packet;
	packet.add<header_t>(SMSG_MACRO_LIST);
	packet.add<int8_t>(macros->getMax() + 1);
	for (int8_t i = 0; i <= macros->getMax(); i++) {
		SkillMacros::SkillMacro *macro = macros->getSkillMacro(i);
		if (macro != nullptr) {
			packet.addString(macro->name);
			packet.addBool(macro->shout);
			packet.add<int32_t>(macro->skill1);
			packet.add<int32_t>(macro->skill2);
			packet.add<int32_t>(macro->skill3);
		}
		else {
			packet.addString("");
			packet.addBool(false);
			packet.add<int32_t>(0);
			packet.add<int32_t>(0);
			packet.add<int32_t>(0);
		}
	}

	player->getSession()->send(packet);
}

void PlayerPacket::updateStat(Player *player, int64_t updateBits, int64_t value, bool itemResponse) {
	PacketCreator packet;
	packet.add<header_t>(SMSG_PLAYER_UPDATE);
	packet.addBool(itemResponse);
	packet.add<int64_t>(updateBits);
	switch (updateBits) {
		// For now it only accepts updateBits as a single unit, might be a collection later
		case Stats::Skin:
		case Stats::Level:
		case 0x400000:
		case 0x80000000:
			packet.add<int8_t>(static_cast<int8_t>(value));
			break;

		case Stats::Job:
		case Stats::Str:
		case Stats::Dex:
		case Stats::Int:
		case Stats::Luk:
		case Stats::Ap:
			packet.add<int16_t>(static_cast<int16_t>(value));
			break;

		case Stats::Pet:
		case 0x80000:
		case 0x100000:
		case 0x20000000:
			packet.add<int64_t>(value);
			break;

		case Stats::Sp:
			player->getStats()->addSpData(packet);
			break;

		case 0: break;
		default:
			packet.add<int32_t>(static_cast<int32_t>(value));
			break;
	}
	
	packet.addBool(false);
	if (false) {
		packet.add<int8_t>(0);
	}

	packet.addBool(false);
	if (false) {
		packet.add<int32_t>(0); // Battle Record
		packet.add<int32_t>(0);
	}

	player->getSession()->send(packet);
}

void PlayerPacket::changeChannel(Player *player, ip_t ip, port_t port) {
	PacketCreator packet;
	packet.add<header_t>(SMSG_CHANNEL_CHANGE);
	packet.addBool(true);
	packet.add<ip_t>(htonl(ip)); // MapleStory accepts IP addresses in big-endian
	packet.add<port_t>(port);
	packet.add<int8_t>(0);
	player->getSession()->send(packet);
}

void PlayerPacket::showMessage(Player *player, const string &msg, int8_t type) {
	PacketCreator packet;
	showMessagePacket(packet, msg, type);
	player->getSession()->send(packet);
}

void PlayerPacket::showMessageChannel(const string &msg, int8_t type) {
	PacketCreator packet;
	showMessagePacket(packet, msg, type);
	PlayerDataProvider::Instance()->sendPacket(packet);
}

void PlayerPacket::showMessageWorld(const string &msg, int8_t type) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_TO_CHANNELS);
	packet.add<int16_t>(IMSG_TO_PLAYERS);
	showMessagePacket(packet, msg, type);
	ChannelServer::Instance()->sendToWorld(packet);
}

void PlayerPacket::showMessageGlobal(const string &msg, int8_t type) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_TO_LOGIN);
	packet.add<int16_t>(IMSG_TO_WORLDS);
	packet.add<int16_t>(IMSG_TO_CHANNELS);
	packet.add<int16_t>(IMSG_TO_PLAYERS);
	showMessagePacket(packet, msg, type);
	ChannelServer::Instance()->sendToWorld(packet);
}

void PlayerPacket::showMessagePacket(PacketCreator &packet, const string &msg, int8_t type) {
	packet.add<header_t>(SMSG_MESSAGE);
	packet.add<int8_t>(type);
	packet.addString(msg);
	if (type == NoticeTypes::Blue) {
		packet.add<int32_t>(0);
	}
}

void PlayerPacket::instructionBubble(Player *player, const string &msg, int16_t width, int16_t time, bool isStatic, int32_t x, int32_t y) {
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
	packet.addBool(!isStatic);

	if (isStatic) {
		packet.add<int32_t>(x);
		packet.add<int32_t>(y);
	}

	player->getSession()->send(packet);
}

void PlayerPacket::showHpBar(Player *player, Player *target) {
	PacketCreator packet;
	packet.add<header_t>(SMSG_PARTY_HP_DISPLAY);
	packet.add<int32_t>(player->getId());
	packet.add<int32_t>(player->getStats()->getHp());
	packet.add<int32_t>(player->getStats()->getMaxHp());
	target->getSession()->send(packet);
}

void PlayerPacket::sendBlockedMessage(Player *player, int8_t type) {
	PacketCreator packet;
	packet.add<header_t>(SMSG_CHANNEL_BLOCKED);
	packet.add<int8_t>(type);
	player->getSession()->send(packet);
}

void PlayerPacket::sendYellowMessage(Player *player, const string &msg) {
	PacketCreator packet;
	packet.add<header_t>(SMSG_YELLOW_MESSAGE);
	packet.addBool(true);
	packet.addString(msg);
	player->getSession()->send(packet);
}

void PlayerPacket::sendMulticolorMessage(Player *player, MulticolorMessage::Colors color, const string &msg) {
	PacketCreator packet;
	packet.add<header_t>(SMSG_CHAT_MULTICOLOR);
	packet.add<uint16_t>(color);
	packet.addString(msg);
	player->getSession()->send(packet);
}