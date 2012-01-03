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
#include "BuffsPacket.h"
#include "Buffs.h"
#include "BuffsPacketHelper.h"
#include "Maps.h"
#include "PacketCreator.h"
#include "Player.h"
#include "Session.h"
#include "SkillConstants.h"
#include "SmsgHeader.h"

void BuffsPacket::useSkill(Player *player, int32_t skillId, int32_t time, ActiveBuff &playerSkill, ActiveMapBuff &mapSkill, int16_t addedInfo) {
	time *= 1000;
	PacketCreator packet;
	packet.addHeader(SMSG_SKILL_USE);

	BuffsPacketHelper::addBytes(packet, playerSkill.types);

	for (size_t i = 0; i < playerSkill.vals.size(); i++) {
		packet.add<int16_t>(playerSkill.vals[i]);
		packet.add<int32_t>(skillId);
		packet.add<int32_t>(time);
	}
	packet.add<int16_t>(0);
	packet.add<int16_t>(addedInfo);
	packet.add<int8_t>(0); // Number of times you've been buffed total - only certain skills have this part
	player->getSession()->send(packet);
	if (player->getActiveBuffs()->isUsingHide()) {
		return;
	}
	if (playerSkill.hasMapBuff) {
		packet = PacketCreator();
		packet.addHeader(SMSG_3RD_PARTY_SKILL);
		packet.add<int32_t>(player->getId());

		BuffsPacketHelper::addBytes(packet, mapSkill.typeList);

		if ((mapSkill.typeList[BuffBytes::Byte3] & 0x40) > 0) {
			packet.add<int32_t>(skillId);
		}
		else {
			for (size_t i = 0; i < mapSkill.values.size(); i++) {
				uint8_t byte = mapSkill.bytes[i];
				if (byte == BuffBytes::Byte5) {
					packet.add<int16_t>(mapSkill.values[i]);
				}
				else {
					packet.add<int8_t>(static_cast<int8_t>(mapSkill.values[i]));
				}
			}
		}
		packet.add<int16_t>(0);
		packet.add<int16_t>(addedInfo);
		Maps::getMap(player->getMap())->sendPacket(packet, player);
	}
}

