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
#include "MobsPacket.h"
#include "MapleSession.h"
#include "Maps.h"
#include "Mobs.h"
#include "PacketCreator.h"
#include "Player.h"
#include "SendHeader.h"

void MobsPacket::spawnMob(Player *player, Mob *mob, int8_t summoneffect, Mob *owner, bool spawn, bool show) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_SHOW_MOB);
	packet.add<int32_t>(mob->getId());
	packet.add<int8_t>(1);
	packet.add<int32_t>(mob->getMobId());
	mob->statusPacket(packet); // Mob's status such as frozen, stunned, and etc
	packet.addPos(mob->getPos());

	int8_t bitfield = (owner != 0 ? 0x08 : 0x02) | mob->getFacingDirection();

	packet.add<int8_t>(bitfield); // 0x08 - a summon, 0x02 - ???, 0x01 - faces right
	packet.add<int16_t>(mob->getFh());
	packet.add<int16_t>(mob->getOriginFh());
	if (owner != 0) {
		packet.add<int8_t>(summoneffect != 0 ? summoneffect : -3);
	}
	else {
		packet.add<int8_t>(spawn ? -2 : -1);
	}
	if (owner != 0)
		packet.add<int32_t>(owner->getId());
	packet.add<int8_t>(-1);
	packet.add<int32_t>(0);
	if (show) {
		player->getSession()->send(packet);
	}
	else {
		Maps::getMap(mob->getMapId())->sendPacket(packet);
	}
}

void MobsPacket::requestControl(Player *player, Mob *mob, bool spawn) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_CONTROL_MOB);
	packet.add<int8_t>(1);
	packet.add<int32_t>(mob->getId());
	packet.add<int8_t>(1);
	packet.add<int32_t>(mob->getMobId());
	mob->statusPacket(packet); // Mob's status such as frozen, stunned, and etc
	packet.add<int32_t>(0);
	packet.addPos(mob->getPos());
	packet.add<int8_t>(2); // Not stance, exploring further
	packet.add<int16_t>(mob->getFh());
	packet.add<int16_t>(mob->getOriginFh());
	packet.add<int16_t>(-1); // ??
	packet.add<int32_t>(0);
	player->getSession()->send(packet);
}

void MobsPacket::endControlMob(Player *player, Mob *mob) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_CONTROL_MOB);
	packet.add<int8_t>(0);
	packet.add<int32_t>(mob->getId());
	player->getSession()->send(packet);
}

void MobsPacket::moveMobResponse(Player *player, int32_t mobid, int16_t moveid, bool useskill, int32_t mp, uint8_t skill, uint8_t level) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_MOVE_MOB_RESPONSE);
	packet.add<int32_t>(mobid);
	packet.add<int16_t>(moveid);
	packet.add<int8_t>(useskill);
	packet.add<int16_t>(static_cast<int16_t>(mp));
	packet.add<uint8_t>(skill);
	packet.add<uint8_t>(level);
	player->getSession()->send(packet);
}

void MobsPacket::moveMob(Player *player, int32_t mobid, bool useskill, int8_t skill, Pos target, unsigned char *buf, int32_t len) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_MOVE_MOB);
	packet.add<int32_t>(mobid);
	packet.add<int8_t>(useskill);
	packet.add<int8_t>(skill);
	packet.addPos(target);
	packet.addBuffer(buf, len);
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void MobsPacket::healMob(Mob *mob, int32_t amount) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_DAMAGE_MOB);
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
	packet.add<int16_t>(SEND_DAMAGE_MOB);
	packet.add<int32_t>(mob->getId());
	packet.add<int8_t>(0);
	packet.add<int32_t>(amount);
	packet.add<int8_t>(0);
	packet.add<int8_t>(0);
	packet.add<int8_t>(0);
	Maps::getMap(mob->getMapId())->sendPacket(packet);
}

void MobsPacket::applyStatus(Mob *mob, int32_t statusmask, const vector<StatusInfo> &info, int16_t delay, const vector<int32_t> &reflection) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_APPLY_MOB_STATUS);
	packet.add<int32_t>(mob->getId());
	packet.add<int32_t>(statusmask);

	for (size_t i = 0; i < info.size(); i++) {
		packet.add<int16_t>(info[i].val);
		if (info[i].skillid >= 0) {
			packet.add<int32_t>(info[i].skillid);
		}
		else {
			packet.add<int16_t>(info[i].mobskill);
			packet.add<int16_t>(info[i].level);
		}
		packet.add<int16_t>(0); // Not sure what this is
	}

	for (size_t i = 0; i < reflection.size(); i++) {
		packet.add<int32_t>(reflection[i]);
	}

	packet.add<int16_t>(delay);
	
	int8_t buffcount = info.size();
	if (reflection.size() > 0) {
		buffcount /= 2; // This gives 2 buffs per reflection but it's really one buff
	}
	packet.add<int8_t>(buffcount);

	Maps::getMap(mob->getMapId())->sendPacket(packet);
}

void MobsPacket::removeStatus(Mob *mob, int32_t status) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_REMOVE_MOB_STATUS);
	packet.add<int32_t>(mob->getId());
	packet.add<int32_t>(status);
	packet.add<int8_t>(1);
	Maps::getMap(mob->getMapId())->sendPacket(packet);
}

void MobsPacket::showHp(Player *player, int32_t mobid, int8_t per, bool miniboss) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_SHOW_MOB_HP);
	packet.add<int32_t>(mobid);
	packet.add<int8_t>(per);
	if (miniboss)
		Maps::getMap(player->getMap())->sendPacket(packet);
	else
		player->getSession()->send(packet);
}
// Boss hp
void MobsPacket::showBossHp(Player *player, int32_t mobid, int32_t hp, const MobInfo &info) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_MAP_EFFECT);
	packet.add<int8_t>(0x05);
	packet.add<int32_t>(mobid);
	packet.add<int32_t>(hp);
	packet.add<int32_t>(info.hp);
	packet.add<int8_t>(info.hpcolor);
	packet.add<int8_t>(info.hpbgcolor);
	Maps::getMap(player->getMap())->sendPacket(packet);
}

void MobsPacket::dieMob(Mob *mob, int8_t death) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_KILL_MOB);
	packet.add<int32_t>(mob->getId());
	packet.add<int8_t>(death);
	Maps::getMap(mob->getMapId())->sendPacket(packet);
}
