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
#include "PlayerHandler.h"
#include "Door.h"
#include "Drop.h"
#include "DropHandler.h"
#include "DropsPacket.h"
#include "GameLogicUtilities.h"
#include "InventoryPacket.h"
#include "ItemDataProvider.h"
#include "MapleTvs.h"
#include "Maps.h"
#include "Mist.h"
#include "MobHandler.h"
#include "MonsterBookPacket.h"
#include "MovementHandler.h"
#include "Player.h"
#include "PlayerDataProvider.h"
#include "PlayerPacket.h"
#include "PlayersPacket.h"
#include "Randomizer.h"
#include "PacketReader.h"
#include "SkillConstants.h"
#include "SkillDataProvider.h"
#include "Skills.h"
#include "SkillsPacket.h"
#include "Summons.h"
#include "Timer/Time.h"
#include "Timer/Timer.h"
#include <functional>

using std::tr1::bind;

void PlayerHandler::handleDoorUse(Player *player, PacketReader &packet) {
	int32_t doorid = packet.get<int32_t>();
	bool totown = !packet.getBool();
	Player *doorholder = PlayerDataProvider::Instance()->getPlayer(doorid);
	if (doorholder == nullptr || (doorholder->getParty() != player->getParty() && doorholder != player)) {
		// Hacking or lag
		return;
	}
	doorholder->getDoor()->warp(player, totown);
}

