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
#include "Inventory.h"
#include "MapleSession.h"
#include "Maps.h"
#include "Mobs.h"
#include "MobsPacket.h"
#include "PacketCreator.h"
#include "Player.h"
#include "PacketReader.h"
#include "SendHeader.h"

void MobsPacket::spawnMob(Player *player, Mob *mob, Mob *owner, bool spawn, bool show) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_SHOW_MOB);
	packet.add<int32_t>(mob->getID());
	packet.add<int16_t>(1);
	packet.add<int32_t>(mob->getMobID());
	mob->statusPacket(packet); // Mob's status such as frozen, stunned, and etc
	packet.add<int32_t>(0);
	packet.addPos(mob->getPos());
	packet.add<int16_t>(owner != 0 ? 0x08 : 0x02); // Not stance, exploring further
	packet.add<int16_t>(mob->getFH());
	packet.add<int16_t>(mob->getOriginFH());
	packet.add<int16_t>(owner != 0 ? -3 : spawn ? -2 : -1);
	if (owner != 0)
		packet.add<int32_t>(owner->getID());
	packet.add<int16_t>(-1);
	packet.add<int32_t>(0);
	if (show)
		player->getSession()->send(packet);
	else
		Maps::getMap(mob->getMapID())->sendPacket(packet);
}

void MobsPacket::requestControl(Player *player, Mob *mob, bool spawn) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_CONTROL_MOB);
	packet.add<int16_t>(1);
	packet.add<int32_t>(mob->getID());
	packet.add<int16_t>(1);
	packet.add<int32_t>(mob->getMobID());
	mob->statusPacket(packet); // Mob's status such as frozen, stunned, and etc
	packet.add<int32_t>(0);
	packet.addPos(mob->getPos());
	packet.add<int16_t>(2); // Not stance, exploring further
	packet.add<int16_t>(mob->getFH());
	packet.add<int16_t>(mob->getOriginFH());
	packet.add<int16_t>(-1); // ??
	packet.add<int32_t>(0);
	player->getSession()->send(packet);
}

void MobsPacket::endControlMob(Player *player, Mob *mob) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_CONTROL_MOB);
	packet.add<int16_t>(0);
	packet.add<int32_t>(mob->getID());
	player->getSession()->send(packet);
}

void MobsPacket::moveMobResponse(Player *player, int32_t mobid, int16_t moveid, bool useskill, int32_t mp) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_MOVE_MOB_RESPONSE);
	packet.add<int32_t>(mobid);
	packet.add<int16_t>(moveid);
	packet.add<int16_t>(useskill);
	packet.add<int32_t>(mp);
	player->getSession()->send(packet);
}

