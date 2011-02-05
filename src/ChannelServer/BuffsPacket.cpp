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
#include "BuffsPacket.h"
#include "Buffs.h"
#include "BuffsPacketHelper.h"
#include "MapleSession.h"
#include "Maps.h"
#include "PacketCreator.h"
#include "Player.h"
#include "SendHeader.h"
#include "SkillConstants.h"

void BuffsPacket::useSkill(Player *player, int32_t skillid, int32_t time, ActiveBuff &pskill, ActiveMapBuff &mskill, int16_t addedinfo) {
	time *= 1000;
	PacketCreator packet;
	packet.addHeader(SMSG_SKILL_USE);

	BuffsPacketHelper::addBytes(packet, pskill.types);

	for (size_t i = 0; i < pskill.vals.size(); i++) {
		packet.add<int16_t>(pskill.vals[i]);
		packet.add<int32_t>(skillid);
		packet.add<int32_t>(time);
	}
	packet.add<int16_t>(0);
	packet.add<int16_t>(addedinfo);
	packet.add<int8_t>(0); // Number of times you've been buffed total - only certain skills have this part
	player->getSession()->send(packet);
	if (player->getActiveBuffs()->isUsingHide()) {
		return;
	}
	if (pskill.hasmapbuff) {
		packet = PacketCreator();
		packet.addHeader(SMSG_3RD_PARTY_SKILL);
		packet.add<int32_t>(player->getId());

		BuffsPacketHelper::addBytes(packet, mskill.typelist);

		if ((mskill.typelist[BuffBytes::Byte3] & 0x40) > 0) {
			packet.add<int32_t>(skillid);
		}
		else {
			for (size_t i = 0; i < mskill.values.size(); i++) {
				uint8_t byte = mskill.bytes[i];
				if (byte == BuffBytes::Byte5) {
					packet.add<int16_t>(mskill.values[i]);
				}
				else {
					packet.add<int8_t>(static_cast<int8_t>(mskill.values[i]));
				}
			}
		}
		packet.add<int16_t>(0);
		packet.add<int16_t>(addedinfo);
		Maps::getMap(player->getMap())->sendPacket(packet, player);
	}
}

