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
#include "MobsPacket.h"
#include "Maps.h"
#include "Mob.h"
#include "PacketCreator.h"
#include "Player.h"
#include "Session.h"
#include "SmsgHeader.h"
#include "StatusInfo.h"
#include "WidePos.h"

auto MobsPacket::spawnMob(Player *player, ref_ptr_t<Mob> mob, int8_t summonEffect, ref_ptr_t<Mob> owner, bool spawn, bool show) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_MOB_SHOW);
	packet.add<int32_t>(mob->getMapMobId());
	packet.add<int8_t>(static_cast<int8_t>(mob->getControlStatus()));
	packet.add<int32_t>(mob->getMobId());
	mob->statusPacket(packet); // Mob's status such as frozen, stunned, and etc

	packet.addClass<Pos>(mob->getPos());

	int8_t bitfield = (owner != nullptr ? 0x08 : 0x02) | (mob->isFacingLeft() ? 0x01 : 0);
	if (mob->canFly()) {
		bitfield |= 0x04;
	}

	packet.add<int8_t>(bitfield); // 0x08 - a summon, 0x04 - flying, 0x02 - ???, 0x01 - faces left

	packet.add<int16_t>(mob->getFoothold());
	packet.add<int16_t>(mob->getOriginFoothold());

	if (owner != nullptr) {
		packet.add<int8_t>(summonEffect != 0 ? summonEffect : -3);
		packet.add<int32_t>(owner->getMapMobId());
	}
	else {
		packet.add<int8_t>(spawn ? -2 : -1);
	}
	packet.add<int8_t>(-1);
	packet.add<int32_t>(0);
	if (show && player != nullptr) {
		player->getSession()->send(packet);
	}
	else {
		Maps::getMap(mob->getMapId())->sendPacket(packet);
	}
}

auto MobsPacket::requestControl(Player *player, ref_ptr_t<Mob> mob, bool spawn, Player *display) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_MOB_CONTROL);
	packet.add<int8_t>(1);
	packet.add<int32_t>(mob->getMapMobId());
	packet.add<int8_t>(static_cast<int8_t>(mob->getControlStatus()));
	packet.add<int32_t>(mob->getMobId());

	mob->statusPacket(packet); // Mob's status such as frozen, stunned, and etc

	packet.addClass<Pos>(mob->getPos());

	int8_t bitfield = 0x02 | (mob->isFacingLeft() ? 0x01 : 0);
	if (mob->canFly()) {
		bitfield |= 0x04;
	}

	packet.add<int8_t>(bitfield); // 0x08 - a summon, 0x04 - flying, 0x02 - ???, 0x01 - faces left

	packet.add<int16_t>(mob->getFoothold());
	packet.add<int16_t>(mob->getOriginFoothold());
	packet.add<int8_t>(spawn ? -2 : -1);
	packet.add<int8_t>(-1);
	packet.add<int32_t>(0);
	if (player != nullptr) {
		player->getSession()->send(packet);
	}
	else if (display != nullptr) {
		display->getSession()->send(packet);
	}
	else {
		Maps::getMap(mob->getMapId())->sendPacket(packet);
	}
}

auto MobsPacket::endControlMob(Player *player, int32_t mapId, int32_t mapMobId) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_MOB_CONTROL);
	packet.add<int8_t>(0);
	packet.add<int32_t>(mapMobId);
	if (player != nullptr) {
		player->getSession()->send(packet);
	}
	else {
		Maps::getMap(mapId)->sendPacket(packet);
	}
}

auto MobsPacket::moveMobResponse(Player *player, int32_t mapMobId, int16_t moveId, bool skillPossible, int32_t mp, uint8_t skill, uint8_t level) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_MOB_MOVEMENT);
	packet.add<int32_t>(mapMobId);
	packet.add<int16_t>(moveId);
	packet.add<bool>(skillPossible);
	packet.add<int16_t>(static_cast<int16_t>(mp));
	packet.add<uint8_t>(skill);
	packet.add<uint8_t>(level);
	player->getSession()->send(packet);
}

auto MobsPacket::moveMob(Player *player, int32_t mapMobId, bool skillPossible, int8_t rawAction, uint8_t skill, uint8_t level, int16_t option, unsigned char *buf, int32_t len) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_MOB_CONTROL_MOVEMENT);
	packet.add<int32_t>(mapMobId);
	packet.add<bool>(skillPossible);
	packet.add<int8_t>(rawAction);
	packet.add<uint8_t>(skill);
	packet.add<uint8_t>(level);
	packet.add<int16_t>(option);
	packet.addBuffer(buf, len);
	player->getMap()->sendPacket(packet, player);
}

