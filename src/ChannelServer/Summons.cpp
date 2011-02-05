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
#include "Summons.h"
#include "GameLogicUtilities.h"
#include "LoopingId.h"
#include "Maps.h"
#include "MovementHandler.h"
#include "PacketReader.h"
#include "SkillConstants.h"
#include "SkillDataProvider.h"
#include "SummonsPacket.h"

// Summon Class
Summon::Summon(int32_t id, int32_t summonid, uint8_t level) :
id(id),
summonid(summonid),
level(level),
hp(0)
{
	switch (summonid) {
		case Jobs::Ranger::Puppet:
		case Jobs::Sniper::Puppet:
		case Jobs::WindArcher::Puppet:
			hp = SkillDataProvider::Instance()->getSkill(summonid, level)->x; // Get HP for puppet
		case Jobs::Outlaw::Octopus:
		case Jobs::Corsair::WrathOfTheOctopi:
			type = Static;
			break;
		case Jobs::Priest::SummonDragon:
		case Jobs::Ranger::SilverHawk:
		case Jobs::Sniper::GoldenEagle:
		case Jobs::Bowmaster::Phoenix:
		case Jobs::Marksman::Frostprey:
		case Jobs::Outlaw::Gaviota:
			type = Flying;
			break;
		default:
			type = Follow;
			break;
	}
}

// Summons namespace
LoopingId Summons::summonids;

int32_t Summons::loopId() {
	return summonids.next();
}

void Summons::useSummon(Player *player, int32_t skillid, uint8_t level) {
	Summon *summon = new Summon(loopId(), skillid, level);
	bool puppet = GameLogicUtilities::isPuppet(skillid);
	removeSummon(player, puppet, false, SummonMessages::None);
	Pos ppos = player->getPos();
	Pos sumpos;
	if (puppet) {
		int16_t x = ppos.x + 200 * (player->isFacingRight() ? 1 : -1);
		sumpos = Maps::getMap(player->getMap())->findFloor(Pos(x, ppos.y));
	}
	else {
		sumpos = ppos;
	}
	summon->setPos(sumpos);
	player->getSummons()->addSummon(summon, SkillDataProvider::Instance()->getSkill(skillid, level)->time);
	SummonsPacket::showSummon(player, summon, true);
}

void Summons::removeSummon(Player *player, bool puppet, bool packetOnly, int8_t showMessage, bool fromTimer) {
	// Maybe we don't need the packetOnly thing...? We don't use it anyway...

	Summon *summon = puppet ? player->getSummons()->getPuppet() : player->getSummons()->getSummon();
	if (summon != nullptr) {
		SummonsPacket::removeSummon(player, summon, showMessage);
		if (!packetOnly) {
			player->getSummons()->removeSummon(puppet, fromTimer);
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
	if (Summon *summon = ofplayer->getSummons()->getSummon()) {
		SummonsPacket::showSummon(ofplayer, summon, false, toplayer);
	}
	if (Summon *puppet = ofplayer->getSummons()->getPuppet()) {
		SummonsPacket::showSummon(ofplayer, puppet, false, toplayer);
	}
}

void Summons::moveSummon(Player *player, PacketReader &packet) {
	int32_t summonid = packet.get<int32_t>();

	// I am not certain what this is, but in the Odin source they seemed to think it was original position. However, it caused AIDS.
	packet.skipBytes(4);

	Summon *summon = player->getSummons()->getSummon(summonid);
	if (summon == nullptr || summon->getType() == Summon::Static) {
		// Up to no good, lag, or something else
		return;
	}

	Pos startPos = summon->getPos(); // Original gangsta
	MovementHandler::parseMovement(summon, packet);
	packet.reset(10);
	SummonsPacket::moveSummon(player, summon, startPos, packet.getBuffer(), (packet.getBufferLength() - 9));
}

void Summons::damageSummon(Player *player, PacketReader &packet) {
	int32_t summonid = packet.get<int32_t>();
	int8_t notsure = packet.get<int8_t>();
	int32_t damage = packet.get<int32_t>();
	int32_t mobid = packet.get<int32_t>();

	if (Summon *summon = player->getSummons()->getPuppet()) {
		summon->doDamage(damage);
		//SummonsPacket::damageSummon(player, summonid, notsure, damage, mobid); // TODO: Find out if this packet even sends anymore
		if (summon->getHP() <= 0) {
			removeSummon(player, true, false, SummonMessages::None, true);
		}
	}
}
