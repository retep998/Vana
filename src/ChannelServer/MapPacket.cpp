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
#include "MapPacket.h"
#include "Buffs.h"
#include "ChannelServer.h"
#include "Inventory.h"
#include "Maps.h"
#include "Mist.h"
#include "PacketCreator.h"
#include "Pet.h"
#include "Player.h"
#include "PlayerPacketHelper.h"
#include "Session.h"
#include "SmsgHeader.h"
#include "TimeUtilities.h"
#include <unordered_map>

using std::unordered_map;

PacketCreator MapPacket::playerPacket(Player *player) {
	PacketCreator packet;
	MapEntryBuffs enter = player->getActiveBuffs()->getMapEntryBuffs();
	packet.add<header_t>(SMSG_MAP_PLAYER_SPAWN);
	packet.add<int32_t>(player->getId());
	packet.add<uint8_t>(player->getStats()->getLevel()); // New
	packet.addString(player->getName());
	packet.addString("HERP"); // New
	packet.addString("DERP"); // Guild
	packet.add<int16_t>(0); // Guild icon garbage
	packet.add<int8_t>(0); // Guild icon garbage
	packet.add<int16_t>(0); // Guild icon garbage
	packet.add<int8_t>(0); // Guild icon garbage

	{
		using namespace BuffBytes; // 32 bytes the fuck

		packet.add<uint8_t>(enter.types[Byte29]);
		packet.add<uint8_t>(enter.types[Byte30]);
		packet.add<uint8_t>(enter.types[Byte31]);
		packet.add<uint8_t>(enter.types[Byte32]);

		packet.add<uint8_t>(enter.types[Byte25]);
		packet.add<uint8_t>(enter.types[Byte26]);
		packet.add<uint8_t>(enter.types[Byte27]);
		packet.add<uint8_t>(enter.types[Byte28]);

		packet.add<uint8_t>(enter.types[Byte21]);
		packet.add<uint8_t>(enter.types[Byte22]);
		packet.add<uint8_t>(enter.types[Byte23]);
		packet.add<uint8_t>(enter.types[Byte24]);

		packet.add<uint8_t>(enter.types[Byte17]);
		packet.add<uint8_t>(enter.types[Byte18]);
		packet.add<uint8_t>(enter.types[Byte19]);
		packet.add<uint8_t>(enter.types[Byte20]);

		packet.add<uint8_t>(enter.types[Byte13]);
		packet.add<uint8_t>(enter.types[Byte14]);
		packet.add<uint8_t>(enter.types[Byte15]);
		packet.add<uint8_t>(enter.types[Byte16]);

		packet.add<uint8_t>(enter.types[Byte9]); // 0x10, 0x20, 0x40, 0x80, 0x08
		packet.add<uint8_t>(enter.types[Byte10]); // 0x01, 0x02
		packet.add<uint8_t>(enter.types[Byte11]);
		packet.add<uint8_t>(enter.types[Byte12]);

		packet.add<uint8_t>(enter.types[Byte5]);
		packet.add<uint8_t>(enter.types[Byte6]);
		packet.add<uint8_t>(enter.types[Byte7]);
		packet.add<uint8_t>(enter.types[Byte8]);

		packet.add<uint8_t>(enter.types[Byte1]);
		packet.add<uint8_t>(enter.types[Byte2]);
		packet.add<uint8_t>(enter.types[Byte3]);
		packet.add<uint8_t>(enter.types[Byte4]);

		const int8_t byteorder[EntryByteQuantity] = { 
		Byte1,		Byte2,		Byte3,		Byte4,
		Byte5,		Byte6,		Byte7,		Byte8,
		Byte9,		Byte10,		Byte11,		Byte12,
		Byte13,		Byte14,		Byte15,		Byte16,
		Byte17,		Byte18,		Byte19,		Byte20,
		Byte21,		Byte22,		Byte23,		Byte24,
		Byte25,		Byte26,		Byte27,		Byte28,
		Byte29,		Byte30,		Byte31,		Byte32
		};

		for (int8_t i = 0; i < EntryByteQuantity; i++) {
			int8_t cbyte = byteorder[i]; // Values are sorted by lower bytes first
			if (enter.types[cbyte] != 0) {
				for (unordered_map<uint8_t, MapEntryVals>::iterator iter = enter.values[cbyte].begin(); iter != enter.values[cbyte].end(); ++iter) {
					if (iter->second.debuff) {
						if (!(iter->first == 0x01 && cbyte == Byte5)) { // Glitch in global, Slow doesn't display properly and if you try, it error 38s
							packet.add<int16_t>(iter->second.skill);
							packet.add<int16_t>(iter->second.val);
						}
					}
					else if (iter->second.use) {
						int16_t value = iter->second.val;
						if (cbyte == Byte3) {
							if (iter->first == 0x20) {
								packet.add<int8_t>(player->getActiveBuffs()->getCombo() + 1);
							}
							if (iter->first == 0x40) {
								packet.add<int32_t>(player->getActiveBuffs()->getCharge());
							}
						}
						else if (cbyte == Byte5) {
							packet.add<int16_t>(value);
						}
						else {
							packet.add<int8_t>(static_cast<int8_t>(value));
						}
					}
				}
			}
		}

		packet.add<int8_t>(0);

		packet.add<int8_t>(0);

		packet.add<int8_t>(0);

		/*
		packet.add<int16_t>(0);

		packet.add<int32_t>(0);
		packet.add<int32_t>(0);
		packet.add<int32_t>(1065638850); // Server tick count or something
		packet.add<int8_t>(0);
		packet.add<int16_t>(0);

		packet.add<int32_t>(0);
		packet.add<int32_t>(0);
		packet.add<int32_t>(1065638850);
		packet.add<int8_t>(0);
		packet.add<int16_t>(0);

		packet.add<int32_t>(0);
		packet.add<int32_t>(0);
		packet.add<int32_t>(1065638850);
		packet.add<int16_t>(0);
		packet.add<int8_t>(0);


		packet.add<int32_t>(enter.mountId); // No point to having an if, these are 0 when not in use
		packet.add<int32_t>(enter.mountSkill);
		packet.add<int32_t>(1065638850);
		packet.add<int8_t>(0);

		packet.add<int32_t>(0);
		packet.add<int32_t>(0);
		packet.add<int32_t>(1065638850);
		packet.add<int8_t>(0);
		packet.add<int16_t>(0);

		packet.add<int8_t>(0);
		packet.add<int16_t>(0);
		packet.add<int16_t>(0);

		packet.add<int32_t>(0);
		packet.add<int32_t>(0);
		packet.add<int32_t>(1065638850);
		packet.add<int8_t>(0);
		packet.add<int16_t>(0);

		packet.add<int16_t>(0);

		packet.add<int32_t>(0);
		packet.add<int32_t>(0);
		packet.add<int32_t>(1065638850);
		packet.add<int8_t>(0);
		packet.add<int16_t>(0);
		*/
	}

	packet.add<int16_t>(0); // New
	
	
	packet.add<int16_t>(player->getStats()->getJob());

	PlayerPacketHelper::addPlayerDisplay(packet, player);

	packet.add<int32_t>(0);
	packet.add<int32_t>(0);

	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0); // Foreach -> decode 2 ints?

	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);

	packet.add<int32_t>(0);
	packet.add<int32_t>(player->getItemEffect());
	packet.add<int32_t>(player->getChair());
	packet.add<int32_t>(0);
	packet.addPos(player->getPos());
	packet.add<int8_t>(player->getStance());
	packet.add<int16_t>(player->getFh());
	packet.add<int8_t>(0);

	for (int8_t i = 0; i < Inventories::MaxPetCount; i++) {
		if (Pet *pet = player->getPets()->getSummoned(i)) {
			packet.add<int8_t>(1);
			packet.add<int32_t>(pet->getItemId());
			packet.addString(pet->getName());
			packet.add<int64_t>(pet->getId());
			packet.addPos(pet->getPos());
			packet.add<int8_t>(pet->getStance());
			packet.add<int16_t>(pet->getFh());
			packet.addBool(pet->hasNameTag());
			packet.addBool(pet->hasQuoteItem());
		}
	}
	packet.add<int8_t>(0);
	
	{
		packet.add<int8_t>(1);

		packet.addBool(false);
		/*
		Foreach:
			if (Bool) {
				INT
				INT
				INT
				STR
				??????
				SHRT
				SHRT
				BYTE
				SHRT
			}
		*/
	}

	packet.add<int32_t>(1);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);

	packet.addBool(false);
	/*
	// MINIROOM
	if (Bool) {
	INT
	STR
	BYTE
	BYTE
	BYTE
	BYTE
	BYTE

	}
	*/

	if (!player->getChalkboard().empty()) {
		packet.addBool(true);
		packet.addString(player->getChalkboard());
	}
	else {
		packet.addBool(false);
	}
	packet.addBool(false); // LONG LONG
	packet.addBool(false); // LONG LONG

	packet.addBool(false); // INT INT INT 

	packet.add<int8_t>(0); // FLAG
	/*
	if (flag & 0x01) ???
	if (flag & 0x02) ???
	if (flag & 0x04) ???
	if (flag & 0x08) INT + get_update_time()
	if (flag & 0x10) INT + get_update_time()
	if (flag & 0x20) INT (if < 5000 -> do something??)
	*/

	packet.add<int32_t>(0);

	packet.addBool(false);
	/*
	if (Bool) {
	val1 = INT
	for (0..val1) {
	INT
	}
	}

	*/

	packet.add<int32_t>(0);

	// End?

	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int16_t>(0);

	
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	return packet;
}