void MobsPacket::moveMob(Player *player, int32_t mobid, bool useskill, int32_t skill, unsigned char *buf, int32_t len) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_MOVE_MOB);
	packet.add<int32_t>(mobid);
	packet.add<int16_t>(useskill);
	packet.add<int32_t>(skill);
	packet.add<int16_t>(0);
	packet.addBuffer(buf, len);
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void MobsPacket::damageMob(Player *player, PacketReader &pack) {
	pack.skipBytes(1);
	uint8_t tbyte = pack.get<int8_t>();
	uint8_t targets = tbyte / 0x10;
	uint8_t hits = tbyte % 0x10;
	int32_t skillid = pack.get<int32_t>();
	bool s4211006 = false;
	if (skillid == 4211006) {
		tbyte = (targets * 0x10) + 0x0A;
		s4211006 = true;
	}
	PacketCreator packet;
	packet.add<int16_t>(SEND_DAMAGE_MOB_MELEE);
	packet.add<int32_t>(player->getId());
	packet.add<int16_t>(tbyte);
	if (skillid > 0) {
		packet.add<int16_t>(player->getSkills()->getSkillLevel(skillid));
		packet.add<int32_t>(skillid);
	} 
	else
		packet.add<int16_t>(0);
	packet.add<int16_t>(pack.get<int8_t>()); // Projectile display
	packet.add<int16_t>(pack.get<int8_t>()); // Direction/animation
	pack.skipBytes(1); // Weapon subclass
	packet.add<int16_t>(pack.get<int8_t>()); // Weapon speed
	pack.skipBytes(4); // Ticks
	if (skillid == 5201002 || skillid == 5101004) {
		pack.skipBytes(4); // Charge
	}
	int32_t masteryid = 0;
	switch (GETWEAPONTYPE(player->getInventory()->getEquippedID(11))) {
		case WEAPON_1H_SWORD:
		case WEAPON_2H_SWORD:
			switch ((player->getJob() / 10)) {
				case 11: masteryid = 1100000; break;
				case 12: masteryid = 1200000; break;
				case 90:
				case 91:
					masteryid = (player->getSkills()->getSkillLevel(1100000) >= player->getSkills()->getSkillLevel(1200000) ? 1100000 : 1200000);
					break;
			}
			break;
		case WEAPON_1H_AXE:
		case WEAPON_2H_AXE:
			masteryid = 1100001;
			break;
		case WEAPON_1H_MACE:
		case WEAPON_2H_MACE:
			masteryid = 1200001;
			break;
		case WEAPON_SPEAR:
			masteryid = 1300000;
			break;
		case WEAPON_POLEARM:
			masteryid = 1300001;
			break;
		case WEAPON_DAGGER:
			masteryid = 4200000;
			break;
		case WEAPON_KNUCKLE:
			masteryid = 5100001;
			break;
	}
	packet.add<int16_t>((masteryid > 0 ? ((player->getSkills()->getSkillLevel(masteryid) + 1) / 2) : 0));
	packet.add<int32_t>(0);
	for (int8_t i = 0; i < targets; i++) {
		int32_t mapmobid = pack.get<int32_t>();
		packet.add<int32_t>(mapmobid);
		packet.add<int16_t>(0x06);
		pack.skipBytes(12);
		if (s4211006) {
			hits = pack.get<int8_t>();
			packet.add<int16_t>(hits);
		}
		else
			pack.skipBytes(2);
		for (int8_t j = 0; j < hits; j++) {
			int32_t damage = pack.get<int32_t>();
			packet.add<int32_t>(damage);
		}
		pack.skipBytes(4);
	}
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void MobsPacket::damageMobRanged(Player *player, PacketReader &pack) {
	pack.skipBytes(1);
	uint8_t tbyte = pack.get<int8_t>();
	int8_t targets = tbyte / 0x10;
	int8_t hits = tbyte % 0x10;
	int32_t skillid = pack.get<int32_t>();
	switch (skillid) {
		case 3121004:
		case 3221001:
		case 5221004:
			pack.skipBytes(4);
			break;
	}
	bool shadow_meso = (skillid == 4111004);
	uint8_t display = pack.get<int8_t>(); // Projectile display
	uint8_t animation = pack.get<int8_t>(); // Direction/animation
	uint8_t w_class = pack.get<int8_t>(); // Weapon subclass
	uint8_t w_speed = pack.get<int8_t>(); // Weapon speed
	pack.skipBytes(4); // Ticks
	int16_t slot = pack.get<int16_t>(); // Slot
	int16_t csstar = pack.get<int16_t>(); // Cash Shop star
	if (!shadow_meso) {
		if ((display & 0x40) == 0x40)
			// Shadow Claw/+Shadow Partner = 0x40/0x48 - bitwise and with 0x40 = 0x40 for both
			pack.skipBytes(4); // Shadow Claw star ID
	}
	PacketCreator packet;
	packet.add<int16_t>(SEND_DAMAGE_MOB_RANGED);
	packet.add<int32_t>(player->getId());
	packet.add<int16_t>(tbyte);
	if (skillid > 0) {
		packet.add<int16_t>(player->getSkills()->getSkillLevel(skillid));
		packet.add<int32_t>(skillid);
	}
	else
		packet.add<int16_t>(0);
	packet.add<int16_t>(display);
	packet.add<int16_t>(animation);
	packet.add<int16_t>(w_speed);
	int32_t masteryid = 0;
	switch (GETWEAPONTYPE(player->getInventory()->getEquippedID(11))) {
		case WEAPON_BOW:
			masteryid = 3100000;
			break;
		case WEAPON_CROSSBOW:
			masteryid = 3200000;
			break;
		case WEAPON_CLAW:
			masteryid = 4100000;
			break;
		case WEAPON_GUN:
			masteryid = 5200000;
			break;
	}
	packet.add<int16_t>((masteryid > 0 ? ((player->getSkills()->getSkillLevel(masteryid) + 1) / 2) : 0));
	// Bug in global:
	// The colored swoosh does not display as it should
	int32_t itemid = 0;
	if (!shadow_meso) {
		if (csstar > 0)
			itemid = player->getInventory()->getItem(5, csstar)->id;
		else if (slot > 0) {
			Item *item = player->getInventory()->getItem(2, slot);
			if (item != 0)
				itemid = item->id;
		}
	}
	packet.add<int32_t>(itemid);
	pack.skipBytes(1); // 0x00 = AoE, 0x41 = other
	for (int8_t i = 0; i < targets; i++) {
		int32_t mobid = pack.get<int32_t>();
		packet.add<int32_t>(mobid);
		packet.add<int16_t>(0x06);
		pack.skipBytes(14);
		for (int8_t j = 0; j < hits; j++) {
			int32_t damage = pack.get<int32_t>();
			switch (skillid) {
				case 3221007: // Snipe is always crit
					damage += 0x80000000; // Critical damage = 0x80000000 + damage
					break;
				default:
					break;
			}
			packet.add<int32_t>(damage); 
		}
		pack.skipBytes(4);
	}
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void MobsPacket::damageMobSpell(Player *player, PacketReader &pack) {
	pack.skipBytes(1);
	uint8_t tbyte = pack.get<int8_t>();
	int8_t targets = tbyte / 0x10;
	int8_t hits = tbyte % 0x10;
	PacketCreator packet;
	packet.add<int16_t>(SEND_DAMAGE_MOB_SPELL);
	packet.add<int32_t>(player->getId());
	packet.add<int16_t>(tbyte);
	packet.add<int16_t>(1); // Spells are always a skill
	int32_t skillid = pack.get<int32_t>();
	int32_t charge = 0;
	packet.add<int32_t>(skillid);
	if (skillid == 2121001 || skillid == 2221001 || skillid == 2321001) // Big Bang has a 4 byte charge time after skillid
		charge = pack.get<int32_t>();
	packet.add<int16_t>(pack.get<int8_t>()); // Projectile display
	packet.add<int16_t>(pack.get<int8_t>()); // Direction/animation
	pack.skipBytes(1); // Weapon subclass
	packet.add<int16_t>(pack.get<int8_t>()); // Casting speed
	pack.skipBytes(4); // Ticks
	packet.add<int16_t>(0); // Mastery byte is always 0 because spells don't have a swoosh
	packet.add<int32_t>(0); // No clue
	for (int8_t i = 0; i < targets; i++) {
		int32_t mobid = pack.get<int32_t>();
		packet.add<int32_t>(mobid);
		packet.add<int16_t>(-1);
		pack.skipBytes(3); // Useless crap for display
		pack.skipBytes(1); // State
		pack.skipBytes(10); // Useless crap for display continued
		for (int8_t j = 0; j < hits; j++) {
			int32_t damage = pack.get<int32_t>();
			packet.add<int32_t>(damage);
		}
		pack.skipBytes(4);
	}
	if (charge > 0)
		packet.add<int32_t>(charge);
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void MobsPacket::damageMobSummon(Player *player, PacketReader &pack) {
	int32_t summonid = pack.get<int32_t>();
	pack.skipBytes(5);
	int8_t targets = pack.get<int8_t>();
	PacketCreator packet;
	packet.add<int16_t>(SEND_DAMAGE_MOB_SUMMON);
	packet.add<int32_t>(player->getId());
	packet.add<int32_t>(summonid);
	packet.add<int16_t>(4);
	packet.add<int16_t>(targets);
	for (int8_t i = 0; i < targets; i++) {
		int32_t mobid = pack.get<int32_t>();
		packet.add<int32_t>(mobid);
		packet.add<int16_t>(6);

		pack.skipBytes(14); // Crap

		int32_t damage = pack.get<int32_t>();
		packet.add<int32_t>(damage);
	}
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void MobsPacket::applyStatus(Mob *mob, const StatusInfo &info, int16_t delay) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_APPLY_MOB_STATUS);
	packet.add<int32_t>(mob->getID());
	packet.add<int32_t>(info.status);

	packet.add<int16_t>(info.val);
	if (info.skillid >= 0) {
		packet.add<int32_t>(info.skillid);
	}
	else {
		packet.add<int16_t>(info.mobskill);
		packet.add<int16_t>(info.level);
	}
	packet.add<int16_t>(0);

	packet.add<int16_t>(delay);
	packet.add<int16_t>(1);
	Maps::getMap(mob->getMapID())->sendPacket(packet);
}

void MobsPacket::removeStatus(Mob *mob, int32_t status) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_REMOVE_MOB_STATUS);
	packet.add<int32_t>(mob->getID());
	packet.add<int32_t>(status);
	packet.add<int16_t>(1);
	Maps::getMap(mob->getMapID())->sendPacket(packet);
}

void MobsPacket::showHP(Player *player, int32_t mobid, int8_t per, bool miniboss) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_SHOW_MOB_HP);
	packet.add<int32_t>(mobid);
	packet.add<int16_t>(per);
	if (miniboss)
		Maps::getMap(player->getMap())->sendPacket(packet);
	else
		player->getSession()->send(packet);
}
// Boss hp
void MobsPacket::showBossHP(Player *player, int32_t mobid, int32_t hp, const MobInfo &info) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_MAP_EFFECT);
	packet.add<int16_t>(0x05);
	packet.add<int32_t>(mobid);
	packet.add<int32_t>(hp);
	packet.add<int32_t>(info.hp);
	packet.add<int16_t>(info.hpcolor);
	packet.add<int16_t>(info.hpbgcolor);
	Maps::getMap(player->getMap())->sendPacket(packet);
}

void MobsPacket::dieMob(Mob *mob) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_KILL_MOB);
	packet.add<int32_t>(mob->getID());
	packet.add<int16_t>(1);
	Maps::getMap(mob->getMapID())->sendPacket(packet);
}