void BuffsPacket::giveDebuff(Player *player, uint8_t skillid, uint8_t level, int16_t time, int16_t delay, ActiveBuff &pskill, ActiveMapBuff &mskill) {
	time *= 1000;
	PacketCreator packet;
	packet.addHeader(SMSG_SKILL_USE);

	BuffsPacketHelper::addBytes(packet, pskill.types);

	for (size_t i = 0; i < pskill.vals.size(); i++) {
		packet.add<int16_t>(pskill.vals[i]);
		packet.add<uint16_t>(skillid);
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

	BuffsPacketHelper::addBytes(packet, mskill.typelist);

	for (size_t i = 0; i < mskill.values.size(); i++) {
		if (skillid == MobSkills::Poison) {
			packet.add<int16_t>(mskill.values[i]);
		}
		packet.add<uint16_t>(skillid);
		packet.add<uint16_t>(level);
	}
	packet.add<int16_t>(0);
	packet.add<int16_t>(delay);
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void BuffsPacket::endDebuff(Player *player, ActiveBuff &pskill) {
	PacketCreator packet;
	packet.addHeader(SMSG_SKILL_CANCEL);

	BuffsPacketHelper::addBytes(packet, pskill.types);

	packet.add<int8_t>(0);
	player->getSession()->send(packet);
	if (player->getActiveBuffs()->isUsingHide()) {
		return;
	}
	packet = PacketCreator();
	packet.addHeader(SMSG_3RD_PARTY_BUFF_END);
	packet.add<int32_t>(player->getId());

	BuffsPacketHelper::addBytes(packet, pskill.types);

	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void BuffsPacket::endSkill(Player *player, ActiveBuff &pskill) {
	PacketCreator packet;
	packet.addHeader(SMSG_SKILL_CANCEL);

	BuffsPacketHelper::addBytes(packet, pskill.types);

	packet.add<int8_t>(0);
	player->getSession()->send(packet);
	if (player->getActiveBuffs()->isUsingHide()) {
		return;
	}
	packet = PacketCreator();
	packet.addHeader(SMSG_3RD_PARTY_BUFF_END);
	packet.add<int32_t>(player->getId());

	BuffsPacketHelper::addBytes(packet, pskill.types);

	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void BuffsPacket::usePirateBuff(Player *player, int32_t skillid, int32_t time, ActiveBuff &pskill, ActiveMapBuff &mskill) {
	PacketCreator packet;
	int16_t castedtime = static_cast<int16_t>(time);
	packet.addHeader(SMSG_SKILL_USE);

	BuffsPacketHelper::addBytes(packet, pskill.types);

	packet.add<int16_t>(0);
	for (size_t i = 0; i < pskill.vals.size(); i++) {
		packet.add<int16_t>(pskill.vals[i]);
		packet.add<int16_t>(0);
		packet.add<int32_t>(skillid);
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

	BuffsPacketHelper::addBytes(packet, mskill.typelist);

	packet.add<int16_t>(0);
	for (size_t i = 0; i < pskill.vals.size(); i++) {
		packet.add<int16_t>(pskill.vals[i]);
		packet.add<int16_t>(0);
		packet.add<int32_t>(skillid);
		packet.add<int32_t>(0); // No idea, hate pirates, seems to be server tick count in ms
		packet.add<int8_t>(0);
		packet.add<int16_t>(castedtime);
	}
	packet.add<int16_t>(0);
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void BuffsPacket::useSpeedInfusion(Player *player, int32_t skillid, int32_t time, ActiveBuff &pskill, ActiveMapBuff &mskill, int16_t addedinfo) {
	int32_t castedvalue = static_cast<int32_t>(pskill.vals[0]);
	int16_t castedtime = static_cast<int16_t>(time);
	PacketCreator packet;
	packet.addHeader(SMSG_SKILL_USE);

	BuffsPacketHelper::addBytes(packet, pskill.types);

	packet.add<int16_t>(0);
	packet.add<int32_t>(castedvalue);
	packet.add<int32_t>(skillid);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int16_t>(0);
	packet.add<int16_t>(castedtime);
	packet.add<int16_t>(addedinfo);
	player->getSession()->send(packet);
	if (player->getActiveBuffs()->isUsingHide()) {
		return;
	}
	packet = PacketCreator();
	packet.addHeader(SMSG_3RD_PARTY_SKILL);
	packet.add<int32_t>(player->getId());

	BuffsPacketHelper::addBytes(packet, mskill.typelist);

	packet.add<int16_t>(0);
	packet.add<int32_t>(castedvalue);
	packet.add<int32_t>(skillid);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int16_t>(0);
	packet.add<int16_t>(castedtime);
	packet.add<int16_t>(addedinfo);
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void BuffsPacket::useMount(Player *player, int32_t skillid, int32_t time, ActiveBuff &pskill, ActiveMapBuff &mskill, int16_t addedinfo, int32_t mountid) {
	time *= 1000;
	PacketCreator packet;
	packet.addHeader(SMSG_SKILL_USE);

	BuffsPacketHelper::addBytes(packet, pskill.types);

	packet.add<int16_t>(0);
	packet.add<int32_t>(mountid);
	packet.add<int32_t>(skillid);
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

	BuffsPacketHelper::addBytes(packet, mskill.typelist);

	packet.add<int16_t>(0);
	packet.add<int32_t>(mountid);
	packet.add<int32_t>(skillid);
	packet.add<int32_t>(0);
	packet.add<int16_t>(0);
	packet.add<int8_t>(0);
	packet.add<int8_t>(0);
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void BuffsPacket::useHomingBeacon(Player *player, int32_t skillid, ActiveBuff &pskill, int32_t mapmobid) {
	PacketCreator packet;
	packet.addHeader(SMSG_SKILL_USE);

	BuffsPacketHelper::addBytes(packet, pskill.types);

	packet.add<int16_t>(0);
	for (size_t i = 0; i < pskill.vals.size(); i++) {
		packet.add<int16_t>(pskill.vals[i]);
	}
	packet.add<int16_t>(0);
	packet.add<int32_t>(skillid);
	packet.add<int32_t>(0); // Time
	packet.add<int8_t>(0);
	packet.add<int32_t>(mapmobid);
	packet.add<int16_t>(0);
	player->getSession()->send(packet);
}