void PlayerHandler::handleDamage(Player *player, PacketReader &packet) {
	const int8_t BumpDamage = -1;
	const int8_t MapDamage = -2;

	packet.skipBytes(4); // Tickcount, why would you abuse this (difference is too low too)

	int8_t type = packet.get<int8_t>();
	packet.skipBytes(1); // Element - 0x00 = elementless, 0x01 = ice, 0x02 = fire, 0x03 = lightning
	int32_t damage = packet.get<int32_t>();
	bool applieddamage = false;
	bool deadlyattack = false;
	uint8_t hit = 0;
	uint8_t stance = 0;
	uint8_t disease = 0;
	uint8_t level = 0;
	uint16_t mpburn = 0;
	int32_t mapmobid = 0; // Map Mob ID
	int32_t mobid = 0; // Actual Mob ID - i.e. 8800000 for Zakum
	int32_t nodamageid = 0;
	Mob *mob = nullptr;
	ReturnDamageInfo pgmr;

	if (type != MapDamage) {
		mobid = packet.get<int32_t>();

		mapmobid = packet.get<int32_t>();
		mob = Maps::getMap(player->getMap())->getMob(mapmobid);
		if (mob == nullptr || mob->getMobId() != mobid) {
			// Hacking
			return;
		}

		mobid = mob->getMobId();
		if (type != BumpDamage) {
			int32_t attackerid = (mob->hasLink() ? mob->getLink() : mobid);
			MobAttackInfo *attack = MobDataProvider::Instance()->getMobAttack(attackerid, type);
			if (attack == nullptr) {
				// Hacking, I think
				return;
			}
			disease = attack->disease;
			level = attack->level;
			mpburn = attack->mpBurn;
			deadlyattack = attack->deadlyAttack;
		}
		hit = packet.get<uint8_t>(); // Knock direction
		pgmr.reduction = packet.get<uint8_t>();
		packet.skipBytes(1); // I think reduction is a short, but it's a byte in the S -> C packet, so..
		if (pgmr.reduction != 0) {
			pgmr.isPhysical = packet.getBool();
			pgmr.mapMobId = packet.get<int32_t>();
			packet.skipBytes(1); // 0x06 for Power Guard, 0x00 for Mana Reflection?
			packet.skipBytes(4); // Mob position garbage
			pgmr.pos = packet.getPos();
			pgmr.damage = damage;
			if (pgmr.isPhysical) {
				// Only Power Guard decreases damage
				damage = (damage - (damage * pgmr.reduction / 100));
			}
			mob->applyDamage(player->getId(), (pgmr.damage * pgmr.reduction / 100));
		}
	}

	if (type == MapDamage) {
		level = packet.get<uint8_t>();
		disease = packet.get<uint8_t>();
	}
	else {
		stance = packet.get<int8_t>(); // Power Stance
		if (stance > 0 && !player->getActiveBuffs()->hasPowerStance()) {
			// Hacking
			return;
		}
	}

	if (damage == -1) {
		if (!player->getSkills()->hasNoDamageSkill()) {
			// Hacking
			return;
		}
		nodamageid = player->getSkills()->getNoDamageSkill();
	}

	if (disease > 0 && damage != 0) {
		// Fake/Guardian don't prevent disease
		player->getActiveBuffs()->addDebuff(disease, level);
	}

	if (damage > 0 && !player->hasGmEquip()) {
		if (player->getActiveBuffs()->hasMesoGuard() && player->getInventory()->getMesos() > 0) {
			int32_t sid = player->getActiveBuffs()->getMesoGuard();
			int16_t mesorate = player->getActiveBuffs()->getActiveSkillInfo(sid)->x;
			int16_t mesoloss = (int16_t)(mesorate * damage / 2 / 100);
			int32_t mesos = player->getInventory()->getMesos();
			int32_t newmesos = mesos - mesoloss;

			if (newmesos < 0) {
				// Special damage calculation for not having enough mesos
				double reduction = 2.0 - ((double)(mesos / mesoloss)) / 2.0;
				damage = (uint16_t)(damage / reduction);
				// This puts us pretty close to the damage observed clientside
				// TODO: Improve formula
			}
			else {
				damage /= 2;
				// Usually displays 1 below the actual damage but is sometimes accurate - no clue why
			}

			player->getInventory()->setMesos(newmesos);
			player->getStats()->damageHp((uint16_t) damage);

			if (deadlyattack && player->getStats()->getMp() > 0) {
				player->getStats()->setMp(1);
			}
			if (mpburn > 0) {
				player->getStats()->damageMp(mpburn);
			}
			applieddamage = true;

			SkillsPacket::showSkillEffect(player, sid);
		}

		if (player->getActiveBuffs()->hasMagicGuard()) {
			int16_t mp = player->getStats()->getMp();
			int16_t hp = player->getStats()->getHp();

			if (deadlyattack) {
				if (mp > 0) {
					player->getStats()->setMp(1);
				}
				player->getStats()->setHp(1);
			}
			else if (mpburn > 0) {
				player->getStats()->damageMp(mpburn);
				player->getStats()->damageHp((uint16_t) damage);
			}
			else {
				int32_t sid = player->getActiveBuffs()->getMagicGuard();
				int16_t reduc = player->getActiveBuffs()->getActiveSkillInfo(sid)->x;
				uint16_t mpdamage = (uint16_t)((damage * reduc) / 100);
				uint16_t hpdamage = (uint16_t)(damage - mpdamage);

				if (mpdamage < mp || player->getActiveBuffs()->hasInfinity()) {
					player->getStats()->damageMp(mpdamage);
					player->getStats()->damageHp(hpdamage);
				}
				else if (mpdamage >= mp) {
					player->getStats()->setMp(0);
					player->getStats()->damageHp(hpdamage + (mpdamage - mp));
				}
			}
			applieddamage = true;
		}

		if (player->getSkills()->hasAchilles()) {
			int32_t sid = player->getSkills()->getAchilles();
			double red = (2.0 - player->getSkills()->getSkillInfo(sid)->x / 1000.0);

			player->getStats()->damageHp((uint16_t) (damage / red));

			if (deadlyattack && player->getStats()->getMp() > 0) {
				player->getStats()->setMp(1);
			}
			if (mpburn > 0) {
				player->getStats()->damageMp(mpburn);
			}

			applieddamage = true;
		}

		if (!applieddamage) {
			if (deadlyattack) {
				if (player->getStats()->getMp() > 0) {
					player->getStats()->setMp(1);
				}
				player->getStats()->setHp(1);
			}
			else {
				player->getStats()->damageHp((uint16_t) damage);
			}

			if (mpburn > 0) {
				player->getStats()->damageMp(mpburn);
			}
			if (player->getActiveBuffs()->getActiveSkillLevel(Jobs::Corsair::Battleship) > 0) {
				player->getActiveBuffs()->reduceBattleshipHp((uint16_t) damage);
			}
		}
		int32_t morph = player->getActiveBuffs()->getCurrentMorph();
		if (morph < 0  || (morph != 0 && player->getStats()->getHp() == 0)) {
			player->getActiveBuffs()->endMorph();
		}
	}
	PlayersPacket::damagePlayer(player, damage, mobid, hit, type, stance, nodamageid, pgmr);
}

void PlayerHandler::handleFacialExpression(Player *player, PacketReader &packet) {
	int32_t face = packet.get<int32_t>();
	PlayersPacket::faceExpression(player, face);
}

void PlayerHandler::handleGetInfo(Player *player, PacketReader &packet) {
	if (!player->updateTickCount(packet.get<int32_t>())) {
		// Tickcount was the same or less than 100 of the difference.
		return;
	}
	if (Player *info = PlayerDataProvider::Instance()->getPlayer(packet.get<int32_t>())) {
		PlayersPacket::showInfo(player, info, packet.getBool());
	}
}

