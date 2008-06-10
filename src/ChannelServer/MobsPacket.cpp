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
#include "BufferUtilities.h"
#include "SendHeader.h"

void MobsPacket::controlMob(Player *player, Mob* mob) {
	Packet packet;
	packet.addHeader(SEND_CONTROL_MOB);
	packet.addByte(1);
	packet.addInt(mob->getID());
	packet.addByte(1);
	packet.addInt(mob->getMobID());
	packet.addInt(0);
	packet.addShort(mob->getPos().x);
	packet.addShort(mob->getPos().y-1);
	packet.addByte(mob->getType());
	packet.addShort(0);
	packet.addShort(mob->getFH());
	packet.addShort(-1);
	packet.send(player);
}
void MobsPacket::endControlMob(Player *player, Mob* mob) {
	Packet packet;
	packet.addHeader(SEND_CONTROL_MOB);
	packet.addByte(0);
	packet.addInt(mob->getID());
	packet.send(player);
}

void MobsPacket::spawnMob(vector <Player*> players, Mob* mob) {
	Packet packet;
	packet.addHeader(SEND_SHOW_MOB);
	packet.addInt(mob->getID());
	packet.addByte(1);
	packet.addInt(mob->getMobID());
	packet.addInt(0);
	packet.addShort(mob->getPos().x);
	packet.addShort(mob->getPos().y-1);
	packet.addByte(mob->getType());
	packet.addShort(0);
	packet.addShort(mob->getFH());
	packet.addShort(-2);
	packet.sendTo<Player>(NULL, players, true);
}

void MobsPacket::showMob(Player *player, Mob* mob) {
	Packet packet;
	packet.addHeader(SEND_SHOW_MOB);
	packet.addInt(mob->getID());
	packet.addByte(1);
	packet.addInt(mob->getMobID());
	packet.addInt(0);
	packet.addShort(mob->getPos().x);
	packet.addShort(mob->getPos().y-1);
	packet.addByte(mob->getType());
	packet.addShort(0);
	packet.addShort(mob->getFH());
	packet.addShort(-1);
	packet.send(player);
}

void MobsPacket::moveMobResponse(Player *player, int mobid, short moveid, bool useskill, int mp) {
	Packet packet;
	packet.addHeader(SEND_MOVE_MOB2);
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
	packet.sendTo<Player>(player, players, 0);
}

void MobsPacket::damageMob(Player *player, vector <Player*> players, unsigned char* pack) {
	int howmany = pack[1]/0x10;
	int hits = pack[1]%0x10;
	int skillid = BufferUtilities::getInt(pack+2);
	bool s4211006 = false;
	if (skillid == 4211006)
		s4211006 = true;
	Packet packet;
	packet.addHeader(SEND_DAMAGE_MOB);
	packet.addInt(player->getPlayerid());
	packet.addByte(pack[1]);
	if (BufferUtilities::getInt(pack+2) > 0) {
		packet.addByte(-1);
		packet.addInt(BufferUtilities::getInt(pack+2));
	} 
	else
		packet.addByte(0);
	packet.addByte(pack[7]);
	packet.addByte(pack[9]);
	packet.addByte(10);
	packet.addInt(0);
	for (int i=0; i<howmany; i++) {
		int mobid = BufferUtilities::getInt(pack+14+i*(22-s4211006+4*(hits-1)));
		packet.addInt(mobid);
		packet.addByte(-1);
		for (int j=0; j<hits; j++) {
			int damage = BufferUtilities::getInt(pack+32-s4211006+i*(22-s4211006+4*(hits-1))+j*4);
			packet.addInt(damage);
		}
	}
	packet.sendTo<Player>(player, players, 0);
}


void MobsPacket::damageMobRanged(Player *player, vector <Player*> players, unsigned char* pack, int itemid) {
	int howmany = pack[1]/0x10;
	int hits = pack[1]%0x10;
	int skillid = BufferUtilities::getInt(pack+2);
	short offset = 0;
	if (skillid == 3121004 || skillid == 3221001)
		offset = 4;
	Packet packet;
	packet.addHeader(SEND_DAMAGE_MOB_RANGED);
	packet.addInt(player->getPlayerid());
	packet.addByte(pack[1]);
	if (BufferUtilities::getInt(pack+2)>0) {
		packet.addByte(1);
		packet.addInt(BufferUtilities::getInt(pack+2));
	} 
	else
		packet.addByte(0);
	packet.addByte(pack[7+offset]);
	packet.addByte(pack[9+offset]);
	packet.addByte(pack[13+offset]);
	packet.addInt(itemid);
	for (int i=0; i<howmany; i++) {
		int mobid = BufferUtilities::getInt(pack+19+offset+i*(22+4*(hits-1)));
		packet.addInt(mobid);
		packet.addByte(-1);
		for (int j=0; j<hits; j++) {
			int damage = BufferUtilities::getInt(pack+37+offset+i*(22+4*(hits-1))+j*4);
			packet.addInt(damage);
		}
	}
	packet.sendTo<Player>(player, players, 0);
}

void MobsPacket::damageMobSpell(Player *player, vector <Player*> players, unsigned char* pack) {
	int howmany = pack[1]/0x10;
	int hits = pack[1]%0x10;
	Packet packet;
	packet.addHeader(SEND_DAMAGE_MOB_SPELL);
	packet.addInt(player->getPlayerid());
	packet.addByte(pack[1]);
	packet.addByte(1);
	int skillid = BufferUtilities::getInt(pack+2);
	packet.addInt(skillid);
	short offset = 0;
	if (skillid == 2121001 || skillid == 2221001 || skillid == 2321001) // Big Bang has a 4 byte charge time after skillid
		offset = 4;
	packet.addByte(pack[7+offset]);
	packet.addByte(pack[9+offset]);
	packet.addByte(0);
	packet.addInt(0);
	for (int i=0; i<howmany; i++) {
		int mobid = BufferUtilities::getInt(pack+14+offset+i*(22+4*(hits-1)));
		packet.addInt(mobid);
		packet.addByte(-1);
		for (int j=0; j<hits; j++) {
			int damage = BufferUtilities::getInt(pack+32+offset+i*(22+4*(hits-1))+j*4);
			packet.addInt(damage);
		}
	}
	if (offset)
		packet.addInt(BufferUtilities::getInt(pack+6));
	packet.sendTo<Player>(player, players, 0);
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
void MobsPacket::showBossHP(Player *player, vector <Player*> players, MobHPInfoStruct mob) {
	Packet packet;
	packet.addHeader(SEND_MAP_EFFECT);
	packet.addByte(0x05);
	packet.addInt(mob.mobid);
	packet.addInt(mob.hp);
	packet.addInt(mob.mhp);
	packet.addByte((char)mob.hpcolor);
	packet.addByte((char)mob.hpbgcolor);
	packet.sendTo(player, players, true);
}

void MobsPacket::dieMob(Player *player, vector<Player*> players, Mob* mob, int mobid) {
	Packet packet;
	packet.addHeader(SEND_KILL_MOB);
	packet.addInt(mobid);
	packet.addByte(1);
	packet.sendTo<Player>(player, players, 1); 
	if (mob->getControl() != NULL && mob->getControl()->getMap() == player->getMap()) {
		Packet packet;
		packet.addHeader(SEND_CONTROL_MOB);
		packet.addByte(0);
		packet.addInt(mobid);
		packet.send(mob->getControl());
	}
}
