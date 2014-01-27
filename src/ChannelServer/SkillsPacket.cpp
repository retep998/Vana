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
#include "SkillsPacket.h"
#include "GameConstants.h"
#include "Maps.h"
#include "PacketCreator.h"
#include "Player.h"
#include "Session.h"
#include "Skills.h"
#include "Session.h"
#include "SmsgHeader.h"

auto SkillsPacket::addSkill(Player *player, int32_t skillId, const PlayerSkillInfo &skillInfo) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_SKILL_ADD);
	packet.add<int8_t>(1);
	packet.add<int16_t>(1);
	packet.add<int32_t>(skillId);
	packet.add<int32_t>(skillInfo.level); // Level
	packet.add<int32_t>(skillInfo.playerMaxSkillLevel); // Master Level
	packet.add<int8_t>(1);
	player->getSession()->send(packet);
}

auto SkillsPacket::showSkill(Player *player, int32_t skillId, uint8_t level, uint8_t direction, bool party, bool self) -> void {
	if (player->isUsingGmHide()) {
		return;
	}
	PacketCreator packet;
	if (party && self) {
		packet.add<header_t>(SMSG_THEATRICS);
	}
	else {
		packet.add<header_t>(SMSG_SKILL_SHOW);
		packet.add<int32_t>(player->getId());
	}
	packet.add<int8_t>(party ? 2 : 1);
	packet.add<int32_t>(skillId);
	packet.add<int8_t>(level); // TODO
	switch (skillId) {
		case Skills::Hero::MonsterMagnet: // Monster Magnet processing
		case Skills::Paladin::MonsterMagnet:
		case Skills::DarkKnight::MonsterMagnet:
			packet.add<uint8_t>(direction);
			break;
	}
	if (self) {
		player->getSession()->send(packet);
	}
	else {
		player->getMap()->sendPacket(packet, player);
	}
}

auto SkillsPacket::healHp(Player *player, int16_t hp) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_THEATRICS);
	packet.add<int8_t>(0x0A);
	packet.add<int16_t>(hp);
	player->getSession()->send(packet);
}

auto SkillsPacket::showSkillEffect(Player *player, int32_t skillId, uint8_t level) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_THEATRICS); // For the using player
	bool send = false;
	switch (skillId) {
		case Skills::FpWizard::MpEater:
		case Skills::IlWizard::MpEater:
		case Skills::Cleric::MpEater:
			packet.add<int8_t>(1);
			packet.add<int32_t>(skillId);
			packet.add<int8_t>(1);
			send = true;
			break;
		case Skills::DragonKnight::DragonBlood: // Dragon Blood
			packet.add<int8_t>(5);
			packet.add<int32_t>(skillId);
			send = true;
			break;
	}
	if (send) {
		player->getSession()->send(packet);
	}
	if (player->isUsingGmHide()) {
		return;
	}
	packet = PacketCreator();
	send = false;
	packet.add<header_t>(SMSG_SKILL_SHOW); // For others
	packet.add<int32_t>(player->getId());
	switch (skillId) {
		case Skills::FpWizard::MpEater:
		case Skills::IlWizard::MpEater:
		case Skills::Cleric::MpEater:
			packet.add<int8_t>(1);
			packet.add<int32_t>(skillId);
			packet.add<int8_t>(1);
			send = true;
			break;
		case Skills::ChiefBandit::MesoGuard:
		case Skills::DragonKnight::DragonBlood:
			packet.add<int8_t>(5);
			packet.add<int32_t>(skillId);
			send = true;
			break;
	}
	if (send) {
		player->getMap()->sendPacket(packet, player);
	}
}

auto SkillsPacket::showChargeOrStationarySkill(Player *player, const ChargeOrStationarySkillInfo &info) -> void {
	if (player->isUsingGmHide()) {
		return;
	}
	PacketCreator packet;
	packet.add<header_t>(SMSG_CHARGE_OR_STATIONARY_SKILL);
	packet.add<int32_t>(player->getId());
	packet.add<int32_t>(info.skillId);
	packet.add<int8_t>(info.level);
	packet.add<int8_t>(info.direction);
	packet.add<int8_t>(info.weaponSpeed);
	player->getMap()->sendPacket(packet, player);
}

auto SkillsPacket::endChargeOrStationarySkill(Player *player, const ChargeOrStationarySkillInfo &info) -> void {
	if (player->isUsingGmHide()) {
		return;
	}
	PacketCreator packet;
	packet.add<header_t>(SMSG_CHARGE_OR_STATIONARY_SKILL_END);
	packet.add<int32_t>(player->getId());
	packet.add<int32_t>(info.skillId);
	player->getMap()->sendPacket(packet, player);
}

auto SkillsPacket::showMagnetSuccess(Player *player, int32_t mapMobId, uint8_t success) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_MOB_DRAGGED);
	packet.add<int32_t>(mapMobId);
	packet.add<uint8_t>(success);
	player->getMap()->sendPacket(packet, player);
}

auto SkillsPacket::sendCooldown(Player *player, int32_t skillId, int16_t time) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_SKILL_COOLDOWN);
	packet.add<int32_t>(skillId);
	packet.add<int16_t>(time);
	player->getSession()->send(packet);
}

auto SkillsPacket::showBerserk(Player *player, uint8_t level, bool on) -> void {
	// Sends to map/user
	PacketCreator packet;
	packet.add<header_t>(SMSG_THEATRICS);
	packet.add<int8_t>(1);
	packet.add<int32_t>(Skills::DarkKnight::Berserk);
	packet.add<int8_t>(level);
	packet.add<bool>(on);
	player->getSession()->send(packet);
	if (player->isUsingGmHide()) {
		return;
	}
	packet = PacketCreator();
	packet.add<header_t>(SMSG_SKILL_SHOW); // For others
	packet.add<int32_t>(player->getId());
	packet.add<int8_t>(1);
	packet.add<int32_t>(Skills::DarkKnight::Berserk);
	packet.add<int8_t>(level);
	packet.add<bool>(on);
	player->getMap()->sendPacket(packet, player);
}