void PlayerHandler::handleHeal(Player *player, PacketReader &packet) {
	packet.skipBytes(4);

	int16_t hp = packet.get<int16_t>();
	int16_t mp = packet.get<int16_t>();
	if (player->getStats()->getHp() == 0 || hp > 400 || mp > 1000 || (hp > 0 && mp > 0)) {
		// Hacking
		return;
	}
	player->getStats()->modifyHp(hp);
	player->getStats()->modifyMp(mp);
}

void PlayerHandler::handleMoving(Player *player, PacketReader &packet) {
	if (packet.get<int8_t>() != player->getPortalCount(false)) {
		// Portal count didn't match. Ignore.
		return;
	}
	packet.reset(11);
	MovementHandler::parseMovement(player, packet);
	packet.reset(11);
	PlayersPacket::showMoving(player, packet.getBuffer(), packet.getBufferLength());

	if (player->getFh() == 0) {
		// Player is floating in the air
		int32_t mapid = player->getMap();
		Pos playerpos = player->getPos();
		Map *map = Maps::getMap(mapid);

		Pos floor = map->findFloor(playerpos);
		if (floor.y == playerpos.y) {
			// There are no footholds below the player
			int8_t count = player->getFallCounter();
			if (count > 3) {
				player->setMap(mapid);
			}
			else {
				player->setFallCounter(++count);
			}
		}
	}
	else if (player->getFallCounter() > 0) {
		player->setFallCounter(0);
	}
}

void PlayerHandler::handleSpecialSkills(Player *player, PacketReader &packet) {
	int32_t skillid = packet.get<int32_t>();
	switch (skillid) {
		case Jobs::Hero::MonsterMagnet:
		case Jobs::Paladin::MonsterMagnet:
		case Jobs::DarkKnight::MonsterMagnet:
		case Jobs::Marksman::PiercingArrow:
		case Jobs::FPArchMage::BigBang:
		case Jobs::ILArchMage::BigBang:
		case Jobs::Bishop::BigBang: {
			SpecialSkillInfo info;
			info.skillId = skillid;
			info.level = packet.get<uint8_t>();
			info.direction = packet.get<uint8_t>();
			info.weaponSpeed = packet.get<uint8_t>();
			player->setSpecialSkill(info);
			SkillsPacket::showSpecialSkill(player, info);
			break;
		}
		case Jobs::ChiefBandit::Chakra: {
			int16_t dex = player->getStats()->getDex(true);
			int16_t luk = player->getStats()->getLuk(true);
			int16_t recovery = player->getSkills()->getSkillInfo(skillid)->y;
			int16_t maximum = (luk * 66 / 10 + dex) * 2 / 10 * (recovery / 100 + 1);
			int16_t minimum = (luk * 33 / 10 + dex) * 2 / 10 * (recovery / 100 + 1);
			// Maximum = (luk * 6.6 + dex) * 0.2 * (recovery% / 100 + 1)
			// Minimum = (luk * 3.3 + dex) * 0.2 * (recovery% / 100 + 1)
			// I used 66 / 10 and 2 / 10 respectively to get 6.6 and 0.2 without using floating points
			player->getStats()->modifyHp(Randomizer::Instance()->randShort(maximum, minimum));
			break;
		}
	}
}

void PlayerHandler::handleMonsterBook(Player *player, PacketReader &packet) {
	int32_t cardid = packet.get<int32_t>();
	if (cardid != 0 && player->getMonsterBook()->getCard(cardid) == nullptr) {
		// Hacking
		return;
	}
	else if (cardid != 0) {
		int32_t mobid = ItemDataProvider::Instance()->getMobId(cardid);
		if (mobid != 0) {
			player->getMonsterBook()->setCover(mobid);
			MonsterBookPacket::changeCover(player, cardid);
		}
	}
	else {
		player->getMonsterBook()->setCover(0);
		MonsterBookPacket::changeCover(player, 0);
	}
}

void PlayerHandler::handleAdminMessenger(Player *player, PacketReader &packet) {
	if (!player->isAdmin()) {
		// Hacking
		player->addWarning();
		return;
	}
	Player *receiver = nullptr;
	bool hasTarget = packet.get<int8_t>() == 2;
	int8_t sort = packet.get<int8_t>();
	bool useWhisper = packet.getBool();
	int8_t type = packet.get<int8_t>();
	int32_t characterId = packet.get<int32_t>();

	if (player->getId() != characterId) {
		return;
	}

	string line1 = packet.getString();
	string line2 = packet.getString();
	string line3 = packet.getString();
	string line4 = packet.getString();
	string line5 = packet.getString();
	if (hasTarget) {
		receiver = PlayerDataProvider::Instance()->getPlayer(packet.getString());
	}

	int32_t time = 15;
	switch (type) {
		case 1: time = 30; break;
		case 2: time = 60; break;
	}

	MapleTvs::Instance()->addMessage(player, receiver, line1, line2, line3, line4, line5, 5075000 + type, time);
	if (sort == 1) {
		InventoryPacket::showSuperMegaphone(player, player->getMedalName() + " : " + line1 + line2 + line3 + line4 + line5, useWhisper);
	}
}