void BuffsPacket::giveDebuff(Player *player, uint8_t skillId, uint8_t level, int16_t time, int16_t delay, ActiveBuff &playerSkill, ActiveMapBuff &mapSkill) {
	time *= 1000;
	PacketCreator packet;
	packet.addHeader(SMSG_SKILL_USE);

	BuffsPacketHelper::addBytes(packet, playerSkill.types);

	for (size_t i = 0; i < playerSkill.vals.size(); i++) {
		packet.add<int16_t>(playerSkill.vals[i]);
		packet.add<uint16_t>(skillId);
		packet.add<uint16_t>(level);
		packet.add<int32_t>(time);
	}
	packet.add<int16_t>(0);
	packet.add<int16_t>(delay);
	packet.add<int8_t>(0);
	player->getSession()->send(packet);
	if (player->getActiveBuffs()->isUsingHide()) {
		return;
	}
	packet = PacketCreator();
	packet.addHeader(SMSG_3RD_PARTY_SKILL);
	packet.add<int32_t>(player->getId());

	BuffsPacketHelper::addBytes(packet, mapSkill.typeList);

	for (size_t i = 0; i < mapSkill.values.size(); i++) {
		if (skillId == MobSkills::Poison) {
			packet.add<int16_t>(mapSkill.values[i]);
		}
		packet.add<uint16_t>(skillId);
		packet.add<uint16_t>(level);
	}
	packet.add<int16_t>(0);
	packet.add<int16_t>(delay);
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void BuffsPacket::endDebuff(Player *player, ActiveBuff &playerSkill) {
	PacketCreator packet;
	packet.addHeader(SMSG_SKILL_CANCEL);

	BuffsPacketHelper::addBytes(packet, playerSkill.types);

	packet.add<int8_t>(0);
	player->getSession()->send(packet);
	if (player->getActiveBuffs()->isUsingHide()) {
		return;
	}
	packet = PacketCreator();
	packet.addHeader(SMSG_3RD_PARTY_BUFF_END);
	packet.add<int32_t>(player->getId());

	BuffsPacketHelper::addBytes(packet, playerSkill.types);

	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void BuffsPacket::endSkill(Player *player, ActiveBuff &playerSkill) {
	PacketCreator packet;
	packet.addHeader(SMSG_SKILL_CANCEL);

	BuffsPacketHelper::addBytes(packet, playerSkill.types);

	packet.add<int8_t>(0);
	player->getSession()->send(packet);
	if (player->getActiveBuffs()->isUsingHide()) {
		return;
	}
	packet = PacketCreator();
	packet.addHeader(SMSG_3RD_PARTY_BUFF_END);
	packet.add<int32_t>(player->getId());

	BuffsPacketHelper::addBytes(packet, playerSkill.types);

	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void BuffsPacket::usePirateBuff(Player *player, int32_t skillId, int32_t time, ActiveBuff &playerSkill, ActiveMapBuff &mapSkill) {
	PacketCreator packet;
	int16_t castedtime = static_cast<int16_t>(time);
	packet.addHeader(SMSG_SKILL_USE);

	BuffsPacketHelper::addBytes(packet, playerSkill.types);

	packet.add<int16_t>(0);
	for (size_t i = 0; i < playerSkill.vals.size(); i++) {
		packet.add<int16_t>(playerSkill.vals[i]);
		packet.add<int16_t>(0);
		packet.add<int32_t>(skillId);
		packet.add<int32_t>(0); // No idea, hate pirates, seems to be server tick count in ms
		packet.add<int8_t>(0);
		packet.add<int16_t>(castedtime);
	}
	packet.add<int16_t>(0);
	packet.add<int8_t>(0); // Number of times you've been buffed total - only certain skills have this part
	player->getSession()->send(packet);
	if (player->getActiveBuffs()->isUsingHide()) {
		return;
	}
	packet = PacketCreator();
	packet.addHeader(SMSG_3RD_PARTY_SKILL);
	packet.add<int32_t>(player->getId());

	BuffsPacketHelper::addBytes(packet, mapSkill.typeList);

	packet.add<int16_t>(0);
	for (size_t i = 0; i < playerSkill.vals.size(); i++) {
		packet.add<int16_t>(playerSkill.vals[i]);
		packet.add<int16_t>(0);
		packet.add<int32_t>(skillId);
		packet.add<int32_t>(0); // No idea, hate pirates, seems to be server tick count in ms
		packet.add<int8_t>(0);
		packet.add<int16_t>(castedtime);
	}
	packet.add<int16_t>(0);
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void BuffsPacket::useSpeedInfusion(Player *player, int32_t skillId, int32_t time, ActiveBuff &playerSkill, ActiveMapBuff &mapSkill, int16_t addedInfo) {
	int32_t castedvalue = static_cast<int32_t>(playerSkill.vals[0]);
	int16_t castedtime = static_cast<int16_t>(time);
	PacketCreator packet;
	packet.addHeader(SMSG_SKILL_USE);

	BuffsPacketHelper::addBytes(packet, playerSkill.types);

	packet.add<int16_t>(0);
	packet.add<int32_t>(castedvalue);
	packet.add<int32_t>(skillId);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int16_t>(0);
	packet.add<int16_t>(castedtime);
	packet.add<int16_t>(addedInfo);
	player->getSession()->send(packet);
	if (player->getActiveBuffs()->isUsingHide()) {
		return;
	}
	packet = PacketCreator();
	packet.addHeader(SMSG_3RD_PARTY_SKILL);
	packet.add<int32_t>(player->getId());

	BuffsPacketHelper::addBytes(packet, mapSkill.typeList);

	packet.add<int16_t>(0);
	packet.add<int32_t>(castedvalue);
	packet.add<int32_t>(skillId);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int16_t>(0);
	packet.add<int16_t>(castedtime);
	packet.add<int16_t>(addedInfo);
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void BuffsPacket::useMount(Player *player, int32_t skillId, int32_t time, ActiveBuff &playerSkill, ActiveMapBuff &mapSkill, int16_t addedInfo, int32_t mountId) {
	time *= 1000;
	PacketCreator packet;
	packet.addHeader(SMSG_SKILL_USE);

	BuffsPacketHelper::addBytes(packet, playerSkill.types);

	packet.add<int16_t>(0);
	packet.add<int32_t>(mountId);
	packet.add<int32_t>(skillId);
	packet.add<int32_t>(0); // Server tick value
	packet.add<int16_t>(0);
	packet.add<int8_t>(0);
	packet.add<int8_t>(0); // Number of times you've been buffed total
	player->getSession()->send(packet);
	if (player->getActiveBuffs()->isUsingHide()) {
		return;
	}
	packet = PacketCreator();
	packet.addHeader(SMSG_3RD_PARTY_SKILL);
	packet.add<int32_t>(player->getId());

	BuffsPacketHelper::addBytes(packet, mapSkill.typeList);

	packet.add<int16_t>(0);
	packet.add<int32_t>(mountId);
	packet.add<int32_t>(skillId);
	packet.add<int32_t>(0);
	packet.add<int16_t>(0);
	packet.add<int8_t>(0);
	packet.add<int8_t>(0);
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void BuffsPacket::useHomingBeacon(Player *player, int32_t skillId, ActiveBuff &playerSkill, int32_t mapMobId) {
	PacketCreator packet;
	packet.addHeader(SMSG_SKILL_USE);

	BuffsPacketHelper::addBytes(packet, playerSkill.types);

	packet.add<int16_t>(0);
	for (size_t i = 0; i < playerSkill.vals.size(); i++) {
		packet.add<int16_t>(playerSkill.vals[i]);
	}
	packet.add<int16_t>(0);
	packet.add<int32_t>(skillId);
	packet.add<int32_t>(0); // Time
	packet.add<int8_t>(0);
	packet.add<int32_t>(mapMobId);
	packet.add<int16_t>(0);
	player->getSession()->send(packet);
}