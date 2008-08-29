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
#include "SkillsPacket.h"
#include "Player.h"
#include "PacketCreator.h"
#include "Skills.h"
#include "SendHeader.h"
#include "Maps.h"

void SkillsPacket::addSkill(Player *player, int skillid, int level, int maxlevel) {
	PacketCreator packet;
	packet.addShort(SEND_ADD_SKILL);
	packet.addByte(1);
	packet.addShort(1);
	packet.addInt(skillid);
	packet.addInt(level); // Level
	packet.addInt(maxlevel); // Master Level
	packet.addByte(1);
	player->getPacketHandler()->send(packet);
}

void SkillsPacket::showSkill(Player *player, int skillid, unsigned char level) {
	if (player->getSkills()->getActiveSkillLevel(9101004) > 0)
		return;
 	PacketCreator packet;
 	packet.addShort(SEND_SHOW_SKILL);
 	packet.addInt(player->getId());
 	packet.addByte(1);
 	packet.addInt(skillid);
	packet.addByte(level); //TODO
	Maps::maps[player->getMap()]->sendPacket(packet, player);
}

void SkillsPacket::useSkill(Player *player, int skillid, int time, SkillActiveInfo pskill, SkillActiveInfo mskill, short addedinfo) {
	PacketCreator packet;
	packet.addShort(SEND_USE_SKILL);
	packet.addInt64(0);
	for (char i = 0; i < 8; i++)
		packet.addByte(pskill.types[i]);
	for (size_t i = 0; i < pskill.vals.size(); i++) {
		packet.addShort(pskill.vals[i]);
		packet.addInt(skillid);
		packet.addInt(time);
	}
	packet.addShort(0);
	packet.addShort(addedinfo);
	packet.addByte(0); // Number of times you've been buffed total - only certain skills have this part
	player->getPacketHandler()->send(packet);
	if (player->getSkills()->getActiveSkillLevel(9101004) > 0)
		return;
	if (mskill.vals.size() > 0) {
		packet = PacketCreator();
		packet.addShort(SEND_SHOW_OTHERS_SKILL);
		packet.addInt(player->getId());
		packet.addInt64(0);
		for (char i = 0; i < 8; i++)
			packet.addByte(mskill.types[i]);
		for (size_t i = 0; i < mskill.vals.size(); i++)
			packet.addShort(mskill.vals[i]);
		packet.addShort(0);
		Maps::maps[player->getMap()]->sendPacket(packet, player);
	}
}

void SkillsPacket::healHP(Player *player, short hp) {
	PacketCreator packet;
	packet.addShort(SEND_GAIN_ITEM);
	packet.addByte(0xA);
	packet.addShort(hp);
	player->getPacketHandler()->send(packet);
}

void SkillsPacket::endSkill(Player *player, SkillActiveInfo pskill, SkillActiveInfo mskill) {
	PacketCreator packet;
	packet.addShort(SEND_CANCEL_SKILL);
	packet.addInt64(0);
	for (char i = 0; i < 8; i++)
		packet.addByte(pskill.types[i]);
	packet.addByte(0);
	player->getPacketHandler()->send(packet);
	if (player->getSkills()->getActiveSkillLevel(9101004) > 0)
		return;
	if (mskill.vals.size() > 0) {
		packet = PacketCreator();
		packet.addShort(SEND_CANCEL_OTHERS_BUFF);
		packet.addInt(player->getId());
		packet.addInt64(0);
		for (char i = 0; i < 8; i++)
			packet.addByte(mskill.types[i]);
		Maps::maps[player->getMap()]->sendPacket(packet, player);
	}
}

void SkillsPacket::showSkillEffect(Player *player, int skillid, unsigned char level) {
	PacketCreator packet;
	packet.addShort(SEND_GAIN_ITEM); // For the using player
	bool send = false;
	switch (skillid) { 
		case 2100000:
		case 2200000:
		case 2300000: // MP Eater
			packet.addByte(1);
			packet.addInt(skillid);
			packet.addByte(1);
			send = true;
			break;
		case 1311008: // Dragon Blood
			packet.addByte(5);
			packet.addInt(skillid);
			send = true;
			break;
	}
	if (send)
		player->getPacketHandler()->send(packet);
	if (player->getSkills()->getActiveSkillLevel(9101004) > 0)
		return;
	packet = PacketCreator();
	send = false;
	packet.addShort(SEND_SHOW_SKILL);  // For others
	packet.addInt(player->getId());
	switch (skillid) {
		case 2100000:
		case 2200000:
		case 2300000: // MP Eater
			packet.addByte(1);
			packet.addInt(skillid);
			packet.addByte(1);
			send = true;
			break;
		case 4211005: // Meso Guard
		case 1311008: // Dragon Blood
			packet.addByte(5);
			packet.addInt(skillid);
			send = true;
			break;
	}
	if (send)
		Maps::maps[player->getMap()]->sendPacket(packet, player);
}

void SkillsPacket::showSpecialSkill(Player *player, SpecialSkillInfo info) { // Hurricane, Pierce, Big Bang, Monster Magnet
	if (player->getSkills()->getActiveSkillLevel(9101004) > 0)
		return;
	PacketCreator packet;
	packet.addShort(SEND_SPECIAL_SKILL);
	packet.addInt(player->getId());
	packet.addInt(info.skillid);
	packet.addByte(info.level);
	packet.addByte(info.direction);
	packet.addByte(info.w_speed);
	Maps::maps[player->getMap()]->sendPacket(packet, player);
}

void SkillsPacket::endSpecialSkill(Player *player, SpecialSkillInfo info) {
	if (player->getSkills()->getActiveSkillLevel(9101004) > 0)
		return;
	PacketCreator packet;
	packet.addShort(SEND_SPECIAL_SKILL_END);
	packet.addInt(player->getId());
	packet.addInt(info.skillid);
	Maps::maps[player->getMap()]->sendPacket(packet, player);
}

void SkillsPacket::showMagnetSuccess(Player *player, int mapmobid, unsigned char success) {
	PacketCreator packet;
	packet.addShort(SEND_SHOW_DRAGGED);
	packet.addInt(mapmobid);
	packet.addByte(success);
	Maps::maps[player->getMap()]->sendPacket(packet, player);
}

void SkillsPacket::sendCooldown(Player *player, int skillid, short time) {
	PacketCreator packet;
	packet.addShort(SEND_COOLDOWN);
	packet.addInt(skillid);
	packet.addShort(time);
	player->getPacketHandler()->send(packet);
}

void SkillsPacket::showBerserk(Player *player, unsigned char level, bool on) { // Sends to map/user
	PacketCreator packet;
	packet.addShort(SEND_GAIN_ITEM);
	packet.addByte(1);
	packet.addInt(1320006);
	packet.addByte(level);
	packet.addByte((on ? 1 : 0));
	player->getPacketHandler()->send(packet);
	if (player->getSkills()->getActiveSkillLevel(9101004) > 0)
		return;
	packet = PacketCreator();
	packet.addShort(SEND_SHOW_SKILL);  // For others
	packet.addInt(player->getId());
	packet.addByte(1);
	packet.addInt(1320006);
	packet.addByte(level);
	packet.addByte((on ? 1 : 0));
	Maps::maps[player->getMap()]->sendPacket(packet, player);
}