void PlayerHandler::useMeleeAttack(Player *player, PacketReader &packet) {
	Attack attack = compileAttack(player, packet, SkillTypes::Melee);
	if (!player->updateTickCount(attack.ticks) || attack.portals != player->getPortalCount(false)) {
		// Tickcount was the same or less than 100 of the difference.
		return;
	}
	PlayersPacket::useMeleeAttack(player, attack);
	int8_t damagedtargets = 0;
	int32_t skillid = attack.skillId;
	uint8_t level = attack.skillLevel;

	if (skillid != Jobs::All::RegularAttack) {
		Skills::useAttackSkill(player, skillid);
	}

	int32_t map = player->getMap();
	uint8_t pplevel = player->getActiveBuffs()->getActiveSkillLevel(Jobs::ChiefBandit::Pickpocket); // Check for active pickpocket level
	bool ppok = !attack.isMesoExplosion && pplevel > 0;
	SkillLevelInfo *picking = SkillDataProvider::Instance()->getSkill(Jobs::ChiefBandit::Pickpocket, pplevel);
	Pos origin;
	vector<int32_t> ppdamages;

	for (Attack::iterator i = attack.damages.begin(); i != attack.damages.end(); ++i) {
		int32_t targettotal = 0;
		int8_t connectedhits = 0;
		Mob *mob = Maps::getMap(map)->getMob(i->first);
		if (mob == nullptr) {
			continue;
		}
		origin = mob->getPos(); // Info for pickpocket before mob is set to 0 (in the case that mob dies)
		for (Attack::diterator k = i->second.begin(); k != i->second.end(); ++k) {
			int32_t damage = *k;
			if (damage != 0) {
				connectedhits++;
				targettotal += damage;
			}
			if (ppok && Randomizer::Instance()->randInt(99) < picking->prop) {
				 // Make sure this is a melee attack and not meso explosion, plus pickpocket being active
				ppdamages.push_back(damage);
			}
			if (mob == nullptr) {
				if (ppok) {
					// Roll along after the mob is dead to finish getting damage values for pickpocket
					continue;
				}
				break;
			}
			if (skillid == Jobs::Paladin::HeavensHammer) {
				damage = (mob->isBoss() ? Stats::MaxDamage : (mob->getHp() - 1)); // If a Paladin wants to prove that it does something else, feel free
			}
			else if (skillid == Jobs::Bandit::Steal && !mob->isBoss()) {
				DropHandler::doDrops(player->getId(), map, mob->getInfo()->level, mob->getMobId(), mob->getPos(), false, false, mob->getTauntEffect(), true);
			}
			int32_t temphp = mob->getHp();
			mob->applyDamage(player->getId(), damage);
			if (temphp <= damage) {
				// Mob was killed, so set the Mob pointer to 0
				mob = nullptr;
			}
		}
		if (targettotal > 0) {
			if (mob != nullptr && mob->getHp() > 0) {
				MobHandler::handleMobStatus(player, mob, skillid, level, player->getInventory()->getEquippedId(EquipSlots::Weapon), connectedhits); // Mob status handler (freeze, stun, etc)
				if (mob->getHp() < mob->getSelfDestructHp()) {
					mob->explode();
				}
			}
			damagedtargets++;
		}
		uint8_t ppsize = ppdamages.size();
		for (uint8_t pickpocket = 0; pickpocket < ppsize; pickpocket++) { // Drop stuff for Pickpocket
			Pos pppos = origin;
			pppos.x += (ppsize % 2 == 0 ? 5 : 0) + (ppsize / 2) - 20 * ((ppsize / 2) - pickpocket);

			clock_t pptime = 175 * pickpocket;
			int32_t ppmesos = ((ppdamages[pickpocket] * picking->x) / 10000); // TODO: Check on this formula in different situations
			Drop *ppdrop = new Drop(player->getMap(), ppmesos, pppos, player->getId(), true);
			ppdrop->setTime(100);
			new Timer::Timer(bind(&Drop::doDrop, ppdrop, origin),
				Timer::Id(Timer::Types::PickpocketTimer, player->getId(), player->getActiveBuffs()->getPickpocketCounter()),
				0, Timer::Time::fromNow(pptime));
		}
		ppdamages.clear();
	}

	if (player->getSkills()->hasEnergyCharge()) {
		player->getActiveBuffs()->increaseEnergyChargeLevel(damagedtargets);
	}

	switch (skillid) {
		case Jobs::ChiefBandit::MesoExplosion: {
			uint8_t items = packet.get<int8_t>();
			int32_t map = player->getMap();
			for (uint8_t i = 0; i < items; i++) {
				int32_t objId = packet.get<int32_t>();
				packet.skipBytes(1); // Some value
				if (Drop *drop = Maps::getMap(map)->getDrop(objId)) {
					if (!drop->isMesos()) {
						// Hacking
						player->addWarning();
						return;
					}
					DropsPacket::explodeDrop(drop);
					Maps::getMap(map)->removeDrop(drop->getId());
					delete drop;
				}
			}
			break;
		}
		case Jobs::Marauder::EnergyDrain:
		case Jobs::ThunderBreaker::EnergyDrain: {
			int32_t hpRecover = static_cast<int32_t>(attack.totalDamage * player->getSkills()->getSkillInfo(skillid)->x / 100);
			if (hpRecover > player->getStats()->getMaxHp())
				player->getStats()->setHp(player->getStats()->getMaxHp());
			else
				player->getStats()->modifyHp((int16_t) hpRecover);
			break;
		}
		case Jobs::Crusader::SwordPanic: // Crusader finishers
		case Jobs::Crusader::SwordComa:
		case Jobs::Crusader::AxePanic:
		case Jobs::Crusader::AxeComa:
		case Jobs::DawnWarrior::Panic:
		case Jobs::DawnWarrior::Coma:
			player->getActiveBuffs()->setCombo(0, true);
			break;
		case Jobs::Crusader::Shout:
		case Jobs::Gm::SuperDragonRoar:
		case Jobs::SuperGm::SuperDragonRoar:
			break;
		case Jobs::DragonKnight::DragonRoar: {
			int16_t x_value = SkillDataProvider::Instance()->getSkill(skillid, level)->x;
			uint16_t reduction = (player->getStats()->getMaxHp() / 100) * x_value;
			if (reduction < player->getStats()->getHp()) {
				player->getStats()->damageHp(reduction);
			}
			else {
				// Hacking
				return;
			}
			Buffs::addBuff(player, Jobs::DragonKnight::DragonRoar, level, 0);
			break;
		}
		case Jobs::DragonKnight::Sacrifice: {
			int16_t hp_damage_x = player->getSkills()->getSkillInfo(skillid)->x;
			uint16_t hp_damage = static_cast<uint16_t>(attack.totalDamage * hp_damage_x / 100);
			player->getStats()->setHp(hp_damage > player->getStats()->getHp() ? 1 : hp_damage);
			break;
		}
		case Jobs::WhiteKnight::ChargeBlow: {
			int8_t acb_level = player->getSkills()->getSkillLevel(Jobs::Paladin::AdvancedCharge);
			int16_t acb_x = 0;
			if (acb_level > 0) {
				acb_x = SkillDataProvider::Instance()->getSkill(Jobs::Paladin::AdvancedCharge, acb_level)->x;
			}
			if ((acb_x != 100) && (acb_x == 0 || Randomizer::Instance()->randShort(99) > (acb_x - 1))) {
				player->getActiveBuffs()->stopCharge();
			}
			break;
		}
		default:
			if (attack.totalDamage > 0) {
				player->getActiveBuffs()->addCombo();
			}
	}
}

