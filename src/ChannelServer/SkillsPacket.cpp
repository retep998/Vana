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
#include "SkillsPacket.h"
#include "GameConstants.h"
#include "MapleSession.h"
#include "Maps.h"
#include "PacketCreator.h"
#include "Player.h"
#include "SendHeader.h"
#include "Skills.h"

void SkillsPacket::addSkill(Player *player, int32_t skillid, PlayerSkillInfo skillinfo) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_ADD_SKILL);
	packet.add<int8_t>(1);
	packet.add<int16_t>(1);
	packet.add<int32_t>(skillid);
	packet.add<int32_t>(skillinfo.level); // Level
	packet.add<int32_t>(skillinfo.maxlevel); // Master Level
	packet.add<int8_t>(1);
	player->getSession()->send(packet);
}

void SkillsPacket::showSkill(Player *player, int32_t skillid, uint8_t level, bool party, bool self) {
	if (player->getActiveBuffs()->getActiveSkillLevel(SuperGM::HIDE) > 0)
		return;
 	PacketCreator packet;
	if (party && self) {
		packet.add<int16_t>(SEND_GAIN_ITEM);
	}
	else {
 		packet.add<int16_t>(SEND_SHOW_SKILL);
 		packet.add<int32_t>(player->getId());
	}
	packet.add<int8_t>(party ? 2 : 1);
 	packet.add<int32_t>(skillid);
	packet.add<int8_t>(level); // TODO
	if (self)
		player->getSession()->send(packet);
	else
		Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void SkillsPacket::healHP(Player *player, int16_t hp) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_GAIN_ITEM);
	packet.add<int8_t>(0xA);
	packet.add<int16_t>(hp);
	player->getSession()->send(packet);
}

void SkillsPacket::showSkillEffect(Player *player, int32_t skillid, uint8_t level) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_GAIN_ITEM); // For the using player
	bool send = false;
	switch (skillid) {
		case 2100000:
		case 2200000:
		case 2300000: // MP Eater
			packet.add<int8_t>(1);
			packet.add<int32_t>(skillid);
			packet.add<int8_t>(1);
			send = true;
			break;
		case 1311008: // Dragon Blood
			packet.add<int8_t>(5);
			packet.add<int32_t>(skillid);
			send = true;
			break;
	}
	if (send)
		player->getSession()->send(packet);
	if (player->getActiveBuffs()->getActiveSkillLevel(SuperGM::HIDE) > 0)
		return;
	packet = PacketCreator();
	send = false;
	packet.add<int16_t>(SEND_SHOW_SKILL);  // For others
	packet.add<int32_t>(player->getId());
	switch (skillid) {
		case 2100000:
		case 2200000:
		case 2300000: // MP Eater
			packet.add<int8_t>(1);
			packet.add<int32_t>(skillid);
			packet.add<int8_t>(1);
			send = true;
			break;
		case 4211005: // Meso Guard
		case 1311008: // Dragon Blood
			packet.add<int8_t>(5);
			packet.add<int32_t>(skillid);
			send = true;
			break;
	}
	if (send)
		Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void SkillsPacket::showSpecialSkill(Player *player, SpecialSkillInfo info) { // Hurricane, Pierce, Big Bang, Monster Magnet
	if (player->getActiveBuffs()->getActiveSkillLevel(SuperGM::HIDE) > 0)
		return;
	PacketCreator packet;
	packet.add<int16_t>(SEND_SPECIAL_SKILL);
	packet.add<int32_t>(player->getId());
	packet.add<int32_t>(info.skillid);
	packet.add<int8_t>(info.level);
	packet.add<int8_t>(info.direction);
	packet.add<int8_t>(info.w_speed);
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void SkillsPacket::endSpecialSkill(Player *player, SpecialSkillInfo info) {
	if (player->getActiveBuffs()->getActiveSkillLevel(SuperGM::HIDE) > 0)
		return;
	PacketCreator packet;
	packet.add<int16_t>(SEND_SPECIAL_SKILL_END);
	packet.add<int32_t>(player->getId());
	packet.add<int32_t>(info.skillid);
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void SkillsPacket::showMagnetSuccess(Player *player, int32_t mapmobid, uint8_t success) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_SHOW_DRAGGED);
	packet.add<int32_t>(mapmobid);
	packet.add<int8_t>(success);
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void SkillsPacket::sendCooldown(Player *player, int32_t skillid, int16_t time) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_COOLDOWN);
	packet.add<int32_t>(skillid);
	packet.add<int16_t>(time);
	player->getSession()->send(packet);
}

void SkillsPacket::showBerserk(Player *player, uint8_t level, bool on) { // Sends to map/user
	PacketCreator packet;
	packet.add<int16_t>(SEND_GAIN_ITEM);
	packet.add<int8_t>(1);
	packet.add<int32_t>(1320006);
	packet.add<int8_t>(level);
	packet.add<int8_t>(on ? 1 : 0);
	player->getSession()->send(packet);
	if (player->getActiveBuffs()->getActiveSkillLevel(SuperGM::HIDE) > 0)
		return;
	packet = PacketCreator();
	packet.add<int16_t>(SEND_SHOW_SKILL);  // For others
	packet.add<int32_t>(player->getId());
	packet.add<int8_t>(1);
	packet.add<int32_t>(1320006);
	packet.add<int8_t>(level);
	packet.add<int8_t>(on ? 1 : 0);
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}