void MapPacket::showPlayer(Player *player) {
	PacketCreator packet = playerPacket(player);
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void MapPacket::removePlayer(Player *player) {
	PacketCreator packet;
	packet.add<header_t>(SMSG_MAP_PLAYER_DESPAWN);
	packet.add<int32_t>(player->getId());
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void MapPacket::changeMap(Player *player) {
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

	packet.addBool(false); // Not a connect packet
	packet.add<int16_t>(0); // Some amount for a funny message at the top of the screen
	if (false) {
		size_t lineAmount = 0;
		packet.addString("Message title");
		for (size_t i = 0; i < lineAmount; i++) {
			packet.addString("Line");
		}
	}

	packet.addBool(false);

	packet.add<int32_t>(player->getMap());
	packet.add<int8_t>(player->getMappos());
	packet.add<int32_t>(player->getStats()->getHp());
	packet.addBool(false);
	if (false) {
		packet.add<int32_t>(0);
		packet.add<int32_t>(0);
	}
	packet.add<int64_t>(TimeUtilities::getServerTime());
	packet.add<int32_t>(100);
	packet.add<int8_t>(0);
	packet.add<int8_t>(1);
	player->getSession()->send(packet);
}

void MapPacket::portalBlocked(Player *player) {
	PacketCreator packet;
	packet.add<header_t>(SMSG_PLAYER_UPDATE);
	packet.add<int8_t>(1);
	packet.add<int64_t>(0);
	packet.addBool(false);
	packet.addBool(false);
	player->getSession()->send(packet);
}

void MapPacket::showClock(Player *player, int8_t hour, int8_t min, int8_t sec) {
	PacketCreator packet;
	packet.add<header_t>(SMSG_TIMER);
	packet.add<int8_t>(0x01);
	packet.add<int8_t>(hour);
	packet.add<int8_t>(min);
	packet.add<int8_t>(sec);
	player->getSession()->send(packet);
}

void MapPacket::showTimer(int32_t mapId, int32_t sec) {
	PacketCreator packet;
	if (sec > 0) {
		packet.add<header_t>(SMSG_TIMER);
		packet.add<int8_t>(0x02);
		packet.add<int32_t>(sec);
	}
	else {
		packet.add<header_t>(SMSG_TIMER_OFF);
	}
	Maps::getMap(mapId)->sendPacket(packet);
}

void MapPacket::showTimer(Player *player, int32_t sec) {
	PacketCreator packet;
	if (sec > 0) {
		packet.add<header_t>(SMSG_TIMER);
		packet.add<int8_t>(0x02);
		packet.add<int32_t>(sec);
	}
	else {
		packet.add<header_t>(SMSG_TIMER_OFF);
	}
	player->getSession()->send(packet);
}

void MapPacket::forceMapEquip(Player *player) {
	PacketCreator packet;
	packet.add<header_t>(SMSG_MAP_FORCE_EQUIPMENT);
	player->getSession()->send(packet);
}

void MapPacket::showEventInstructions(int32_t mapId) {
	PacketCreator packet;
	packet.add<header_t>(SMSG_EVENT_INSTRUCTION);
	packet.add<int8_t>(0x00);
	Maps::getMap(mapId)->sendPacket(packet);
}

void MapPacket::showMist(Player *player, Mist *mist) {
	PacketCreator packet;
	packet.add<header_t>(SMSG_MIST_SPAWN);
	packet.add<int32_t>(mist->getId());
	packet.add<int32_t>(mist->isMobMist() ? 0 : mist->isPoison() ? 1 : 2);
	packet.add<int32_t>(mist->getOwnerId());
	packet.add<int32_t>(mist->getSkillId());
	packet.add<uint8_t>(mist->getSkillLevel());
	packet.add<int16_t>(0);
	packet.addPos(mist->getLt(), true);
	packet.addPos(mist->getRb(), true);
	packet.add<int32_t>(0);
	player->getSession()->send(packet);
}

void MapPacket::spawnMist(int32_t mapId, Mist *mist) {
	PacketCreator packet;
	packet.add<header_t>(SMSG_MIST_SPAWN);
	packet.add<int32_t>(mist->getId());
	packet.add<int32_t>(mist->isMobMist() ? 0 : mist->isPoison() ? 1 : 2);
	packet.add<int32_t>(mist->getOwnerId());
	packet.add<int32_t>(mist->getSkillId());
	packet.add<uint8_t>(mist->getSkillLevel());
	packet.add<int16_t>(mist->getDelay());
	packet.addPos(mist->getLt(), true);
	packet.addPos(mist->getRb(), true);
	packet.add<int32_t>(0);
	Maps::getMap(mapId)->sendPacket(packet);
}

void MapPacket::removeMist(int32_t mapId, int32_t id) {
	PacketCreator packet;
	packet.add<header_t>(SMSG_MIST_DESPAWN);
	packet.add<int32_t>(id);
	Maps::getMap(mapId)->sendPacket(packet);
}

void MapPacket::instantWarp(Player *player, int8_t portalId) {
	PacketCreator packet;
	packet.add<header_t>(SMSG_MAP_TELEPORT);
	packet.add<int8_t>(0x01);
	packet.add<int8_t>(portalId);
	player->getSession()->send(packet);
}

void MapPacket::changeWeather(int32_t mapId, bool adminWeather, int32_t itemId, const string &message) {
	PacketCreator packet;
	packet.add<header_t>(SMSG_MAP_WEATHER_EFFECT);
	packet.addBool(adminWeather);
	packet.add<int32_t>(itemId);
	if (itemId != 0 && !adminWeather) {
		// Admin weathers doesn't have a message
		packet.addString(message);
	}
	Maps::getMap(mapId)->sendPacket(packet);
}