void PlayerHandler::useRangedAttack(Player *player, PacketReader &packet) {
	Attack attack = compileAttack(player, packet, SkillTypes::Ranged);
	if (!player->updateTickCount(attack.ticks) || attack.portals != player->getPortalCount(false)) {
		// Tickcount was the same or less than 100 of the difference.
		return;
	}
	PlayersPacket::useRangedAttack(player, attack);
	int32_t skillid = attack.skillId;
	uint8_t level = attack.skillLevel;

	switch (skillid) {
		case Jobs::Bowmaster::Hurricane:
		case Jobs::WindArcher::Hurricane:
		case Jobs::Corsair::RapidFire:
			if (player->getSpecialSkill() == 0) {
				SpecialSkillInfo info;
				info.skillId = skillid;
				info.direction = attack.animation;
				info.weaponSpeed = attack.weaponSpeed;
				info.level = level;
				player->setSpecialSkill(info);
				SkillsPacket::showSpecialSkill(player, info);
			}
			break;
	}

	Skills::useAttackSkillRanged(player, skillid, attack.starPos);

	int32_t mhp = 0;
	int32_t firsthit = 0;

	for (Attack::iterator i = attack.damages.begin(); i != attack.damages.end(); ++i) {
		int32_t mapmobid = i->first;
		Mob *mob = Maps::getMap(player->getMap())->getMob(mapmobid);
		if (mob == nullptr) {
			continue;
		}
		int32_t targettotal = 0;
		int8_t connectedhits = 0;

		for (Attack::diterator k = i->second.begin(); k != i->second.end(); ++k) {
			int32_t damage = *k;
			if (damage != 0) {
				connectedhits++;
				targettotal += damage;
			}
			if (firsthit == 0) {
				firsthit = damage;
			}
			if (mob == nullptr) {
				continue;
			}
			mhp = mob->getMaxHp();
			if (skillid == Jobs::Ranger::MortalBlow || skillid == Jobs::Sniper::MortalBlow) {
				SkillLevelInfo *sk = player->getSkills()->getSkillInfo(skillid);
				int32_t hp_p = mhp * sk->x / 100; // Percentage of HP required for Mortal Blow activation
				if ((mob->getHp() < hp_p) && (Randomizer::Instance()->randShort(99) < sk->y)) {
					damage = mob->getHp();
				}
			}
			else if (skillid == Jobs::Outlaw::HomingBeacon || skillid == Jobs::Corsair::Bullseye) {
				Buffs::addBuff(player, skillid, level, 0, mapmobid);
			}
			int32_t temphp = mob->getHp();
			mob->applyDamage(player->getId(), damage);
			if (temphp <= damage) {
				mob = nullptr;
			}
		}
		if (mob != nullptr && targettotal > 0 && mob->getHp() > 0) {
			MobHandler::handleMobStatus(player, mob, skillid, level, player->getInventory()->getEquippedId(EquipSlots::Weapon), connectedhits, firsthit); // Mob status handler (freeze, stun, etc)
			if (mob->getHp() < mob->getSelfDestructHp()) {
				mob->explode();
			}
		}
	}

	switch (skillid) {
		case Jobs::Assassin::Drain: { // Drain
			int16_t drain_x = player->getSkills()->getSkillInfo(skillid)->x;
			int32_t hpRecover = static_cast<int32_t>(attack.totalDamage * drain_x / 100);
			int16_t playerMaxHp = player->getStats()->getMaxHp();
			if (hpRecover > mhp) {
				hpRecover = mhp;
			}
			if (hpRecover > (playerMaxHp / 2)) {
				hpRecover = playerMaxHp / 2;
			}
			if (hpRecover > playerMaxHp) {
				player->getStats()->setHp(playerMaxHp);
			}
			else {
				player->getStats()->modifyHp(static_cast<int16_t>(hpRecover));
			}
			break;
		}
		case Jobs::DawnWarrior::SoulBlade:
			if (attack.totalDamage > 0) {
				player->getActiveBuffs()->addCombo();
			}
			break;
	}
}