auto MobsPacket::healMob(int32_t mapId, int32_t mapMobId, int32_t amount) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_MOB_DAMAGE);
	packet.add<int32_t>(mapMobId);
	packet.add<int8_t>(0);
	packet.add<int32_t>(-amount);
	packet.add<int8_t>(0);
	packet.add<int8_t>(0);
	packet.add<int8_t>(0);
	Maps::getMap(mapId)->sendPacket(packet);
}

auto MobsPacket::hurtMob(int32_t mapId, int32_t mapMobId, int32_t amount) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_MOB_DAMAGE);
	packet.add<int32_t>(mapMobId);
	packet.add<int8_t>(0);
	packet.add<int32_t>(amount);
	packet.add<int8_t>(0);
	packet.add<int8_t>(0);
	packet.add<int8_t>(0);
	Maps::getMap(mapId)->sendPacket(packet);
}

auto MobsPacket::damageFriendlyMob(ref_ptr_t<Mob> mob, int32_t damage) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_MOB_DAMAGE);
	packet.add<int32_t>(mob->getMapMobId());
	packet.add<int8_t>(1);
	packet.add<int32_t>(damage);
	packet.add<int32_t>(mob->getHp());
	packet.add<int32_t>(mob->getMaxHp());
	Maps::getMap(mob->getMapId())->sendPacket(packet);
}

auto MobsPacket::applyStatus(int32_t mapId, int32_t mapMobId, int32_t statusMask, const vector_t<StatusInfo> &info, int16_t delay, const vector_t<int32_t> &reflection) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_MOB_STATUS_ADDITION);
	packet.add<int32_t>(mapMobId);
	packet.add<int32_t>(statusMask);

	for (size_t i = 0; i < info.size(); i++) {
		packet.add<int16_t>(static_cast<int16_t>(info[i].val));
		if (info[i].skillId >= 0) {
			packet.add<int32_t>(info[i].skillId);
		}
		else {
			packet.add<int16_t>(info[i].mobSkill);
			packet.add<int16_t>(info[i].level);
		}
		packet.add<int16_t>(-1); // Not sure what this is
	}

	for (size_t i = 0; i < reflection.size(); i++) {
		packet.add<int32_t>(reflection[i]);
	}

	packet.add<int16_t>(delay);

	uint8_t buffCount = info.size();
	if (reflection.size() > 0) {
		buffCount /= 2; // This gives 2 buffs per reflection but it's really one buff
	}
	packet.add<int8_t>(buffCount);

	Maps::getMap(mapId)->sendPacket(packet);
}

auto MobsPacket::removeStatus(int32_t mapId, int32_t mapMobId, int32_t status) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_MOB_STATUS_REMOVE);
	packet.add<int32_t>(mapMobId);
	packet.add<int32_t>(status);
	packet.add<int8_t>(1);
	Maps::getMap(mapId)->sendPacket(packet);
}

auto MobsPacket::showHp(Player *player, int32_t mapMobId, int8_t percentage) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_MOB_HP_DISPLAY);
	packet.add<int32_t>(mapMobId);
	packet.add<int8_t>(percentage);
	player->getSession()->send(packet);
}

auto MobsPacket::showHp(int32_t mapId, int32_t mapMobId, int8_t percentage) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_MOB_HP_DISPLAY);
	packet.add<int32_t>(mapMobId);
	packet.add<int8_t>(percentage);
	Maps::getMap(mapId)->sendPacket(packet);
}

auto MobsPacket::showBossHp(ref_ptr_t<Mob> mob) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_MAP_EFFECT);
	packet.add<int8_t>(0x05);
	packet.add<int32_t>(mob->getMobId());
	packet.add<int32_t>(mob->getHp());
	packet.add<int32_t>(mob->getMaxHp());
	packet.add<int8_t>(mob->getHpBarColor());
	packet.add<int8_t>(mob->getHpBarBgColor());
	Maps::getMap(mob->getMapId())->sendPacket(packet);
}

auto MobsPacket::dieMob(int32_t mapId, int32_t mapMobId, int8_t death) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_MOB_DEATH);
	packet.add<int32_t>(mapMobId);
	packet.add<int8_t>(death);
	Maps::getMap(mapId)->sendPacket(packet);
}

auto MobsPacket::showSpawnEffect(int32_t mapId, int8_t summonEffect, const Pos &pos) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_MAP_EFFECT);
	packet.add<int8_t>(0x00);
	packet.add<int8_t>(summonEffect);
	packet.addClass<WidePos>(WidePos(pos));
	Maps::getMap(mapId)->sendPacket(packet);
}