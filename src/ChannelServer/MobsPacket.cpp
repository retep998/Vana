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

void MobsPacket::controlMob(Player *player, Mob *mob) {
	Packet packet;
	packet.addHeader(SEND_CONTROL_MOB);
	packet.addByte(1);
	packet.addInt(mob->getID());
	packet.addByte(1);
	packet.addInt(mob->getMobID());
	packet.addShort(0);
	packet.addByte(0);
	packet.addInt(mob->getHP());
	packet.addByte(0);
	packet.addPos(mob->getPos());
	packet.addByte(mob->getType());
	packet.addShort(0);
	packet.addShort(mob->getFH());
	packet.addShort(-1);
	packet.addInt(0);
	packet.send(player);
}

void MobsPacket::endControlMob(Player *player, Mob *mob) {
	Packet packet;
	packet.addHeader(SEND_CONTROL_MOB);
	packet.addByte(0);
	packet.addInt(mob->getID());
	packet.send(player);
}

void MobsPacket::spawnMob(vector <Player*> players, Mob *mob) {
	Packet packet;
	packet.addHeader(SEND_SHOW_MOB);
	packet.addInt(mob->getID());
	packet.addByte(1);
	packet.addInt(mob->getMobID());
	packet.addShort(0);
	packet.addByte(0);
	packet.addInt(mob->getHP());
	packet.addByte(0);
	packet.addPos(mob->getPos());
	packet.addByte(mob->getType());
	packet.addShort(0);
	packet.addShort(mob->getFH());
	packet.addShort(-2);
	packet.addInt(0);
	packet.sendTo<Player>(0, players, true);
}

void MobsPacket::showMob(Player *player, Mob *mob) {
	Packet packet;
	packet.addHeader(SEND_SHOW_MOB);
	packet.addInt(mob->getID());
	packet.addByte(1);
	packet.addInt(mob->getMobID());
	packet.addShort(0);
	packet.addByte(0);
	packet.addInt(mob->getHP());
	packet.addByte(0);
	packet.addPos(mob->getPos());
	packet.addByte(mob->getType());
	packet.addShort(0);
	packet.addShort(mob->getFH());
	packet.addShort(-1);
	packet.addInt(0);
	packet.send(player);
}

void MobsPacket::moveMobResponse(Player *player, int mobid, short moveid, bool useskill, int mp) {
	Packet packet;
	packet.addHeader(SEND_MOVE_MOB_RESPONSE);
	packet.addInt(mobid);
	packet.addShort(moveid);
	packet.addByte(useskill);
	packet.addInt(mp);
	packet.send(player);
}

void MobsPacket::moveMob(Player *player, vector <Player*> players, int mobid, bool useskill, int skill, unsigned char *buf, int len) {
	Packet packet;
	packet.addHeader(SEND_MOVE_MOB);
	packet.addInt(mobid);
	packet.addByte(useskill);
	packet.addInt(skill);
	packet.addByte(0);
	packet.addBuffer(buf, len);
	packet.sendTo(player, players, false);
}

void MobsPacket::damageMob(Player *player, vector <Player*> players, ReadPacket *pack) {
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
	Packet packet;
	packet.addHeader(SEND_DAMAGE_MOB);
	packet.addInt(player->getPlayerid());
	packet.addByte(tbyte);
	if (skillid > 0) {
		packet.addByte(-1);
		packet.addInt(skillid);
	} 
	else
		packet.addByte(0);
	pack->skipBytes(1); // Projectile display
	packet.addByte(pack->getByte()); // Direction/animation
	pack->skipBytes(1); // Weapon subclass
	packet.addByte(pack->getByte()); // Weapon speed
	pack->skipBytes(4); // Ticks
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
	}
	packet.sendTo(player, players, false);
}