void PlayerHandler::usePirateGrenade(Player *player, PacketReader &packet) {
	if (player->getSkills()->getSkillLevel(Jobs::Gunslinger::Grenade) == 0) {
		// Player is not a gunslinger... LOL
		return;
	}

	int32_t posx = packet.get<int32_t>();
	int32_t posy = packet.get<int32_t>();
	int32_t charge = packet.get<int32_t>();
	int32_t unk = packet.get<int32_t>(); // Seems to be the same everytime (tested mapchange, pos change and skill lvl change)

	if (charge <= 0 || charge > 1000) {
		// !!! INVALID STUFF !!!
		return;
	}

	PlayersPacket::usePirateGrenade(player, posx, posy, charge, unk);
}

void PlayerHandler::useSpellAttack(Player *player, PacketReader &packet) {
	Attack attack = compileAttack(player, packet, SkillTypes::Magic);
	if (!player->updateTickCount(attack.ticks) || attack.portals != player->getPortalCount(false)) {
		// Tickcount was the same or less than 100 of the difference.
		return;
	}
	PlayersPacket::useSpellAttack(player, attack);

	int32_t skillid = attack.skillId;
	uint8_t level = attack.skillLevel;

	MpEaterInfo eater;
	eater.skillId = player->getSkills()->getMpEater();
	eater.level = player->getSkills()->getSkillLevel(eater.skillId);
	if (eater.level > 0) {
		SkillLevelInfo *eaaat = SkillDataProvider::Instance()->getSkill(eater.skillId, eater.level);
		eater.prop = eaaat->prop;
		eater.x = eaaat->x;
	}

	if (!attack.isHeal) {
		// Heal is sent as both an attack and as a used skill - it's only sometimes an attack
		Skills::useAttackSkill(player, skillid);
	}

	for (Attack::iterator i = attack.damages.begin(); i != attack.damages.end(); ++i) {
		int32_t targettotal = 0;
		int32_t mapmobid = i->first;
		int8_t connectedhits = 0;
		Mob *mob = Maps::getMap(player->getMap())->getMob(mapmobid);
		if (mob == nullptr) {
			continue;
		}
		if (attack.isHeal && !mob->isUndead()) {
			// Hacking
			return;
		}

		for (Attack::diterator k = i->second.begin(); k != i->second.end(); ++k) {
			int32_t damage = *k;
			if (damage != 0) {
				connectedhits++;
				targettotal += damage;
				if (eater.level != 0 && !eater.used) { // MP Eater
					mob->mpEat(player, &eater);
				}
			}
			int32_t temphp = mob->getHp();
			mob->applyDamage(player->getId(), damage);
			if (temphp <= damage) {
				// Mob was killed, so set the Mob pointer to 0
				mob = nullptr;
				break;
			}
		}
		if (mob != nullptr && targettotal > 0 && mob->getHp() > 0) {
			MobHandler::handleMobStatus(player, mob, skillid, level, player->getInventory()->getEquippedId(EquipSlots::Weapon), connectedhits); // Mob status handler (freeze, stun, etc)
			if (mob->getHp() < mob->getSelfDestructHp()) {
				mob->explode();
			}
		}
	}

	switch (skillid) {
		case Jobs::FPMage::PoisonMist:
		case Jobs::BlazeWizard::FlameGear: {
			Mist *mist = new Mist(player->getMap(), player, player->getPos(), SkillDataProvider::Instance()->getSkill(skillid, level), skillid, level, true);
			break;
		}
	}
}

