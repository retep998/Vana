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
#include "SkillsPacket.h"
#include "GameConstants.h"
#include "MapleSession.h"
#include "Maps.h"
#include "PacketCreator.h"
#include "Player.h"
#include "SendHeader.h"
#include "Skills.h"

void SkillsPacket::addSkill(Player *player, int32_t skillid, const PlayerSkillInfo &skillinfo) {
	PacketCreator packet;
	packet.addHeader(SMSG_SKILL_ADD);
	packet.add<int8_t>(1);
	packet.add<int16_t>(1);
	packet.add<int32_t>(skillid);
	packet.add<int32_t>(skillinfo.level); // Level
	packet.add<int32_t>(skillinfo.maxlevel); // Master Level
	packet.add<int8_t>(1);
	player->getSession()->send(packet);
}

void SkillsPacket::showSkill(Player *player, int32_t skillid, uint8_t level, uint8_t direction, bool party, bool self) {
	if (player->getActiveBuffs()->isUsingHide())
		return;
	PacketCreator packet;
	if (party && self) {
		packet.addHeader(SMSG_THEATRICS);
	}
	else {
		packet.addHeader(SMSG_SKILL_SHOW);
		packet.add<int32_t>(player->getId());
	}
	packet.add<int8_t>(party ? 2 : 1);
	packet.add<int32_t>(skillid);
	packet.add<int8_t>(level); // TODO
	switch (skillid) {
		case Jobs::Hero::MonsterMagnet: // Monster Magnet processing
		case Jobs::Paladin::MonsterMagnet:
		case Jobs::DarkKnight::MonsterMagnet:
			packet.add<uint8_t>(direction);
			break;
	}
	if (self) {
		player->getSession()->send(packet);
	}
	else {
		Maps::getMap(player->getMap())->sendPacket(packet, player);
	}
}

void SkillsPacket::healHP(Player *player, int16_t hp) {
	PacketCreator packet;
	packet.addHeader(SMSG_THEATRICS);
	packet.add<int8_t>(0x0A);
	packet.add<int16_t>(hp);
	player->getSession()->send(packet);
}

void SkillsPacket::showSkillEffect(Player *player, int32_t skillid, uint8_t level) {
	PacketCreator packet;
	packet.addHeader(SMSG_THEATRICS); // For the using player
	bool send = false;
	switch (skillid) {
		case Jobs::FPWizard::MpEater:
		case Jobs::ILWizard::MpEater:
		case Jobs::Cleric::MpEater:
			packet.add<int8_t>(1);
			packet.add<int32_t>(skillid);
			packet.add<int8_t>(1);
			send = true;
			break;
		case Jobs::DragonKnight::DragonBlood: // Dragon Blood
			packet.add<int8_t>(5);
			packet.add<int32_t>(skillid);
			send = true;
			break;
	}
	if (send) {
		player->getSession()->send(packet);
	}
	if (player->getActiveBuffs()->isUsingHide()) {
		return;
	}
	packet = PacketCreator();
	send = false;
	packet.addHeader(SMSG_SKILL_SHOW); // For others
	packet.add<int32_t>(player->getId());
	switch (skillid) {
		case Jobs::FPWizard::MpEater:
		case Jobs::ILWizard::MpEater:
		case Jobs::Cleric::MpEater:
			packet.add<int8_t>(1);
			packet.add<int32_t>(skillid);
			packet.add<int8_t>(1);
			send = true;
			break;
		case Jobs::ChiefBandit::MesoGuard:
		case Jobs::DragonKnight::DragonBlood:
			packet.add<int8_t>(5);
			packet.add<int32_t>(skillid);
			send = true;
			break;
	}
	if (send) {
		Maps::getMap(player->getMap())->sendPacket(packet, player);
	}
}

void SkillsPacket::showSpecialSkill(Player *player, const SpecialSkillInfo &info) { // Hurricane, Pierce, Big Bang, Monster Magnet
	if (player->getActiveBuffs()->isUsingHide()) {
		return;
	}
	PacketCreator packet;
	packet.addHeader(SMSG_SPECIAL_SKILL);
	packet.add<int32_t>(player->getId());
	packet.add<int32_t>(info.skillId);
	packet.add<int8_t>(info.level);
	packet.add<int8_t>(info.direction);
	packet.add<int8_t>(info.weaponSpeed);
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void SkillsPacket::endSpecialSkill(Player *player, const SpecialSkillInfo &info) {
	if (player->getActiveBuffs()->isUsingHide()) {
		return;
	}
	PacketCreator packet;
	packet.addHeader(SMSG_SPECIAL_SKILL_END);
	packet.add<int32_t>(player->getId());
	packet.add<int32_t>(info.skillId);
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void SkillsPacket::showMagnetSuccess(Player *player, int32_t mapmobid, uint8_t success) {
	PacketCreator packet;
	packet.addHeader(SMSG_MOB_DRAGGED);
	packet.add<int32_t>(mapmobid);
	packet.add<uint8_t>(success);
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void SkillsPacket::sendCooldown(Player *player, int32_t skillid, int16_t time) {
	PacketCreator packet;
	packet.addHeader(SMSG_SKILL_COOLDOWN);
	packet.add<int32_t>(skillid);
	packet.add<int16_t>(time);
	player->getSession()->send(packet);
}

void SkillsPacket::showBerserk(Player *player, uint8_t level, bool on) { // Sends to map/user
	PacketCreator packet;
	packet.addHeader(SMSG_THEATRICS);
	packet.add<int8_t>(1);
	packet.add<int32_t>(Jobs::DarkKnight::Berserk);
	packet.add<int8_t>(level);
	packet.addBool(on);
	player->getSession()->send(packet);
	if (player->getActiveBuffs()->isUsingHide()) {
		return;
	}
	packet = PacketCreator();
	packet.addHeader(SMSG_SKILL_SHOW); // For others
	packet.add<int32_t>(player->getId());
	packet.add<int8_t>(1);
	packet.add<int32_t>(Jobs::DarkKnight::Berserk);
	packet.add<int8_t>(level);
	packet.addBool(on);
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}
