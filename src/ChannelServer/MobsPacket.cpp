/*
Copyright (C) 2008 Vana Development Team

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
#include "ReadPacket.h"
#include "SendHeader.h"

void MobsPacket::spawnMob(Player *player, Mob *mob, bool requestControl, bool spawn, bool show) {
	PacketCreator packet;
	if (requestControl) {
		packet.addShort(SEND_CONTROL_MOB);
		packet.addByte(1); // TODO: Aggro variable
	}
	else
		packet.addShort(SEND_SHOW_MOB);

	packet.addInt(mob->getID());
	packet.addByte(5);
	packet.addInt(mob->getMobID());
	packet.addInt(0);
	packet.addPos(mob->getPos());
	packet.addByte(2); // Stance
	packet.addShort(0);
	packet.addShort(mob->getFH());
	packet.addShort(spawn ? -2 : -1);
	packet.addInt(0);
	if (requestControl || show)
		player->getSession()->send(packet);
	else
		Maps::maps[mob->getMapID()]->sendPacket(packet);
}

void MobsPacket::endControlMob(Player *player, Mob *mob) {
	PacketCreator packet;
	packet.addShort(SEND_CONTROL_MOB);
	packet.addByte(0);
	packet.addInt(mob->getID());
	player->getSession()->send(packet);
}

void MobsPacket::moveMobResponse(Player *player, int32_t mobid, int16_t moveid, bool useskill, int32_t mp) {
	PacketCreator packet;
	packet.addShort(SEND_MOVE_MOB_RESPONSE);
	packet.addInt(mobid);
	packet.addShort(moveid);
	packet.addByte(useskill);
	packet.addInt(mp);
	player->getSession()->send(packet);
}

void MobsPacket::moveMob(Player *player, int32_t mobid, bool useskill, int32_t skill, unsigned char *buf, int32_t len) {
	PacketCreator packet;
	packet.addShort(SEND_MOVE_MOB);
	packet.addInt(mobid);
	packet.addByte(useskill);
	packet.addInt(skill);
	packet.addByte(0);
	packet.addBuffer(buf, len);
	Maps::maps[player->getMap()]->sendPacket(packet, player);
}

void MobsPacket::damageMob(Player *player, ReadPacket *pack) {
	pack->skipBytes(1);
	uint8_t tbyte = pack->getByte();
	uint8_t targets = tbyte / 0x10;
	uint8_t hits = tbyte % 0x10;
	int32_t skillid = pack->getInt();
	bool s4211006 = false;
	if (skillid == 4211006) {
		tbyte = (targets * 0x10) + 0x0A;
		s4211006 = true;
	}
	PacketCreator packet;
	packet.addShort(SEND_DAMAGE_MOB);
	packet.addInt(player->getId());
	packet.addByte(tbyte);
	if (skillid > 0) {
		packet.addByte(player->getSkills()->getSkillLevel(skillid));
		packet.addInt(skillid);
	} 
	else
		packet.addByte(0);
	packet.addByte(pack->getByte()); // Projectile display
	packet.addByte(pack->getByte()); // Direction/animation
	pack->skipBytes(1); // Weapon subclass
	packet.addByte(pack->getByte()); // Weapon speed
	pack->skipBytes(4); // Ticks
	if (skillid == 5201002) {
		pack->skipBytes(4); // Charge
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
					masteryid = (player->getSkills()->getSkillLevel(1100000) >= player->getSkills()->getSkillLevel(1200000) ? 1100000 : 1100000);
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
	packet.addByte((masteryid > 0 ? ((player->getSkills()->getSkillLevel(masteryid) + 1) / 2) : 0));
	packet.addInt(0);
	for (int8_t i = 0; i < targets; i++) {
		int32_t mapmobid = pack->getInt();
		packet.addInt(mapmobid);
		packet.addByte(0x06);
		pack->skipBytes(12);
		if (s4211006) {
			packet.addByte(hits);
			pack->skipBytes(1);
		}
		else
			pack->skipBytes(2);
		for (int8_t j = 0; j < hits; j++) {
			int32_t damage = pack->getInt();
			packet.addInt(damage);
		}
		pack->skipBytes(4);
	}
	Maps::maps[player->getMap()]->sendPacket(packet, player);
}

void MobsPacket::damageMobRanged(Player *player, ReadPacket *pack) {
	pack->skipBytes(1);
	uint8_t tbyte = pack->getByte();
	int8_t targets = tbyte / 0x10;
	int8_t hits = tbyte % 0x10;
	int32_t skillid = pack->getInt();
	switch (skillid) {
		case 3121004:
		case 3221001:
		case 5221004:
			pack->skipBytes(4);
			break;
	}
	bool shadow_meso = (skillid == 4111004);
	uint8_t display = pack->getByte(); // Projectile display
	uint8_t animation = pack->getByte(); // Direction/animation
	uint8_t w_class = pack->getByte(); // Weapon subclass
	uint8_t w_speed = pack->getByte(); // Weapon speed
	pack->skipBytes(4); // Ticks
	int16_t slot = pack->getShort(); // Slot
	int16_t csstar = pack->getShort(); // Cash Shop star
	if (!shadow_meso) {
		if ((display & 0x40) == 0x40)
			// Shadow Claw/+Shadow Partner = 0x40/0x48 - bitwise and with 0x40 = 0x40 for both
			pack->skipBytes(4); // Shadow Claw star ID
	}
	PacketCreator packet;
	packet.addShort(SEND_DAMAGE_MOB_RANGED);
	packet.addInt(player->getId());
	packet.addByte(tbyte);
	if (skillid > 0) {
		packet.addByte(player->getSkills()->getSkillLevel(skillid));
		packet.addInt(skillid);
	}
	else
		packet.addByte(0);
	packet.addByte(display);
	packet.addByte(animation);
	packet.addByte(w_speed);
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
	packet.addByte((masteryid > 0 ? ((player->getSkills()->getSkillLevel(masteryid) + 1) / 2) : 0));
	// Bug in global:
	// The colored swoosh does not display as it should
	int32_t itemid = 0;
	if (!shadow_meso) {
		if (csstar > 0)
			itemid = player->getInventory()->getItem(5, csstar)->id;
		else if (slot > 0)
			itemid = player->getInventory()->getItem(2, slot)->id;
	}
	packet.addInt(itemid);
	pack->skipBytes(1); // 0x00 = AoE, 0x41 = other
	for (int8_t i = 0; i < targets; i++) {
		int32_t mobid = pack->getInt();
		packet.addInt(mobid);
		packet.addByte(0x06);
		pack->skipBytes(14);
		for (int8_t j = 0; j < hits; j++) {
			int32_t damage = pack->getInt();
			switch (skillid) {
				case 3221007: // Snipe is always crit
					damage += 0x80000000; // Critical damage = 0x80000000 + damage
					break;
				default:
					break;
			}
			packet.addInt(damage); 
		}
		pack->skipBytes(4);
	}
	Maps::maps[player->getMap()]->sendPacket(packet, player);
}

void MobsPacket::damageMobSpell(Player *player, ReadPacket *pack) {
	pack->skipBytes(1);
	uint8_t tbyte = pack->getByte();
	int8_t targets = tbyte / 0x10;
	int8_t hits = tbyte % 0x10;
	PacketCreator packet;
	packet.addShort(SEND_DAMAGE_MOB_SPELL);
	packet.addInt(player->getId());
	packet.addByte(tbyte);
	packet.addByte(1); // Spells are always a skill
	int32_t skillid = pack->getInt();
	int32_t charge = 0;
	packet.addInt(skillid);
	if (skillid == 2121001 || skillid == 2221001 || skillid == 2321001) // Big Bang has a 4 byte charge time after skillid
		charge = pack->getInt();
	packet.addByte(pack->getByte()); // Projectile display
	packet.addByte(pack->getByte()); // Direction/animation
	pack->skipBytes(1); // Weapon subclass
	packet.addByte(pack->getByte()); // Casting speed
	pack->skipBytes(4); // Ticks
	packet.addByte(0); // Mastery byte is always 0 because spells don't have a swoosh
	packet.addInt(0); // No clue
	for (int8_t i = 0; i < targets; i++) {
		int32_t mobid = pack->getInt();
		packet.addInt(mobid);
		packet.addByte(-1);
		pack->skipBytes(3); // Useless crap for display
		pack->skipBytes(1); // State
		pack->skipBytes(10); // Useless crap for display continued
		for (int8_t j = 0; j < hits; j++) {
			int32_t damage = pack->getInt();
			packet.addInt(damage);
		}
		pack->skipBytes(4);
	}
	if (charge > 0)
		packet.addInt(charge);
	Maps::maps[player->getMap()]->sendPacket(packet, player);
}

void MobsPacket::showHP(Player *player, int32_t mobid, int8_t per) {
	PacketCreator packet;
	packet.addShort(SEND_SHOW_MOB_HP);
	packet.addInt(mobid);
	packet.addByte(per);
	player->getSession()->send(packet);
}
// Miniboss HP
void MobsPacket::showMinibossHP(Player *player, int32_t mobid, int8_t per) {
	PacketCreator packet;
	packet.addShort(SEND_SHOW_MOB_HP);
	packet.addInt(mobid);
	packet.addByte(per);
	Maps::maps[player->getMap()]->sendPacket(packet);
}
// Boss hp
void MobsPacket::showBossHP(Player *player, const MobHPInfo &mob) {
	PacketCreator packet;
	packet.addShort(SEND_MAP_EFFECT);
	packet.addByte(0x05);
	packet.addInt(mob.mobid);
	packet.addInt(mob.hp);
	packet.addInt(mob.mhp);
	packet.addByte(mob.hpcolor);
	packet.addByte(mob.hpbgcolor);
	Maps::maps[player->getMap()]->sendPacket(packet);
}

void MobsPacket::dieMob(Mob *mob) {
	Player *control = mob->getControl();
	if (control != 0 && control->getMap() == mob->getMapID())
		endControlMob(control, mob);

	PacketCreator packet;
	packet.addShort(SEND_KILL_MOB);
	packet.addInt(mob->getID());
	packet.addByte(1);
	Maps::maps[mob->getMapID()]->sendPacket(packet);
}