void PlayerHandler::useEnergyChargeAttack(Player *player, PacketReader &packet) {
	Attack attack = compileAttack(player, packet, SkillTypes::EnergyCharge);
	PlayersPacket::useEnergyChargeAttack(player, attack);

	int32_t skillid = attack.skillId;
	int8_t level = attack.skillLevel;

	for (Attack::iterator i = attack.damages.begin(); i != attack.damages.end(); ++i) {
		int32_t targettotal = 0;
		int32_t mapmobid = i->first;
		int8_t connectedhits = 0;
		Mob *mob = Maps::getMap(player->getMap())->getMob(mapmobid);
		if (mob == nullptr) {
			continue;
		}

		for (Attack::diterator k = i->second.begin(); k != i->second.end(); ++k) {
			int32_t damage = *k;
			if (damage != 0) {
				connectedhits++;
				targettotal += damage;
			}
			int32_t temphp = mob->getHp();
			mob->applyDamage(player->getId(), damage);
			if (temphp <= damage) {
				// Mob was killed, so set the Mob pointer to 0
				mob = nullptr;
				break;
			}
		}
		if (mob != nullptr && targettotal > 0 && mob->getHp() > 0) {
			MobHandler::handleMobStatus(player, mob, skillid, level, player->getInventory()->getEquippedId(EquipSlots::Weapon), connectedhits); // Mob status handler (freeze, stun, etc)
			if (mob->getHp() < mob->getSelfDestructHp()) {
				mob->explode();
			}
		}
	}
}

void PlayerHandler::useSummonAttack(Player *player, PacketReader &packet) {
	Attack attack = compileAttack(player, packet, SkillTypes::Summon);
	if (!player->updateTickCount(attack.ticks)) {
		// Tickcount was the same or less than 100 of the difference.
		return;
	}
	Summon *summon = player->getSummons()->getSummon();
	if (summon == nullptr) {
		// Hacking or some other form of tomfoolery
		return;
	}
	PlayersPacket::useSummonAttack(player, attack);
	int32_t skillid = summon->getSummonId();
	for (Attack::iterator i = attack.damages.begin(); i != attack.damages.end(); ++i) {
		int32_t targettotal = 0;
		int32_t mapmobid = i->first;
		int8_t connectedhits = 0;
		Mob *mob = Maps::getMap(player->getMap())->getMob(mapmobid);
		if (mob == nullptr) {
			continue;
		}
		for (Attack::diterator k = i->second.begin(); k != i->second.end(); ++k) {
			int32_t damage = *k;
			if (damage != 0) {
				connectedhits++;
				targettotal += damage;
			}
			int32_t temphp = mob->getHp();
			mob->applyDamage(player->getId(), damage);
			if (temphp <= damage) {
				// Mob was killed, so set the Mob pointer to 0
				mob = nullptr;
				break;
			}
		}
		if (mob != nullptr && targettotal > 0 && mob->getHp() > 0) {
			MobHandler::handleMobStatus(player, mob, skillid, player->getSkills()->getSkillLevel(skillid), player->getInventory()->getEquippedId(EquipSlots::Weapon), connectedhits); // Mob status handler (freeze, stun, etc)
			if (mob->getHp() < mob->getSelfDestructHp()) {
				mob->explode();
			}
		}
	}
}

