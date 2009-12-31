/*
Copyright (C) 2008-2010 Vana Development Team

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
#include "GameConstants.h"
#include "GameLogicUtilities.h"
#include "LoopingId.h"
#include "Maps.h"
#include "MovementHandler.h"
#include "PacketReader.h"
#include "PlayerPacket.h"
#include "SkillDataProvider.h"
#include "SummonsPacket.h"

// Summon Class
Summon::Summon(int32_t id, int32_t summonid, uint8_t level) : id(id), summonid(summonid), level(level), hp(0) {
	switch (summonid) {
		case Jobs::Ranger::Puppet:
		case Jobs::Sniper::Puppet:
		case Jobs::WindArcher::Puppet:
			hp = SkillDataProvider::Instance()->getSkill(summonid, level)->x; // Get HP for puppet
		case Jobs::Outlaw::Octopus:
		case Jobs::Corsair::WrathOfTheOctopi:
			type = 0; // No movement - Puppets and Octopus
			break;
		case Jobs::Priest::SummonDragon:
		case Jobs::Ranger::SilverHawk:
		case Jobs::Sniper::GoldenEagle:
		case Jobs::Bowmaster::Phoenix:
		case Jobs::Marksman::Frostprey:
		case Jobs::Outlaw::Gaviota:
			type = 3; // Flying - Birds and Priest dragon
			break;
		default:
			type = 1; // Follow - 4th job mage summons
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
	removeSummon(player, puppet, true, false, 0);
	Pos sumpos = player->getPos();
	if (puppet)
		sumpos = Maps::getMap(player->getMap())->findFloor(Pos((player->getPos().x + 200 * (player->isFacingRight() ? 1 : -1)), player->getPos().y));
	summon->setPos(sumpos);
	player->getSummons()->addSummon(summon, SkillDataProvider::Instance()->getSkill(skillid, level)->time);
	SummonsPacket::showSummon(player, summon, true);
}

void Summons::removeSummon(Player *player, bool puppet, bool animated, bool packetOnly, int8_t showMessage, bool fromTimer) {
	Summon *summon = puppet ? player->getSummons()->getPuppet() : player->getSummons()->getSummon();
	if (summon != 0) {
		SummonsPacket::removeSummon(player, summon, animated);
		if (!packetOnly) {
			string name = getSummonName(summon->getSummonId());
			player->getSummons()->removeSummon(puppet, fromTimer);
			switch (showMessage) {
				case 1: PlayerPacket::showMessage(player, name + " has run out of time and will disappear.", 5); break;
				case 2: PlayerPacket::showMessage(player, name + " is disappearing.", 5); break;
			}
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
	int32_t summonid = packet.get<int32_t>();
	packet.skipBytes(4); // I am not certain what this is, but in the Odin source they seemed to think it was original position. However, it caused AIDS.
	Summon *summon = player->getSummons()->getSummon(summonid);
	if (summon == 0)
		// Up to no good, lag, or something else
		return;
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
		if (summon->getHP() <= 0)
			removeSummon(player, true, true, false, false, true);
	}
}

string Summons::getSummonName(int32_t summonid) {
	string ret = "Summon";
	switch (summonid) {
		case Jobs::Bishop::Bahamut: ret = "Bahamut"; break;
		case Jobs::Outlaw::Gaviota: ret = "Gaviota"; break;
		case Jobs::Outlaw::Octopus:
		case Jobs::Corsair::WrathOfTheOctopi: ret = "Octopus"; break;
		case Jobs::Sniper::Puppet:
		case Jobs::Ranger::Puppet:
		case Jobs::WindArcher::Puppet: ret = "Puppet"; break;
		case Jobs::Priest::SummonDragon: ret = "Summon Dragon"; break;
		case Jobs::Ranger::SilverHawk: ret = "Silver Hawk"; break;
		case Jobs::Sniper::GoldenEagle: ret = "Golden Eagle"; break;
		case Jobs::DarkKnight::Beholder: ret = "Beholder"; break;
		case Jobs::FPArchMage::Elquines: ret = "Elquines"; break;
		case Jobs::ILArchMage::Ifrit: ret = "Ifrit"; break;
		case Jobs::Bowmaster::Phoenix: ret = "Phoenix"; break;
		case Jobs::Marksman::Frostprey: ret = "Frostprey"; break;
		case Jobs::DawnWarrior::Soul: ret = "Soul"; break;
		case Jobs::BlazeWizard::Ifrit: ret = "Ifrit"; break;
		case Jobs::BlazeWizard::Flame: ret = "Flame"; break;
		case Jobs::WindArcher::Storm: ret = "Storm"; break;
		case Jobs::NightWalker::Darkness: ret = "Darkness"; break;
		case Jobs::ThunderBreaker::Lightning: ret = "Lightning"; break;
	}
	return ret;
}