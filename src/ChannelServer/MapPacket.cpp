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
#include "MapPacket.h"
#include "Buffs.h"
#include "ChannelServer.h"
#include "Door.h"
#include "Inventory.h"
#include "Kite.h"
#include "MapleSession.h"
#include "Maps.h"
#include "Mist.h"
#include "Party.h"
#include "PacketCreator.h"
#include "Pet.h"
#include "Player.h"
#include "PlayerDataProvider.h"
#include "PlayerPacketHelper.h"
#include "SendHeader.h"
#include "TimeUtilities.h"
#include <boost/tr1/unordered_map.hpp>

using std::tr1::unordered_map;

PacketCreator MapPacket::playerPacket(Player *player) {
	PacketCreator packet;
	MapEntryBuffs enter = player->getActiveBuffs()->getMapEntryBuffs();
	packet.addHeader(SMSG_MAP_PLAYER_SPAWN);
	packet.add<int32_t>(player->getId());
	packet.addString(player->getName());
	if (player->getGuildId() > 0) {
		if (Guild *gi = PlayerDataProvider::Instance()->getGuild(player->getGuildId())) {
			packet.addString(gi->name);
			GuildLogo logo = gi->logo;
			packet.add<int16_t>(logo.background);
			packet.add<uint8_t>(logo.backgroundColor);
			packet.add<int16_t>(logo.logo);
			packet.add<uint8_t>(logo.color);
		}
		else {
			packet.addString("");
			packet.add<int16_t>(0);
			packet.add<int8_t>(0);
			packet.add<int16_t>(0);
			packet.add<int8_t>(0);
		}
	}
	else {
		packet.addString("");
		packet.add<int16_t>(0);
		packet.add<int8_t>(0);
		packet.add<int16_t>(0);
		packet.add<int8_t>(0);
	}

	packet.add<int32_t>(0);
	packet.add<uint8_t>(0xf8);
	packet.add<int8_t>(3);
	packet.add<int16_t>(0);
	{
		using namespace BuffBytes;
		packet.add<uint8_t>(enter.types[Byte5]);
		packet.add<uint8_t>(enter.types[Byte6]);
		packet.add<uint8_t>(enter.types[Byte7]);
		packet.add<uint8_t>(enter.types[Byte8]);
		packet.add<uint8_t>(enter.types[Byte1]);
		packet.add<uint8_t>(enter.types[Byte2]);
		packet.add<uint8_t>(enter.types[Byte3]);
		packet.add<uint8_t>(enter.types[Byte4]);

		const int8_t byteorder[EntryByteQuantity] = { Byte1, Byte2, Byte3, Byte4, Byte5, Byte6, Byte7, Byte8 };

		for (int8_t i = 0; i < EntryByteQuantity; i++) {
			int8_t cbyte = byteorder[i]; // Values are sorted by lower bytes first
			if (enter.types[cbyte] != 0) {
				for (unordered_map<uint8_t, MapEntryVals>::iterator iter = enter.values[cbyte].begin(); iter != enter.values[cbyte].end(); iter++) {
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
	}
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int16_t>(0);
	packet.add<int32_t>(1065638850); // Unknown
	packet.add<int16_t>(0);
	packet.add<int8_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(1065638850);
	packet.add<int16_t>(0);
	packet.add<int8_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(1065638850);
	packet.add<int16_t>(0);
	packet.add<int8_t>(0);

	packet.add<int32_t>(enter.mountid); // No point to having an if, these are 0 when not in use
	packet.add<int32_t>(enter.mountskill);

	packet.add<int32_t>(1065638850);
	packet.add<int8_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(1065638850);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(1065638850);
	packet.add<int8_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(1065638850);
	packet.add<int16_t>(0);
	packet.add<int8_t>(0);
	packet.add<int16_t>(player->getStats()->getJob());

	PlayerPacketHelper::addPlayerDisplay(packet, player);

	packet.add<int32_t>(0);
	packet.add<int32_t>(player->getItemEffect());
	packet.add<int32_t>(player->getChair());
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
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int8_t>(0);
	packet.add<int8_t>(0);
	packet.addBool(!player->getChalkboard().empty());
	packet.addString(player->getChalkboard());
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int16_t>(0);
	return packet;
}

void MapPacket::showPlayer(Player *player) {
	PacketCreator packet = playerPacket(player);
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void MapPacket::removePlayer(Player *player) {
	PacketCreator packet;
	packet.addHeader(SMSG_MAP_PLAYER_DESPAWN);
	packet.add<int32_t>(player->getId());
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void MapPacket::changeMap(Player *player) {
	PacketCreator packet;
	packet.addHeader(SMSG_CHANGE_MAP);
	packet.add<int32_t>(ChannelServer::Instance()->getChannel());
	packet.add<uint8_t>(player->getPortalCount());
	packet.addBool(false); // It's not a connect packet.
	packet.add<int16_t>(0); // Some amount for a funny message at the top of the screen
	/*
	// Structure
	packet.addString( Message title );
	for (size_t i = 0; i < amount; i++) {
		packet.addString( Line );
	}
	*/

	packet.add<int32_t>(player->getMap());
	packet.add<int8_t>(player->getMappos());
	packet.add<int16_t>(player->getStats()->getHp());
	packet.add<int8_t>(0x00);
	packet.add<int64_t>(TimeUtilities::getServerTime());
	player->getSession()->send(packet);
}

void MapPacket::portalBlocked(Player *player) {
	PacketCreator packet;
	packet.addHeader(SMSG_PLAYER_UPDATE);
	packet.add<int8_t>(0x01);
	packet.add<int32_t>(0x00);
	player->getSession()->send(packet);
}

void MapPacket::showClock(Player *player, int8_t hour, int8_t min, int8_t sec) {
	PacketCreator packet;
	packet.addHeader(SMSG_TIMER);
	packet.add<int8_t>(0x01);
	packet.add<int8_t>(hour);
	packet.add<int8_t>(min);
	packet.add<int8_t>(sec);
	player->getSession()->send(packet);
}

void MapPacket::showTimer(int32_t mapid, int32_t sec) {
	PacketCreator packet;
	if (sec > 0) {
		packet.addHeader(SMSG_TIMER);
		packet.add<int8_t>(0x02);
		packet.add<int32_t>(sec);
	}
	else {
		packet.addHeader(SMSG_TIMER_OFF);
	}
	Maps::getMap(mapid)->sendPacket(packet);
}

void MapPacket::showTimer(Player *player, int32_t sec) {
	PacketCreator packet;
	if (sec > 0) {
		packet.addHeader(SMSG_TIMER);
		packet.add<int8_t>(0x02);
		packet.add<int32_t>(sec);
	}
	else {
		packet.addHeader(SMSG_TIMER_OFF);
	}
	player->getSession()->send(packet);
}

void MapPacket::forceMapEquip(Player *player) {
	PacketCreator packet;
	packet.addHeader(SMSG_MAP_FORCE_EQUIPMENT);
	player->getSession()->send(packet);
}

void MapPacket::showEventInstructions(int32_t mapid) {
	PacketCreator packet;
	packet.addHeader(SMSG_EVENT_INSTRUCTION);
	packet.add<int8_t>(0x00);
	Maps::getMap(mapid)->sendPacket(packet);
}

void MapPacket::showMist(Player *player, Mist *mist) {
	PacketCreator packet;
	packet.addHeader(SMSG_MIST_SPAWN);
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

void MapPacket::spawnMist(int32_t mapid, Mist *mist) {
	PacketCreator packet;
	packet.addHeader(SMSG_MIST_SPAWN);
	packet.add<int32_t>(mist->getId());
	packet.add<int32_t>(mist->isMobMist() ? 0 : mist->isPoison() ? 1 : 2);
	packet.add<int32_t>(mist->getOwnerId());
	packet.add<int32_t>(mist->getSkillId());
	packet.add<uint8_t>(mist->getSkillLevel());
	packet.add<int16_t>(mist->getDelay());
	packet.addPos(mist->getLt(), true);
	packet.addPos(mist->getRb(), true);
	packet.add<int32_t>(0);
	Maps::getMap(mapid)->sendPacket(packet);
}

void MapPacket::removeMist(int32_t mapid, int32_t id) {
	PacketCreator packet;
	packet.addHeader(SMSG_MIST_DESPAWN);
	packet.add<int32_t>(id);
	Maps::getMap(mapid)->sendPacket(packet);
}

void MapPacket::instantWarp(Player *player, int8_t pid) {
	PacketCreator packet;
	packet.addHeader(SMSG_MAP_TELEPORT);
	packet.add<int8_t>(0x01);
	packet.add<int8_t>(pid);
	player->getSession()->send(packet);
}

void MapPacket::changeWeather(int32_t mapid, bool adminWeather, int32_t itemid, const string &message) {
	PacketCreator packet;
	packet.addHeader(SMSG_MAP_WEATHER_EFFECT);
	packet.addBool(adminWeather);
	packet.add<int32_t>(itemid);
	if (itemid != 0 && !adminWeather) {
		// Admin weathers doesn't have a message
		packet.addString(message);
	}
	Maps::getMap(mapid)->sendPacket(packet);
}

void MapPacket::changeWeatherPlayer(Player *player, bool adminWeather, int32_t itemid, const string &message) {
	PacketCreator packet;
	packet.addHeader(SMSG_MAP_WEATHER_EFFECT);
	packet.addBool(adminWeather);
	packet.add<int32_t>(itemid);
	if (itemid != 0 && !adminWeather) {
		// Admin weathers doesn't have a message
		packet.addString(message);
	}
	player->getSession()->send(packet);
}

void MapPacket::spawnDoor(Door *door) {
	PacketCreator packet;
	packet.addHeader(SMSG_MYSTIC_DOOR_SPAWN);
	packet.addBool(false);
	packet.add<int32_t>(door->getId());
	packet.addPos(door->getPos());
	door->getSource()->sendPacket(packet);

	packet = PacketCreator();
	packet.addHeader(SMSG_MYSTIC_DOOR_SPAWN);
	packet.addBool(true);
	packet.add<int32_t>(door->getId());
	packet.addPos(door->getPortal()->pos);

	if (Party *p = door->getOwner()->getParty()) {
		p->sendPacket(packet, door->getTownId());
	}
}


void MapPacket::showDoor(Player *player, Door *door, bool istown) {
	PacketCreator packet;
	packet.addHeader(SMSG_MYSTIC_DOOR_SPAWN);
	packet.addBool(true); // Already open
	packet.add<int32_t>(door->getId());
	packet.addPos(istown ? door->getPortal()->pos : door->getPos());
	player->getSession()->send(packet);
}

void MapPacket::showDoorDisappear(Player *player, Door *door) {
	PacketCreator packet;
	packet.addHeader(SMSG_MYSTIC_DOOR_DESPAWN);
	packet.addBool(false);
	packet.add<int32_t>(door->getId());
	player->getSession()->send(packet);
}

void MapPacket::removeDoor(Door *door, bool displace) {
	PacketCreator packet;
	packet.addHeader(SMSG_MYSTIC_DOOR_DESPAWN);
	packet.addBool(displace);
	packet.add<int32_t>(door->getId());

	door->getSource()->sendPacket(packet);

	if (Party *p = door->getOwner()->getParty()) {
		p->sendPacket(packet, door->getTownId());
	}
}

void MapPacket::spawnPortal(int32_t source, int32_t destination, const Pos &srcpos, const Pos &dstpos) {
	PacketCreator packet;
	packet.addHeader(SMSG_PORTAL_ACTION);
	packet.add<int32_t>(destination);
	packet.add<int32_t>(source);
	packet.addPos(srcpos);
	Maps::getMap(source)->sendPacket(packet);

	packet = PacketCreator();
	packet.addHeader(SMSG_PORTAL_ACTION);
	packet.add<int32_t>(source);
	packet.add<int32_t>(destination);
	packet.addPos(dstpos);
	Maps::getMap(destination)->sendPacket(packet);
}

void MapPacket::removePortal(int32_t source, int32_t destination) {
	PacketCreator packet;
	packet.addHeader(SMSG_PORTAL_ACTION);
	packet.add<int32_t>(Maps::NoMap);
	packet.add<int32_t>(Maps::NoMap);
	Maps::getMap(source)->sendPacket(packet);
	Maps::getMap(destination)->sendPacket(packet);
}

void MapPacket::spawnKite(Kite &kite) {
	PacketCreator packet;
	packet.addHeader(SMSG_KITE_SPAWN);
	packet.add<int32_t>(kite.id);
	packet.add<int32_t>(kite.itemid);
	packet.addString(kite.message);
	packet.addString(kite.ownerName);
	packet.addPos(kite.position);
	Maps::getMap(kite.map)->sendPacket(packet);
}

void MapPacket::spawnKite(Player *player, Kite &kite) {
	PacketCreator packet;
	packet.addHeader(SMSG_KITE_SPAWN);
	packet.add<int32_t>(kite.id);
	packet.add<int32_t>(kite.itemid);
	packet.addString(kite.message);
	packet.addString(kite.ownerName);
	packet.addPos(kite.position);
	player->getSession()->send(packet);
}

void MapPacket::despawnKite(Kite &kite, int8_t reason) {
	PacketCreator packet;
	packet.addHeader(SMSG_KITE_DESPAWN);
	packet.add<int8_t>(reason);
	packet.add<int32_t>(kite.id);
	Maps::getMap(kite.map)->sendPacket(packet);
}