Attack PlayerHandler::compileAttack(Player *player, PacketReader &packet, int8_t skillType) {
	Attack attack;
	int8_t targets = 0;
	int8_t hits = 0;
	int32_t skillid = 0;
	bool mesoExplosion = false;
	bool shadowMeso = false;

	if (skillType != SkillTypes::Summon) {
		attack.portals = packet.get<uint8_t>();
		uint8_t tbyte = packet.get<uint8_t>();
		skillid = packet.get<int32_t>();
		targets = tbyte / 0x10;
		hits = tbyte % 0x10;

		if (skillid != Jobs::All::RegularAttack) {
			attack.skillLevel = player->getSkills()->getSkillLevel(skillid);
		}

		packet.skipBytes(4); // Unk, strange constant that doesn't seem to change
		// Things atttemped: Map changes, character changes, job changes, skill changes, position changes, hitting enemies
		// It appears as 0xF9B16E60 which is 4189154912 unsigned, -105812384 signed, doesn't seem to be a size, probably a CRC
		packet.skipBytes(4); // Unk, strange constant dependent on skill, probably a CRC

		switch (skillid) {
			case Jobs::Hermit::ShadowMeso:
				attack.isShadowMeso = true;
				shadowMeso = true;
				break;
			case Jobs::ChiefBandit::MesoExplosion:
				attack.isMesoExplosion = true;
				mesoExplosion = true;
				break;
			case Jobs::Cleric::Heal:
				attack.isHeal = true;
				break;
			case Jobs::Gunslinger::Grenade:
			case Jobs::Brawler::CorkscrewBlow:
			case Jobs::Bowmaster::Hurricane:
			case Jobs::WindArcher::Hurricane:
			case Jobs::Marksman::PiercingArrow:
			case Jobs::NightWalker::PoisonBomb:
			case Jobs::Corsair::RapidFire:
			case Jobs::FPArchMage::BigBang:
			case Jobs::ILArchMage::BigBang:
			case Jobs::Bishop::BigBang:
				attack.isChargeSkill = true;
				attack.charge = packet.get<int32_t>();
				break;
		}

		attack.display = packet.get<uint8_t>();
		attack.animation = packet.get<uint8_t>();
		attack.weaponClass = packet.get<uint8_t>();
		attack.weaponSpeed = packet.get<uint8_t>();
		attack.ticks = packet.get<int32_t>();
	}
	else {
		attack.summonId = packet.get<int32_t>(); // Summon ID, not to be confused with summon skill ID
		attack.ticks = packet.get<int32_t>();
		attack.animation = packet.get<uint8_t>();
		targets = packet.get<int8_t>();
		hits = 1;
	}

	if (skillType == SkillTypes::Ranged) {
		int16_t starSlot = packet.get<int16_t>();
		int16_t csStar = packet.get<int16_t>();
		attack.starPos = starSlot;
		attack.cashStarPos = csStar;
		packet.skipBytes(1); // 0x00 = AoE?
		if (!shadowMeso) {
			if (player->getActiveBuffs()->hasShadowStars() && skillid != Jobs::NightLord::Taunt) {
				attack.starId = packet.get<int32_t>();
			}
			else if (csStar > 0) {
				if (Item *item = player->getInventory()->getItem(Inventories::CashInventory, csStar)) {
					attack.starId = item->getId();
				}
			}
			else if (starSlot > 0) {
				if (Item *item = player->getInventory()->getItem(Inventories::UseInventory, starSlot)) {
					attack.starId = item->getId();
				}
			}
		}
	}

	attack.targets = targets;
	attack.hits = hits;
	attack.skillId = skillid;

	for (int8_t i = 0; i < targets; i++) {
		int32_t mapmobid = packet.get<int32_t>();
		packet.skipBytes(4); // Always 0x06, <two bytes of some kind>, 0x01
		packet.skipBytes(8); // Mob pos, damage pos
		if (!mesoExplosion) {
			packet.skipBytes(2); // Distance
		}
		else {
			hits = packet.get<int8_t>(); // Hits for Meso Explosion
		}
		for (int8_t k = 0; k < hits; k++) {
			int32_t damage = packet.get<int32_t>();
			attack.damages[mapmobid].push_back(damage);
			attack.totalDamage += damage;
		}
		if (skillType != SkillTypes::Summon) {
			packet.skipBytes(4); // 4 bytes of unknown purpose, differs by the mob, probably a CRC
		}
	}

	if (skillType == SkillTypes::Ranged) {
		attack.projectilePos = packet.getPos();
	}
	attack.playerPos = packet.getPos();

	return attack;
}