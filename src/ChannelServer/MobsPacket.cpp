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
#include "MobsPacket.h"
#include "MapleSession.h"
#include "Maps.h"
#include "Mob.h"
#include "PacketCreator.h"
#include "Player.h"
#include "SendHeader.h"

void MobsPacket::spawnMob(Player *player, Mob *mob, int8_t summoneffect, Mob *owner, bool spawn, bool show) {
	PacketCreator packet;
	packet.addHeader(SMSG_MOB_SHOW);
	packet.add<int32_t>(mob->getId());
	packet.add<int8_t>(mob->getControlStatus());
	packet.add<int32_t>(mob->getMobId());
	mob->statusPacket(packet); // Mob's status such as frozen, stunned, and etc

	packet.addPos(mob->getPos());

	int8_t bitfield = (owner != nullptr ? 0x08 : 0x02) | mob->getFacingDirection();
	// 0x02 - ???
	if (mob->canFly()) {
		bitfield |= 0x04;
	}

	packet.add<int8_t>(bitfield);

	packet.add<int16_t>(mob->getFh());
	packet.add<int16_t>(mob->getOriginFh());

	if (owner != nullptr) {
		packet.add<int8_t>(summoneffect != 0 ? summoneffect : -3);
		packet.add<int32_t>(owner->getId());
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

void MobsPacket::requestControl(Player *player, Mob *mob, bool spawn, Player *display) {
	PacketCreator packet;
	packet.addHeader(SMSG_MOB_CONTROL);
	packet.add<int8_t>(1);
	packet.add<int32_t>(mob->getId());
	packet.add<int8_t>(mob->getControlStatus());
	packet.add<int32_t>(mob->getMobId());

	mob->statusPacket(packet); // Mob's status such as frozen, stunned, and etc

	packet.addPos(mob->getPos());

	int8_t bitfield = 0x02 | mob->getFacingDirection();
	// 0x02 = ???
	if (mob->canFly()) {
		bitfield |= 0x04;
	}

	packet.add<int8_t>(bitfield);

	packet.add<int16_t>(mob->getFh());
	packet.add<int16_t>(mob->getOriginFh());
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

void MobsPacket::endControlMob(Player *player, Mob *mob) {
	PacketCreator packet;
	packet.addHeader(SMSG_MOB_CONTROL);
	packet.add<int8_t>(0);
	packet.add<int32_t>(mob->getId());
	if (player != nullptr) {
		player->getSession()->send(packet);
	}
	else {
		Maps::getMap(mob->getMapId())->sendPacket(packet);
	}
}

void MobsPacket::moveMobResponse(Player *player, int32_t mobid, int16_t moveid, bool useskill, int32_t mp, uint8_t skill, uint8_t level) {
	PacketCreator packet;
	packet.addHeader(SMSG_MOB_MOVEMENT);
	packet.add<int32_t>(mobid);
	packet.add<int16_t>(moveid);
	packet.addBool(useskill);
	packet.add<int16_t>(static_cast<int16_t>(mp));
	packet.add<uint8_t>(skill);
	packet.add<uint8_t>(level);
	player->getSession()->send(packet);
}

void MobsPacket::moveMob(Player *player, int32_t mobid, bool useskill, int8_t skill, const Pos &projectiletarget, unsigned char *buf, int32_t len) {
	PacketCreator packet;
	packet.addHeader(SMSG_MOB_CONTROL_MOVEMENT);
	packet.add<int32_t>(mobid);
	packet.addBool(useskill);
	packet.add<int8_t>(skill);
	packet.addPos(projectiletarget);
	packet.addBuffer(buf, len);
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void MobsPacket::healMob(Mob *mob, int32_t amount) {
	PacketCreator packet;
	packet.addHeader(SMSG_MOB_DAMAGE);
	packet.add<int32_t>(mob->getId());
	packet.add<int8_t>(0);
	packet.add<int32_t>(-amount);
	packet.add<int8_t>(0);
	packet.add<int8_t>(0);
	packet.add<int8_t>(0);
	Maps::getMap(mob->getMapId())->sendPacket(packet);
}

void MobsPacket::hurtMob(Mob *mob, int32_t amount) {
	PacketCreator packet;
	packet.addHeader(SMSG_MOB_DAMAGE);
	packet.add<int32_t>(mob->getId());
	packet.add<int8_t>(0);
	packet.add<int32_t>(amount);
	packet.add<int8_t>(0);
	packet.add<int8_t>(0);
	packet.add<int8_t>(0);
	Maps::getMap(mob->getMapId())->sendPacket(packet);
}

void MobsPacket::damageFriendlyMob(Mob *mob, int32_t damage) {
	PacketCreator packet;
	packet.addHeader(SMSG_MOB_DAMAGE);
	packet.add<int32_t>(mob->getId());
	packet.add<int8_t>(1);
	packet.add<int32_t>(damage);
	packet.add<int32_t>(mob->getHp());
	packet.add<int32_t>(mob->getMaxHp());
	Maps::getMap(mob->getMapId())->sendPacket(packet);
}

void MobsPacket::applyStatus(Mob *mob, int32_t statusmask, const vector<StatusInfo> &info, int16_t delay, const vector<int32_t> &reflection) {
	PacketCreator packet;
	packet.addHeader(SMSG_MOB_STATUS_ADDITION);
	packet.add<int32_t>(mob->getId());
	packet.add<int32_t>(statusmask);

	for (size_t i = 0; i < info.size(); i++) {
		packet.add<int16_t>(static_cast<int16_t>(info[i].val));
		if (info[i].skillid >= 0) {
			packet.add<int32_t>(info[i].skillid);
		}
		else {
			packet.add<int16_t>(info[i].mobskill);
			packet.add<int16_t>(info[i].level);
		}
		packet.add<int16_t>(-1); // Not sure what this is
	}

	for (size_t i = 0; i < reflection.size(); i++) {
		packet.add<int32_t>(reflection[i]);
	}

	packet.add<int16_t>(delay);

	uint8_t buffcount = info.size();
	if (reflection.size() > 0) {
		buffcount /= 2; // This gives 2 buffs per reflection but it's really one buff
	}
	packet.add<uint8_t>(buffcount);

	Maps::getMap(mob->getMapId())->sendPacket(packet);
}

void MobsPacket::removeStatus(Mob *mob, int32_t status) {
	PacketCreator packet;
	packet.addHeader(SMSG_MOB_STATUS_REMOVE);
	packet.add<int32_t>(mob->getId());
	packet.add<int32_t>(status);
	packet.add<int8_t>(1);
	Maps::getMap(mob->getMapId())->sendPacket(packet);
}

void MobsPacket::showHp(Player *player, int32_t mobid, int8_t per) {
	PacketCreator packet;
	packet.addHeader(SMSG_MOB_HP_DISPLAY);
	packet.add<int32_t>(mobid);
	packet.add<int8_t>(per);
	player->getSession()->send(packet);
}

void MobsPacket::showHp(int32_t mapid, int32_t mobid, int8_t per) {
	PacketCreator packet;
	packet.addHeader(SMSG_MOB_HP_DISPLAY);
	packet.add<int32_t>(mobid);
	packet.add<int8_t>(per);
	Maps::getMap(mapid)->sendPacket(packet);
}

void MobsPacket::showBossHp(Mob *mob) {
	PacketCreator packet;
	packet.addHeader(SMSG_MAP_EFFECT);
	packet.add<int8_t>(0x05);
	packet.add<int32_t>(mob->getMobId());
	packet.add<int32_t>(mob->getHp());
	packet.add<int32_t>(mob->getMaxHp());
	packet.add<int8_t>(mob->getHpBarColor());
	packet.add<int8_t>(mob->getHpBarBgColor());
	Maps::getMap(mob->getMapId())->sendPacket(packet);
}

void MobsPacket::dieMob(Mob *mob, int8_t death) {
	PacketCreator packet;
	packet.addHeader(SMSG_MOB_DEATH);
	packet.add<int32_t>(mob->getId());
	packet.add<int8_t>(death);
	Maps::getMap(mob->getMapId())->sendPacket(packet);
}

void MobsPacket::showSpawnEffect(int32_t mapid, int8_t summonEffect, const Pos &pos) {
	PacketCreator packet;
	packet.addHeader(SMSG_MAP_EFFECT);
	packet.add<int8_t>(0x00);
	packet.add<int8_t>(summonEffect);
	packet.addPos(pos, true);
	Maps::getMap(mapid)->sendPacket(packet);
}
