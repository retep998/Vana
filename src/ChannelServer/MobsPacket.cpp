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
#include "PacketCreator.h"
#include "Player.h"
#include "Mobs.h"
#include "MobsPacket.h"
#include "SendHeader.h"
#include "ReadPacket.h"
#include "Maps.h"

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
		player->getPacketHandler()->send(packet);
	else
		Maps::maps[mob->getMapID()]->sendPacket(packet);
}

void MobsPacket::endControlMob(Player *player, Mob *mob) {
	PacketCreator packet;
	packet.addShort(SEND_CONTROL_MOB);
	packet.addByte(0);
	packet.addInt(mob->getID());
	player->getPacketHandler()->send(packet);
}

void MobsPacket::moveMobResponse(Player *player, int mobid, short moveid, bool useskill, int mp) {
	PacketCreator packet;
	packet.addShort(SEND_MOVE_MOB_RESPONSE);
	packet.addInt(mobid);
	packet.addShort(moveid);
	packet.addByte(useskill);
	packet.addInt(mp);
	player->getPacketHandler()->send(packet);
}

void MobsPacket::moveMob(Player *player, int mobid, bool useskill, int skill, unsigned char *buf, int len) {
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
	unsigned char tbyte = pack->getByte();
	unsigned char targets = tbyte / 0x10;
	unsigned char hits = tbyte % 0x10;
	int skillid = pack->getInt();
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
		packet.addByte(-1);
		packet.addInt(skillid);
	} 
	else
		packet.addByte(0);
	packet.addByte(0);
	pack->skipBytes(1); // Projectile display
	packet.addByte(pack->getByte()); // Direction/animation
	pack->skipBytes(1); // Weapon subclass
	packet.addByte(pack->getByte()); // Weapon speed
	pack->skipBytes(4); // Ticks
	if (skillid == 5201002) {
		pack->skipBytes(4); // Charge
	}
	packet.addByte(10);
	packet.addInt(0);
	for (char i = 0; i < targets; i++) {
		int mapmobid = pack->getInt();
		packet.addInt(mapmobid);
		packet.addByte(0x06);
		pack->skipBytes(12);
		if (s4211006) {
			packet.addByte(hits);
			pack->skipBytes(1);
		}
		else
			pack->skipBytes(2);
		for (char j = 0; j < hits; j++) {
			int damage = pack->getInt();
			packet.addInt(damage);
		}
		pack->skipBytes(4);
	}
	Maps::maps[player->getMap()]->sendPacket(packet, player);
}

void MobsPacket::damageMobRanged(Player *player, ReadPacket *pack) {
	pack->skipBytes(1);
	unsigned char tbyte = pack->getByte();
	char targets = tbyte / 0x10;
	char hits = tbyte % 0x10;
	int skillid = pack->getInt();
	switch (skillid) {
		case 3121004:
		case 3221001:
		case 5221004:
			pack->skipBytes(4);
			break;
	}
	bool shadow_meso = (skillid == 4111004);
	unsigned char display = pack->getByte(); // Projectile display
	unsigned char animation = pack->getByte(); // Direction/animation
	unsigned char w_class = pack->getByte(); // Weapon subclass
	unsigned char w_speed = pack->getByte(); // Weapon speed
	pack->skipBytes(4); // Ticks
	short slot = pack->getShort(); // Slot
	short csstar = pack->getShort(); // Cash Shop star
	if (!shadow_meso)
		if ((display & 0x40) > 0) // Shadow Claw star ID
			pack->skipBytes(4);

	PacketCreator packet;
	packet.addShort(SEND_DAMAGE_MOB_RANGED);
	packet.addInt(player->getId());
	packet.addByte(tbyte);
	if (skillid > 0) {
		switch (w_class) { // No clue why it does this, but it does, maybe has something to do with the mastery byte?
			case 0x03: packet.addByte(0x07); break; // Bow
			case 0x04: packet.addByte(0x0D); break; // Crossbow
			case 0x07:
				if (shadow_meso) 
					packet.addByte(player->getSkills()->getSkillLevel(skillid));
				else
					packet.addByte(0x0A); // Claw
				break;
			case 0x09: // Gun
				packet.addByte(0x10); break; // TODO: Find proper byte for guns
		}
		packet.addInt(skillid);
	}
	else
		packet.addByte(0);
	packet.addByte(display);
	packet.addByte(animation);
	packet.addByte(w_speed);
	packet.addByte(0x0A); // Mastery display byte, I think - needs VEDB extension and weapon type (2H BW, 1H BW, etc.) segregation
	int itemid = 0;
	if (!shadow_meso) {
		if (csstar > 0)
			itemid = player->getInventory()->getItem(5, csstar)->id;
		else if (slot > 0)
			itemid = player->getInventory()->getItem(2, slot)->id;
	}
	packet.addInt(itemid);
	pack->skipBytes(1); // 0x00 = AoE, 0x41 = other
	for (char i = 0; i < targets; i++) {
		int mobid = pack->getInt();
		packet.addInt(mobid);
		packet.addByte(0x06);
		pack->skipBytes(14);
		for (char j = 0; j < hits; j++) {
			int damage = pack->getInt();
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
	unsigned char tbyte = pack->getByte();
	char targets = tbyte / 0x10;
	char hits = tbyte % 0x10;
	PacketCreator packet;
	packet.addShort(SEND_DAMAGE_MOB_SPELL);
	packet.addInt(player->getId());
	packet.addByte(tbyte);
	packet.addByte(1); // Spells are always a skill
	int skillid = pack->getInt();
	int charge = 0;
	packet.addInt(skillid);
	if (skillid == 2121001 || skillid == 2221001 || skillid == 2321001) // Big Bang has a 4 byte charge time after skillid
		charge = pack->getInt();
	packet.addByte(0);
	pack->skipBytes(1); // Display
	packet.addByte(pack->getByte()); // Direction/animation
	pack->skipBytes(1); // Weapon subclass
	packet.addByte(pack->getByte()); // Casting speed
	pack->skipBytes(4); // Ticks
	packet.addByte(0); // No clue
	packet.addInt(0); // No clue
	for (char i = 0; i < targets; i++) {
		int mobid = pack->getInt();
		packet.addInt(mobid);
		packet.addByte(-1);
		pack->skipBytes(3); // Useless crap for display
		pack->skipBytes(1); // State
		pack->skipBytes(10); // Useless crap for display continued
		for (char j = 0; j < hits; j++) {
			int damage = pack->getInt();
			packet.addInt(damage);
		}
		pack->skipBytes(4);
	}
	if (charge > 0)
		packet.addInt(charge);
	Maps::maps[player->getMap()]->sendPacket(packet, player);
}

void MobsPacket::showHP(Player *player, int mobid, char per) {
	PacketCreator packet;
	packet.addShort(SEND_SHOW_MOB_HP);
	packet.addInt(mobid);
	packet.addByte(per);
	player->getPacketHandler()->send(packet);
}
// Miniboss HP
void MobsPacket::showMinibossHP(Player *player, int mobid, char per) {
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
