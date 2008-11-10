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
#include "Summons.h"
#include "SummonsPacket.h"
#include "Skills.h"
#include "Maps.h"
#include "LoopingId.h"
#include "PlayerPacket.h"
#include "PacketReader.h"
#include "Movement.h"

// Summon Class
Summon::Summon(int32_t id, int32_t summonid, uint8_t level) : id(id), summonid(summonid), level(level), hp(0) {
	switch (summonid) {
		case 3111002:
		case 3211002:
			hp = Skills::skills[summonid][level].x; // Get HP for puppet
		case 5211001:
			type = 0; // No movement - Puppets and Octopus
			break;
		case 2311006:
		case 3111005:
		case 3211005:
		case 3121006:
		case 3221005:
		case 5211002:
			type = 3; // Flying - Birds and Priest dragon
			break;
		default:
			type = 1; // Follow - 4th job mage summons
			break;
	}
}

// Summons namespace
LoopingId Summons::summonids;

void Summons::useSummon(Player *player, int32_t skillid, uint8_t level) {
	Summon *summon = new Summon(summonids.next(), skillid, level);
	bool puppet = ISPUPPET(skillid);
	removeSummon(player, puppet, true, false);
	if (puppet)
		summon->setPos( Maps::maps[player->getMap()]->findFloor(Pos((player->getPos().x + 200 * (player->getStance() % 2 == 1 ? -1 : 1)), player->getPos().y)) );
	else
		summon->setPos(player->getPos());
	player->getSummons()->addSummon(summon, Skills::skills[skillid][level].time);
	SummonsPacket::showSummon(player, summon, true);
}

void Summons::removeSummon(Player *player, bool puppet, bool animated, bool packetOnly, bool fromTimer, bool showMessage) {
	Summon *summon = puppet ? player->getSummons()->getPuppet() : player->getSummons()->getSummon();
	if (summon != 0) {
		SummonsPacket::removeSummon(player, summon, animated);
		if (!packetOnly) {
			player->getSummons()->removeSummon(puppet, fromTimer);
			if (showMessage)
				PlayerPacket::showMessage(player, "Summon has run out of time and will disappear.", 5);
		}
	}
}

void Summons::showSummon(Player *player) {
	if (Summon *summon = player->getSummons()->getSummon()) {
		summon->setPos(player->getPos());
		SummonsPacket::showSummon(player, summon, false);
	}
}

void Summons::showSummons(Player *ofplayer, Player *toplayer) {
	if (Summon *summon = ofplayer->getSummons()->getSummon())
		SummonsPacket::showSummon(ofplayer, summon, false, toplayer);
	if (Summon *puppet = ofplayer->getSummons()->getPuppet())
		SummonsPacket::showSummon(ofplayer, puppet, false, toplayer);
}

void Summons::moveSummon(Player *player, PacketReader &packet) {
	int32_t summonid = packet.getInt();
	packet.skipBytes(4); // I am not certain what this is, but in the Odin source they seemed to think it was original position. However, it caused AIDS.
	Summon *summon = player->getSummons()->getSummon(summonid);
	if (summon == 0)
		// Up to no good, lag, or something else
		return;
	Pos startPos = summon->getPos(); // Original gangsta
	Movement::parseMovement(summon, packet);
	packet.reset(10);
	SummonsPacket::moveSummon(player, summon, startPos, packet.getBuffer(), (packet.getBufferLength() - 9));
}

void Summons::damageSummon(Player *player, PacketReader &packet) {
	int32_t summonid = packet.getInt();
	int8_t notsure = packet.getByte();
	int32_t damage = packet.getInt();
	int32_t mobid = packet.getInt();

	if (Summon *summon = player->getSummons()->getPuppet()) {
		summon->doDamage(damage);
		//SummonsPacket::damageSummon(player, summonid, notsure, damage, mobid); // TODO: Find out if this packet even sends anymore
		if (summon->getHP() <= 0)
			removeSummon(player, true, true, false, false, true);
	}
}