void MobsPacket::damageMobRanged(Player *player, vector <Player*> players, ReadPacket *pack) {
	pack->skipBytes(1);
	unsigned char tbyte = pack->getByte();
	char targets = tbyte / 0x10;
	char hits = tbyte % 0x10;
	int skillid = pack->getInt();
	if (skillid == 3121004 || skillid == 3221001)
		pack->skipBytes(4);
	Packet packet;
	packet.addHeader(SEND_DAMAGE_MOB_RANGED);
	packet.addInt(player->getPlayerid());
	packet.addByte(tbyte);
	if (skillid > 0) {
		packet.addByte(1);
		packet.addInt(skillid);
	} 
	else
		packet.addByte(0);
	unsigned char display = pack->getByte(); // Projectile display
	packet.addByte(pack->getByte()); // Direction/animation
	pack->skipBytes(1); // Weapon subclass
	packet.addByte(pack->getByte()); // Weapon speed
	pack->skipBytes(4); // Ticks
	unsigned char slot = pack->getByte();
	pack->skipBytes(1); // The other pos byte
	short csstar = pack->getShort(); // Cash Shop star
	int itemid = 0;
	if (csstar > 0) {
		itemid = player->inv->getItem(5, csstar)->id;
		if (display == 0x40 || display == 0x48) // Skip itemid for Shadow Claw
			pack->skipBytes(4);
	}
	else if (player->skills->getActiveSkillLevel(4121006)) { // Shadow Claw puts the item ID in the packet
		itemid = player->inv->getItem(2, slot)->id;
	}
	else
		itemid = pack->getInt();
	packet.addByte(slot); // Inventory slot, I guess?
	packet.addInt(itemid);
	pack->skipBytes(1); // 0x00 = AoE, 0x41 = other
	for (char i = 0; i < targets; i++) {
		int mobid = pack->getInt();
		packet.addInt(mobid);
		packet.addByte(-1);
		pack->skipBytes(14);
		for (char j = 0; j < hits; j++) {
			int damage = pack->getInt();
			packet.addInt(damage); // Critical damage = 0x80000000 + damage
		}
	}
	packet.sendTo(player, players, false);
}

void MobsPacket::damageMobSpell(Player *player, vector <Player*> players, ReadPacket *pack) {
	pack->skipBytes(1);
	unsigned char tbyte = pack->getByte();
	char targets = tbyte / 0x10;
	char hits = tbyte % 0x10;
	Packet packet;
	packet.addHeader(SEND_DAMAGE_MOB_SPELL);
	packet.addInt(player->getPlayerid());
	packet.addByte(tbyte);
	packet.addByte(1); // Spells are always a skill
	int skillid = pack->getInt();
	int charge = 0;
	packet.addInt(skillid);
	if (skillid == 2121001 || skillid == 2221001 || skillid == 2321001) // Big Bang has a 4 byte charge time after skillid
		charge = pack->getInt();
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
	}
	if (charge > 0)
		packet.addInt(charge);
	packet.sendTo(player, players, false);
}

void MobsPacket::showHP(Player *player, int mobid, char per) {
	Packet packet;
	packet.addHeader(SEND_SHOW_MOB_HP);
	packet.addInt(mobid);
	packet.addByte(per);
	packet.send(player);
}
// Miniboss HP
void MobsPacket::showMinibossHP(Player *player, vector <Player*> players, int mobid, char per) {
	Packet packet;
	packet.addHeader(SEND_SHOW_MOB_HP);
	packet.addInt(mobid);
	packet.addByte(per);
	packet.sendTo(player, players, true);
}
// Boss hp
void MobsPacket::showBossHP(Player *player, vector <Player*> players, const MobHPInfo &mob) {
	Packet packet;
	packet.addHeader(SEND_MAP_EFFECT);
	packet.addByte(0x05);
	packet.addInt(mob.mobid);
	packet.addInt(mob.hp);
	packet.addInt(mob.mhp);
	packet.addByte(mob.hpcolor);
	packet.addByte(mob.hpbgcolor);
	packet.sendTo(player, players, true);
}

void MobsPacket::dieMob(vector <Player*> players, Mob *mob) {
	Packet packet;
	packet.addHeader(SEND_KILL_MOB);
	packet.addInt(mob->getID());
	packet.addByte(1);
	packet.sendTo<Player>(0, players, true);
	Player *control = mob->getControl();
	if (control != 0 && control->getMap() == mob->getMapID())
		endControlMob(control, mob);
}
