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
#include "BuffsPacket.h"
#include "MapleSession.h"
#include "Maps.h"
#include "PacketCreator.h"
#include "Player.h"
#include "SendHeader.h"
#include "Buffs.h"

void BuffsPacket::useDash(Player *player, int32_t time, SkillActiveInfo pskill) {
	PacketCreator packet;
	int16_t castedtime = static_cast<int16_t>(time);
	packet.addShort(SEND_USE_SKILL);
	packet.addInt64(0);
	for (int8_t i = 0; i < 8; i++)
		packet.addByte(pskill.types[i]);
	packet.addShort(0);
	for (size_t i = 0; i < pskill.vals.size(); i++) {
		packet.addShort(pskill.vals[i]);
		packet.addShort(0);
		packet.addInt(5001005);
		packet.addInt(880689251); // No idea, hate pirates
		packet.addShort(castedtime);
	}
	packet.addShort(0);
	packet.addByte(0); // Number of times you've been buffed total - only certain skills have this part
	player->getSession()->send(packet);
	if (player->getActiveBuffs()->getActiveSkillLevel(9101004) > 0)
		return;
	packet = PacketCreator();
	packet.addShort(SEND_SHOW_OTHERS_SKILL);
	packet.addInt(player->getId());
	packet.addInt64(0);
	for (int8_t i = 0; i < 8; i++)
		packet.addByte(pskill.types[i]);
	packet.addShort(0);
	for (size_t i = 0; i < pskill.vals.size(); i++) {
		packet.addShort(pskill.vals[i]);
		packet.addShort(0);
		packet.addInt(5001005);
		packet.addInt(880689251); // No idea, hate pirates
		packet.addShort(castedtime);
	}
	packet.addShort(0);
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void BuffsPacket::useMount(Player *player, int32_t skillid, int32_t time, SkillActiveInfo pskill, int16_t addedinfo, int32_t mountid) {
	time *= 1000;
	PacketCreator packet;
	packet.addShort(SEND_USE_SKILL);
	packet.addInt64(0);
	for (int8_t i = 0; i < 8; i++)
		packet.addByte(pskill.types[i]);
	packet.addShort(0);
	packet.addInt(mountid);
	packet.addInt(skillid);
	packet.addInt(0); // Server tick value
	packet.addShort(0);
	packet.addByte(0); // Number of times you've been buffed total
	player->getSession()->send(packet);
	if (player->getActiveBuffs()->getActiveSkillLevel(9101004) > 0)
		return;
	packet = PacketCreator();
	packet.addShort(SEND_SHOW_OTHERS_SKILL);
	packet.addInt(player->getId());
	packet.addInt64(0);
	for (int8_t i = 0; i < 8; i++)
		packet.addByte(pskill.types[i]);
	packet.addShort(0);
	packet.addInt(mountid);
	packet.addInt(skillid);
	packet.addInt(0);
	packet.addShort(0);
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void BuffsPacket::useSkill(Player *player, int32_t skillid, int32_t time, SkillActiveInfo pskill, int16_t addedinfo, bool ismorph, bool isitem) {
	if (skillid != 1111002)
		time *= 1000;
	PacketCreator packet;
	packet.addShort(SEND_USE_SKILL);
	packet.addInt64(0);
	for (int8_t i = 0; i < 8; i++)
		packet.addByte(pskill.types[i]);
	for (size_t i = 0; i < pskill.vals.size(); i++) {
		packet.addShort(pskill.vals[i]);
		packet.addInt(skillid);
		packet.addInt(time);
	}
	packet.addShort(0);
	packet.addShort(addedinfo);
	packet.addByte(0); // Number of times you've been buffed total - only certain skills have this part
	player->getSession()->send(packet);
	if (player->getActiveBuffs()->getActiveSkillLevel(9101004) > 0)
		return;
	if (!isitem || (isitem && ismorph)) {
		packet = PacketCreator();
		packet.addShort(SEND_SHOW_OTHERS_SKILL);
		packet.addInt(player->getId());
		packet.addInt64(0);
		for (int8_t i = 0; i < 8; i++)
			packet.addByte(pskill.types[i]);
		for (size_t i = 0; i < pskill.vals.size(); i++)
			packet.addShort(pskill.vals[i]);
		packet.addShort(0);
		if (ismorph)
			packet.addShort(0);
		Maps::getMap(player->getMap())->sendPacket(packet, player);
	}
}

void BuffsPacket::useSpeedInfusion(Player *player, int32_t time, SkillActiveInfo pskill, int16_t addedinfo) {
	int32_t castedvalue = static_cast<int32_t>(pskill.vals[0]);
	int16_t castedtime = static_cast<int16_t>(time);
	PacketCreator packet;
	packet.addShort(SEND_USE_SKILL);
	packet.addInt64(0);
	for (int8_t i = 0; i < 8; i++)
		packet.addByte(pskill.types[i]);
	packet.addShort(0);
	packet.addInt(castedvalue);
	packet.addInt(5121009);
	packet.addInt(0);
	packet.addInt(0);
	packet.addShort(castedtime);
	packet.addShort(addedinfo);
	player->getSession()->send(packet);
	if (player->getActiveBuffs()->getActiveSkillLevel(9101004) > 0)
		return;
	packet = PacketCreator();
	packet.addShort(SEND_SHOW_OTHERS_SKILL);
	packet.addInt(player->getId());
	packet.addInt64(0);
	for (int8_t i = 0; i < 8; i++)
		packet.addByte(pskill.types[i]);
	packet.addShort(0);
	packet.addInt(castedvalue);
	packet.addInt(5121009);
	packet.addInt(0);
	packet.addInt(0);
	packet.addShort(castedtime);
	packet.addShort(addedinfo);
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void BuffsPacket::endSkill(Player *player, SkillActiveInfo pskill) {
	PacketCreator packet;
	packet.addShort(SEND_CANCEL_SKILL);
	packet.addInt64(0);
	for (int8_t i = 0; i < 8; i++)
		packet.addByte(pskill.types[i]);
	packet.addByte(0);
	player->getSession()->send(packet);
	if (player->getActiveBuffs()->getActiveSkillLevel(9101004) > 0)
		return;
	packet = PacketCreator();
	packet.addShort(SEND_CANCEL_OTHERS_BUFF);
	packet.addInt(player->getId());
	packet.addInt64(0);
	for (int8_t i = 0; i < 8; i++)
		packet.addByte(pskill.types[i]);
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}
