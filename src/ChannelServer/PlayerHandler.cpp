/*
Copyright (C) 2008-2014 Vana Development Team

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
#include "PlayerHandler.hpp"
#include "Drop.hpp"
#include "DropHandler.hpp"
#include "DropsPacket.hpp"
#include "GameLogicUtilities.hpp"
#include "InventoryPacket.hpp"
#include "ItemDataProvider.hpp"
#include "MapleTvs.hpp"
#include "Maps.hpp"
#include "Mist.hpp"
#include "MobHandler.hpp"
#include "MonsterBookPacket.hpp"
#include "MovementHandler.hpp"
#include "Player.hpp"
#include "PlayerDataProvider.hpp"
#include "PlayerPacket.hpp"
#include "PlayersPacket.hpp"
#include "Randomizer.hpp"
#include "PacketReader.hpp"
#include "SkillConstants.hpp"
#include "SkillDataProvider.hpp"
#include "Skills.hpp"
#include "SkillsPacket.hpp"
#include "Summon.hpp"
#include "TimeUtilities.hpp"
#include "Timer.hpp"
#include <functional>

auto PlayerHandler::handleDoorUse(Player *player, PacketReader &packet) -> void {
	int32_t doorId = packet.get<int32_t>();
	bool toTown = !packet.get<bool>();
	//Player *doorHolder = PlayerDataProvider::getInstance().getPlayer(doorId);
	//if (doorHolder == nullptr || (doorHolder->getParty() != player->getParty() && doorHolder != player)) {
	//	// Hacking or lag
	//	return;
	//}
	//doorHolder->getDoor()->warp(player, toTown);
}

auto PlayerHandler::handleDamage(Player *player, PacketReader &packet) -> void {
	const int8_t BumpDamage = -1;
	const int8_t MapDamage = -2;

	uint32_t ticks = packet.get<uint32_t>();
	int8_t type = packet.get<int8_t>();
	packet.skipBytes(1); // Element - 0x00 = elementless, 0x01 = ice, 0x02 = fire, 0x03 = lightning
	int32_t damage = packet.get<int32_t>();
	bool damageApplied = false;
	bool deadlyAttack = false;
	uint8_t hit = 0;
	uint8_t stance = 0;
	uint8_t disease = 0;
	uint8_t level = 0;
	uint16_t mpBurn = 0;
	int32_t mapMobId = 0;
	int32_t mobId = 0;
	int32_t noDamageId = 0;
	ReturnDamageInfo pgmr;

	if (type != MapDamage) {
		mobId = packet.get<int32_t>();
		mapMobId = packet.get<int32_t>();
		auto mob = player->getMap()->getMob(mapMobId);
		if (mob != nullptr && mob->getMobId() != mobId) {
			// Hacking
			return;
		}
		if (type != BumpDamage) {
			auto attack = MobDataProvider::getInstance().getMobAttack(mob->getMobIdOrLink(), type);
			if (attack == nullptr) {
				// Hacking
				return;
			}
			disease = attack->disease;
			level = attack->level;
			mpBurn = attack->mpBurn;
			deadlyAttack = attack->deadlyAttack;
		}

		hit = packet.get<uint8_t>(); // Knock direction
		pgmr.reduction = packet.get<uint8_t>();
		packet.skipBytes(1); // I think reduction is a short, but it's a byte in the S -> C packet, so..
		if (pgmr.reduction != 0) {
			pgmr.isPhysical = packet.get<bool>();
			pgmr.mapMobId = packet.get<int32_t>();
			if (pgmr.mapMobId != mapMobId) {
				// Hacking
				return;
			}
			packet.skipBytes(1); // 0x06 for Power Guard, 0x00 for Mana Reflection?
			packet.skipBytes(4); // Mob position garbage
			pgmr.pos = packet.getClass<Pos>();
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
		noDamageId = player->getSkills()->getNoDamageSkill();
	}

	if (disease > 0 && damage != 0) {
		// Fake/Guardian don't prevent disease
		player->getActiveBuffs()->addDebuff(disease, level);
	}

	int16_t mp = player->getStats()->getMp();
	int16_t hp = player->getStats()->getHp();

	auto deadlyAttackFunc = [&player, &mp](bool setHp) {
		if (mp > 0) {
			player->getStats()->setMp(1);
		}
		if (setHp) {
			player->getStats()->setHp(1);
		}
	};

	if (damage > 0 && !player->hasGmBenefits()) {
		if (player->getActiveBuffs()->hasMesoGuard() && player->getInventory()->getMesos() > 0) {
			int32_t skillId = player->getActiveBuffs()->getMesoGuard();
			int16_t mesoRate = player->getActiveBuffs()->getActiveSkillInfo(skillId)->x; // Meso Guard meso %
			int16_t mesoLoss = static_cast<int16_t>(mesoRate * damage / 2 / 100);
			int32_t mesos = player->getInventory()->getMesos();
			int32_t newMesos = mesos - mesoLoss;

			if (newMesos < 0) {
				// Special damage calculation for not having enough mesos
				double reduction = 2.0 - static_cast<double>(mesos / mesoLoss) / 2.0;
				damage = static_cast<uint16_t>(damage / reduction);
				// This puts us pretty close to the damage observed clientside, needs improvement
				// TODO: Improve formula
			}
			else {
				damage /= 2;
				// Usually displays 1 below the actual damage but is sometimes accurate - no clue why
			}

			player->getInventory()->setMesos(newMesos);
			player->getStats()->damageHp(static_cast<uint16_t>(damage));

			if (deadlyAttack) {
				deadlyAttackFunc(false);
			}
			else if (mpBurn > 0) {
				player->getStats()->damageMp(mpBurn);
			}
			damageApplied = true;

			SkillsPacket::showSkillEffect(player, skillId);
		}

		if (player->getActiveBuffs()->hasMagicGuard()) {
			if (deadlyAttack) {
				deadlyAttackFunc(true);
			}
			else if (mpBurn > 0) {
				player->getStats()->damageMp(mpBurn);
				player->getStats()->damageHp(static_cast<uint16_t>(damage));
			}
			else {
				int32_t skillId = player->getActiveBuffs()->getMagicGuard();
				int16_t reduc = player->getActiveBuffs()->getActiveSkillInfo(skillId)->x;
				uint16_t mpDamage = static_cast<uint16_t>((damage * reduc) / 100);
				uint16_t hpDamage = static_cast<uint16_t>(damage - mpDamage);

				if (mpDamage < mp || player->getActiveBuffs()->hasInfinity()) {
					player->getStats()->damageMp(mpDamage);
					player->getStats()->damageHp(hpDamage);
				}
				else if (mpDamage >= mp) {
					player->getStats()->setMp(0);
					player->getStats()->damageHp(hpDamage + (mpDamage - mp));
				}
			}
			damageApplied = true;
		}

		if (player->getSkills()->hasAchilles()) {
			int32_t skillId = player->getSkills()->getAchilles();
			double red = (2.0 - player->getSkills()->getSkillInfo(skillId)->x / 1000.0);

			player->getStats()->damageHp(static_cast<uint16_t>(damage / red));

			if (deadlyAttack) {
				deadlyAttackFunc(false);
			}
			else if (mpBurn > 0) {
				player->getStats()->damageMp(mpBurn);
			}

			damageApplied = true;
		}

		if (!damageApplied) {
			if (deadlyAttack) {
				deadlyAttackFunc(true);
			}
			else {
				player->getStats()->damageHp(static_cast<uint16_t>(damage));
			}

			if (mpBurn > 0) {
				player->getStats()->damageMp(mpBurn);
			}

			if (player->getActiveBuffs()->getActiveSkillLevel(Skills::Corsair::Battleship) > 0) {
				player->getActiveBuffs()->reduceBattleshipHp(static_cast<uint16_t>(damage));
			}
		}
		int32_t morph = player->getActiveBuffs()->getCurrentMorph();
		if (morph < 0 || (morph != 0 && player->getStats()->isDead())) {
			player->getActiveBuffs()->endMorph();
		}
	}
	PlayersPacket::damagePlayer(player, damage, mobId, hit, type, stance, noDamageId, pgmr);
}

auto PlayerHandler::handleFacialExpression(Player *player, PacketReader &packet) -> void {
	int32_t face = packet.get<int32_t>();
	PlayersPacket::faceExpression(player, face);
}

auto PlayerHandler::handleGetInfo(Player *player, PacketReader &packet) -> void {
	uint32_t ticks = packet.get<uint32_t>();
	if (Player *info = PlayerDataProvider::getInstance().getPlayer(packet.get<int32_t>())) {
		PlayersPacket::showInfo(player, info, packet.get<bool>());
	}
}

auto PlayerHandler::handleHeal(Player *player, PacketReader &packet) -> void {
	uint32_t ticks = packet.get<uint32_t>();
	int16_t hp = packet.get<int16_t>();
	int16_t mp = packet.get<int16_t>();
	if (player->getStats()->isDead() || hp > 400 || mp > 1000 || (hp > 0 && mp > 0)) {
		// Hacking
		return;
	}
	player->getStats()->modifyHp(hp);
	player->getStats()->modifyMp(mp);
}

auto PlayerHandler::handleMoving(Player *player, PacketReader &packet) -> void {
	if (packet.get<uint8_t>() != player->getPortalCount()) {
		// Portal count doesn't match, usually an indication of hacking
		return;
	}
	packet.reset(11);
	MovementHandler::parseMovement(player, packet);
	packet.reset(11);
	PlayersPacket::showMoving(player, packet.getBuffer(), packet.getBufferLength());

	if (player->getFoothold() == 0) {
		// Player is floating in the air
		int32_t mapId = player->getMapId();
		const Pos &playerPos = player->getPos();
		Map *map = Maps::getMap(mapId);

		Pos floor;
		if (map->findFloor(playerPos, floor) == SearchResult::NotFound) {
			// There are no footholds below the player
			int8_t count = player->getFallCounter();
			if (count > 3) {
				player->setMap(mapId);
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

auto PlayerHandler::handleSpecialSkills(Player *player, PacketReader &packet) -> void {
	int32_t skillId = packet.get<int32_t>();
	switch (skillId) {
		case Skills::Hero::MonsterMagnet:
		case Skills::Paladin::MonsterMagnet:
		case Skills::DarkKnight::MonsterMagnet:
		case Skills::Marksman::PiercingArrow:
		case Skills::FpArchMage::BigBang:
		case Skills::IlArchMage::BigBang:
		case Skills::Bishop::BigBang: {
			ChargeOrStationarySkillInfo info;
			info.skillId = skillId;
			info.level = packet.get<uint8_t>();
			info.direction = packet.get<uint8_t>();
			info.weaponSpeed = packet.get<uint8_t>();
			player->setChargeOrStationarySkill(info);
			SkillsPacket::showChargeOrStationarySkill(player, info);
			break;
		}
		case Skills::ChiefBandit::Chakra: {
			int16_t dex = player->getStats()->getDex(true);
			int16_t luk = player->getStats()->getLuk(true);
			int16_t recovery = player->getSkills()->getSkillInfo(skillId)->y;
			int16_t maximum = (luk * 66 / 10 + dex) * 2 / 10 * (recovery / 100 + 1);
			int16_t minimum = (luk * 33 / 10 + dex) * 2 / 10 * (recovery / 100 + 1);
			// Maximum = (luk * 6.6 + dex) * 0.2 * (recovery% / 100 + 1)
			// Minimum = (luk * 3.3 + dex) * 0.2 * (recovery% / 100 + 1)
			// I used 66 / 10 and 2 / 10 respectively to get 6.6 and 0.2 without using floating points
			player->getStats()->modifyHp(Randomizer::rand<int16_t>(maximum, minimum));
			break;
		}
	}
}

auto PlayerHandler::handleMonsterBook(Player *player, PacketReader &packet) -> void {
	int32_t cardId = packet.get<int32_t>();
	if (cardId != 0 && player->getMonsterBook()->getCard(cardId) == 0) {
		// Hacking
		return;
	}
	int32_t newCover = 0;
	if (cardId != 0) {
		newCover = ItemDataProvider::getInstance().getMobId(cardId);
	}
	player->getMonsterBook()->setCover(newCover);
	MonsterBookPacket::changeCover(player, cardId);
}

auto PlayerHandler::handleAdminMessenger(Player *player, PacketReader &packet) -> void {
	if (!player->isAdmin()) {
		// Hacking
		return;
	}
	Player *receiver = nullptr;
	bool hasTarget = packet.get<int8_t>() == 2;
	int8_t sort = packet.get<int8_t>();
	bool useWhisper = packet.get<bool>();
	int8_t type = packet.get<int8_t>();
	int32_t characterId = packet.get<int32_t>();

	if (player->getId() != characterId) {
		return;
	}

	string_t line1 = packet.getString();
	string_t line2 = packet.getString();
	string_t line3 = packet.getString();
	string_t line4 = packet.getString();
	string_t line5 = packet.getString();
	if (hasTarget) {
		receiver = PlayerDataProvider::getInstance().getPlayer(packet.getString());
	}

	int32_t time = 15;
	switch (type) {
		case 1: time = 30; break;
		case 2: time = 60; break;
	}

	MapleTvs::getInstance().addMessage(player, receiver, line1, line2, line3, line4, line5, 5075000 + type, time);
	if (sort == 1) {
		out_stream_t output;
		output << player->getMedalName() << " : " << line1 << line2 << line3 << line4 << line5;
		InventoryPacket::showSuperMegaphone(player, output.str(), useWhisper);
	}
}

auto PlayerHandler::useMeleeAttack(Player *player, PacketReader &packet) -> void {
	const Attack &attack = compileAttack(player, packet, SkillType::Melee);
	if (attack.portals != player->getPortalCount()) {
		// Usually evidence of hacking
		return;
	}
	PlayersPacket::useMeleeAttack(player, attack);
	int8_t damagedTargets = 0;
	int32_t skillId = attack.skillId;
	uint8_t level = attack.skillLevel;

	if (skillId != Skills::All::RegularAttack) {
		Skills::useAttackSkill(player, skillId);
	}

	int32_t map = player->getMapId();
	uint8_t ppLevel = player->getActiveBuffs()->getActiveSkillLevel(Skills::ChiefBandit::Pickpocket); // Check for active pickpocket level
	bool ppok = !attack.isMesoExplosion && ppLevel > 0;
	auto picking = SkillDataProvider::getInstance().getSkill(Skills::ChiefBandit::Pickpocket, ppLevel);
	Pos origin;
	vector_t<int32_t> ppDamages;

	for (const auto &target : attack.damages) {
		int32_t targetTotal = 0;
		int8_t connectedHits = 0;
		auto mob = Maps::getMap(map)->getMob(target.first);
		if (mob == nullptr) {
			continue;
		}
		origin = mob->getPos(); // Info for pickpocket before mob is set to nullptr (in the case that mob dies)
		for (const auto &hit : target.second) {
			int32_t damage = hit;
			if (damage != 0) {
				connectedHits++;
				targetTotal += damage;
			}
			if (ppok && Randomizer::rand<uint16_t>(99) < picking->prop) {
				 // Make sure this is a melee attack and not meso explosion, plus pickpocket being active
				ppDamages.push_back(damage);
			}
			if (mob == nullptr) {
				if (ppok) {
					// Roll along after the mob is dead to finish getting damage values for pickpocket
					continue;
				}
				break;
			}
			if (skillId == Skills::Paladin::HeavensHammer) {
				damage = (mob->isBoss() ? Stats::MaxDamage : (mob->getHp() - 1)); // If a Paladin wants to prove that it does something else, feel free
			}
			else if (skillId == Skills::Bandit::Steal && !mob->isBoss()) {
				DropHandler::doDrops(player->getId(), map, mob->getLevel(), mob->getMobId(), mob->getPos(), false, false, mob->getTauntEffect(), true);
			}
			int32_t tempHp = mob->getHp();
			mob->applyDamage(player->getId(), damage);
			if (tempHp <= damage) {
				// Mob was killed, so set the Mob pointer to 0
				mob = nullptr;
			}
		}
		if (targetTotal > 0) {
			if (mob != nullptr && mob->getHp() > 0) {
				MobHandler::handleMobStatus(player->getId(), mob, skillId, level, player->getInventory()->getEquippedId(EquipSlots::Weapon), connectedHits); // Mob status handler (freeze, stun, etc)
				if (mob->getHp() < mob->getSelfDestructHp()) {
					mob->explode();
				}
			}
			damagedTargets++;
		}
		uint8_t ppSize = ppDamages.size();
		for (uint8_t pickpocket = 0; pickpocket < ppSize; ++pickpocket) {
			// Drop stuff for Pickpocket
			Pos ppPos = origin;
			ppPos.x += (ppSize % 2 == 0 ? 5 : 0) + (ppSize / 2) - 20 * ((ppSize / 2) - pickpocket);

			int32_t ppMesos = ((ppDamages[pickpocket] * picking->x) / 10000); // TODO: Check on this formula in different situations
			Drop *ppDrop = new Drop(player->getMapId(), ppMesos, ppPos, player->getId(), true);
			ppDrop->setTime(100);
			Timer::Timer::create([ppDrop, origin](const time_point_t &now) { ppDrop->doDrop(origin); },
				Timer::Id(Timer::Types::PickpocketTimer, player->getId(), player->getActiveBuffs()->getPickpocketCounter()),
				nullptr, milliseconds_t(175 * pickpocket));
		}
		ppDamages.clear();
	}

	if (player->getSkills()->hasEnergyCharge()) {
		player->getActiveBuffs()->increaseEnergyChargeLevel(damagedTargets);
	}

	switch (skillId) {
		case Skills::ChiefBandit::MesoExplosion: {
			uint8_t items = packet.get<int8_t>();
			Map *map = player->getMap();
			for (uint8_t i = 0; i < items; i++) {
				int32_t objId = packet.get<int32_t>();
				packet.skipBytes(1); // Some value
				if (Drop *drop = map->getDrop(objId)) {
					if (!drop->isMesos()) {
						// Hacking
						return;
					}
					DropsPacket::explodeDrop(drop);
					map->removeDrop(drop->getId());
					delete drop;
				}
			}
			break;
		}
		case Skills::Marauder::EnergyDrain:
		case Skills::ThunderBreaker::EnergyDrain: {
			int32_t hpRecover = static_cast<int32_t>(attack.totalDamage * player->getSkills()->getSkillInfo(skillId)->x / 100);
			if (hpRecover > player->getStats()->getMaxHp()) {
				player->getStats()->setHp(player->getStats()->getMaxHp());
			}
			else {
				player->getStats()->modifyHp(static_cast<int16_t>(hpRecover));
			}
			break;
		}
		case Skills::Crusader::SwordPanic: // Crusader finishers
		case Skills::Crusader::SwordComa:
		case Skills::Crusader::AxePanic:
		case Skills::Crusader::AxeComa:
		case Skills::DawnWarrior::Panic:
		case Skills::DawnWarrior::Coma:
			player->getActiveBuffs()->setCombo(0, true);
			break;
		case Skills::Crusader::Shout:
		case Skills::Gm::SuperDragonRoar:
		case Skills::SuperGm::SuperDragonRoar:
			break;
		case Skills::DragonKnight::DragonRoar: {
			int16_t xProperty = SkillDataProvider::getInstance().getSkill(skillId, level)->x;
			uint16_t reduction = (player->getStats()->getMaxHp() / 100) * xProperty;
			if (reduction < player->getStats()->getHp()) {
				player->getStats()->damageHp(reduction);
			}
			else {
				// Hacking
				return;
			}
			Buffs::addBuff(player, Skills::DragonKnight::DragonRoar, level, 0);
			break;
		}
		case Skills::DragonKnight::Sacrifice: {
			if (attack.totalDamage > 0) {
				int16_t xProperty = player->getSkills()->getSkillInfo(skillId)->x;
				int32_t hpDamage = static_cast<int32_t>(attack.totalDamage * xProperty / 100);
				if (hpDamage > player->getStats()->getHp()) {
					hpDamage = player->getStats()->getHp() - 1;
				}
				if (hpDamage > 0) {
					player->getStats()->damageHp(hpDamage);
				}
			}
			break;
		}
		case Skills::WhiteKnight::ChargeBlow: {
			int8_t skillLevel = player->getSkills()->getSkillLevel(Skills::Paladin::AdvancedCharge);
			int16_t xProperty = 0;
			if (skillLevel > 0) {
				xProperty = SkillDataProvider::getInstance().getSkill(Skills::Paladin::AdvancedCharge, skillLevel)->x;
			}
			if ((xProperty != 100) && (xProperty == 0 || Randomizer::rand<int16_t>(99) > (xProperty - 1))) {
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

auto PlayerHandler::useRangedAttack(Player *player, PacketReader &packet) -> void {
	const Attack &attack = compileAttack(player, packet, SkillType::Ranged);
	if (attack.portals != player->getPortalCount()) {
		// Usually evidence of hacking
		return;
	}
	PlayersPacket::useRangedAttack(player, attack);
	int32_t skillId = attack.skillId;
	uint8_t level = attack.skillLevel;

	switch (skillId) {
		case Skills::Bowmaster::Hurricane:
		case Skills::WindArcher::Hurricane:
		case Skills::Corsair::RapidFire:
			if (!player->hasChargeOrStationarySkill()) {
				ChargeOrStationarySkillInfo info;
				info.skillId = skillId;
				info.direction = attack.animation;
				info.weaponSpeed = attack.weaponSpeed;
				info.level = level;
				player->setChargeOrStationarySkill(info);
				SkillsPacket::showChargeOrStationarySkill(player, info);
			}
			break;
	}

	Skills::useAttackSkillRanged(player, skillId, attack.starPos);

	int32_t maxHp = 0;
	int32_t firstHit = 0;

	for (const auto &target : attack.damages) {
		int32_t mapMobId = target.first;
		auto mob = player->getMap()->getMob(mapMobId);
		if (mob == nullptr) {
			continue;
		}
		int32_t targetTotal = 0;
		int8_t connectedHits = 0;

		for (const auto &hit : target.second) {
			int32_t damage = hit;

			if (damage != 0) {
				connectedHits++;
				targetTotal += damage;
			}
			if (firstHit == 0) {
				firstHit = damage;
			}
			if (mob == nullptr) {
				continue;
			}
			maxHp = mob->getMaxHp();
			if (skillId == Skills::Ranger::MortalBlow || skillId == Skills::Sniper::MortalBlow) {
				auto sk = player->getSkills()->getSkillInfo(skillId);
				int32_t hpPercentage = maxHp * sk->x / 100; // Percentage of HP required for Mortal Blow activation
				if (mob->getHp() < hpPercentage && Randomizer::rand<int16_t>(99) < sk->y) {
					damage = mob->getHp();
				}
			}
			else if (skillId == Skills::Outlaw::HomingBeacon || skillId == Skills::Corsair::Bullseye) {
				Buffs::addBuff(player, skillId, level, 0, mapMobId);
			}
			int32_t tempHp = mob->getHp();
			mob->applyDamage(player->getId(), damage);
			if (tempHp <= damage) {
				mob = nullptr;
			}
		}
		if (mob != nullptr && targetTotal > 0 && mob->getHp() > 0) {
			MobHandler::handleMobStatus(player->getId(), mob, skillId, level, player->getInventory()->getEquippedId(EquipSlots::Weapon), connectedHits, firstHit); // Mob status handler (freeze, stun, etc)
			if (mob->getHp() < mob->getSelfDestructHp()) {
				mob->explode();
			}
		}
	}

	switch (skillId) {
		case Skills::Assassin::Drain: {
			int16_t xProperty = player->getSkills()->getSkillInfo(skillId)->x;
			int32_t hpRecover = static_cast<int32_t>(attack.totalDamage * xProperty / 100);
			int16_t playerMaxHp = player->getStats()->getMaxHp();
			if (hpRecover > maxHp) {
				hpRecover = maxHp;
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
		case Skills::DawnWarrior::SoulBlade:
			if (attack.totalDamage > 0) {
				player->getActiveBuffs()->addCombo();
			}
			break;
	}
}

auto PlayerHandler::useSpellAttack(Player *player, PacketReader &packet) -> void {
	const Attack &attack = compileAttack(player, packet, SkillType::Magic);
	if (attack.portals != player->getPortalCount()) {
		// Usually evidence of hacking
		return;
	}
	PlayersPacket::useSpellAttack(player, attack);

	int32_t skillId = attack.skillId;
	uint8_t level = attack.skillLevel;

	MpEaterInfo eater;
	eater.skillId = player->getSkills()->getMpEater();
	eater.level = player->getSkills()->getSkillLevel(eater.skillId);
	if (eater.level > 0) {
		auto skillInfo = SkillDataProvider::getInstance().getSkill(eater.skillId, eater.level);
		eater.prop = skillInfo->prop;
		eater.x = skillInfo->x;
	}

	if (!attack.isHeal) {
		// Heal is sent as both an attack and as a used skill - it's only sometimes an attack
		Skills::useAttackSkill(player, skillId);
	}

	for (const auto &target : attack.damages) {
		int32_t targetTotal = 0;
		int32_t mapMobId = target.first;
		int8_t connectedHits = 0;
		auto mob = player->getMap()->getMob(mapMobId);
		if (mob == nullptr) {
			continue;
		}
		if (attack.isHeal && !mob->isUndead()) {
			// Hacking
			return;
		}

		for (const auto &hit : target.second) {
			int32_t damage = hit;
			if (damage != 0) {
				connectedHits++;
				targetTotal += damage;
			}
			if (damage != 0 && eater.level != 0 && !eater.used) {
				// MP Eater
				mob->mpEat(player, &eater);
			}
			int32_t tempHp = mob->getHp();
			mob->applyDamage(player->getId(), damage);
			if (tempHp <= damage) {
				// Mob was killed, so set the Mob pointer to 0
				mob = nullptr;
				break;
			}
		}
		if (mob != nullptr && targetTotal > 0 && mob->getHp() > 0) {
			MobHandler::handleMobStatus(player->getId(), mob, skillId, level, player->getInventory()->getEquippedId(EquipSlots::Weapon), connectedHits); // Mob status handler (freeze, stun, etc)
			if (mob->getHp() < mob->getSelfDestructHp()) {
				mob->explode();
			}
		}
	}

	switch (skillId) {
		case Skills::FpMage::PoisonMist:
		case Skills::BlazeWizard::FlameGear: {
			auto skill = SkillDataProvider::getInstance().getSkill(skillId, level);
			Mist *mist = new Mist(player->getMapId(), player, skill->time, skill->dimensions.move(player->getPos()), skillId, level, true);
			break;
		}
	}
}

auto PlayerHandler::useEnergyChargeAttack(Player *player, PacketReader &packet) -> void {
	const Attack &attack = compileAttack(player, packet, SkillType::EnergyCharge);
	PlayersPacket::useEnergyChargeAttack(player, attack);

	int32_t skillId = attack.skillId;
	int8_t level = attack.skillLevel;

	for (const auto &target : attack.damages) {
		int32_t targetTotal = 0;
		int32_t mapMobId = target.first;
		int8_t connectedHits = 0;
		auto mob = player->getMap()->getMob(mapMobId);
		if (mob == nullptr) {
			continue;
		}

		for (const auto &hit : target.second) {
			int32_t damage = hit;
			if (damage != 0) {
				connectedHits++;
				targetTotal += damage;
			}
			int32_t tempHp = mob->getHp();
			mob->applyDamage(player->getId(), damage);
			if (tempHp <= damage) {
				// Mob was killed, so set the Mob pointer to 0
				mob = nullptr;
				break;
			}
		}
		if (mob != nullptr && targetTotal > 0 && mob->getHp() > 0) {
			MobHandler::handleMobStatus(player->getId(), mob, skillId, level, player->getInventory()->getEquippedId(EquipSlots::Weapon), connectedHits); // Mob status handler (freeze, stun, etc)
			if (mob->getHp() < mob->getSelfDestructHp()) {
				mob->explode();
			}
		}
	}
}

auto PlayerHandler::useSummonAttack(Player *player, PacketReader &packet) -> void {
	const Attack &attack = compileAttack(player, packet, SkillType::Summon);
	Summon *summon = player->getSummons()->getSummon();
	if (summon == nullptr) {
		// Hacking or some other form of tomfoolery
		return;
	}
	PlayersPacket::useSummonAttack(player, attack);
	int32_t skillId = summon->getSummonId();
	for (const auto &target : attack.damages) {
		int32_t targetTotal = 0;
		int32_t mapMobId = target.first;
		int8_t connectedHits = 0;
		auto mob = player->getMap()->getMob(mapMobId);
		if (mob == nullptr) {
			continue;
		}
		for (const auto &hit : target.second) {
			int32_t damage = hit;
			if (damage != 0) {
				connectedHits++;
				targetTotal += damage;
			}
			int32_t tempHp = mob->getHp();
			mob->applyDamage(player->getId(), damage);
			if (tempHp <= damage) {
				// Mob was killed, so set the Mob pointer to 0
				mob = nullptr;
				break;
			}
		}
		if (mob != nullptr && targetTotal > 0 && mob->getHp() > 0) {
			MobHandler::handleMobStatus(player->getId(), mob, skillId, player->getSkills()->getSkillLevel(skillId), player->getInventory()->getEquippedId(EquipSlots::Weapon), connectedHits); // Mob status handler (freeze, stun, etc)
			if (mob->getHp() < mob->getSelfDestructHp()) {
				mob->explode();
			}
		}
	}
}

auto PlayerHandler::compileAttack(Player *player, PacketReader &packet, SkillType skillType) -> Attack {
	Attack attack;
	int8_t targets = 0;
	int8_t hits = 0;
	int32_t skillId = 0;
	bool mesoExplosion = false;
	bool shadowMeso = false;

	if (skillType != SkillType::Summon) {
		attack.portals = packet.get<uint8_t>();
		uint8_t tByte = packet.get<uint8_t>();
		skillId = packet.get<int32_t>();
		targets = tByte / 0x10;
		hits = tByte % 0x10;

		if (skillId != Skills::All::RegularAttack) {
			attack.skillLevel = player->getSkills()->getSkillLevel(skillId);
		}

		packet.skipBytes(4); // Unk, strange constant that doesn't seem to change
		// Things atttemped: Map changes, character changes, job changes, skill changes, position changes, hitting enemies
		// It appears as 0xF9B16E60 which is 4189154912 unsigned, -105812384 signed, doesn't seem to be a size, probably a CRC
		packet.skipBytes(4); // Unk, strange constant dependent on skill, probably a CRC

		switch (skillId) {
			case Skills::Hermit::ShadowMeso:
				attack.isShadowMeso = true;
				shadowMeso = true;
				break;
			case Skills::ChiefBandit::MesoExplosion:
				attack.isMesoExplosion = true;
				mesoExplosion = true;
				break;
			case Skills::Cleric::Heal:
				attack.isHeal = true;
				break;
			case Skills::Gunslinger::Grenade:
			case Skills::Brawler::CorkscrewBlow:
			case Skills::Bowmaster::Hurricane:
			case Skills::WindArcher::Hurricane:
			case Skills::Marksman::PiercingArrow:
			case Skills::NightWalker::PoisonBomb:
			case Skills::Corsair::RapidFire:
			case Skills::FpArchMage::BigBang:
			case Skills::IlArchMage::BigBang:
			case Skills::Bishop::BigBang:
				attack.isChargeSkill = true;
				attack.charge = packet.get<int32_t>();
				break;
		}

		attack.display = packet.get<uint8_t>();
		attack.animation = packet.get<uint8_t>();
		attack.weaponClass = packet.get<uint8_t>();
		attack.weaponSpeed = packet.get<uint8_t>();
		attack.ticks = packet.get<uint32_t>();
	}
	else {
		attack.summonId = packet.get<int32_t>(); // Summon ID, not to be confused with summon skill ID
		attack.ticks = packet.get<uint32_t>();
		attack.animation = packet.get<uint8_t>();
		targets = packet.get<int8_t>();
		hits = 1;
	}

	if (skillType == SkillType::Ranged) {
		int16_t starSlot = packet.get<int16_t>();
		int16_t csStar = packet.get<int16_t>();
		attack.starPos = starSlot;
		attack.cashStarPos = csStar;
		packet.skipBytes(1); // 0x00 = AoE?
		if (!shadowMeso) {
			if (player->getActiveBuffs()->hasShadowStars() && skillId != Skills::NightLord::Taunt) {
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
	attack.skillId = skillId;

	for (int8_t i = 0; i < targets; ++i) {
		int32_t mapMobId = packet.get<int32_t>();
		packet.skipBytes(4); // Always 0x06, <two bytes of some kind>, 0x01
		packet.skipBytes(8); // Mob pos, damage pos
		if (!mesoExplosion) {
			packet.skipBytes(2); // Distance
		}
		else {
			hits = packet.get<int8_t>(); // Hits for Meso Explosion
		}
		for (int8_t k = 0; k < hits; ++k) {
			int32_t damage = packet.get<int32_t>();
			attack.damages[mapMobId].push_back(damage);
			attack.totalDamage += damage;
		}
		if (skillType != SkillType::Summon) {
			packet.skipBytes(4); // 4 bytes of unknown purpose, differs by the mob, probably a CRC
		}
	}

	if (skillType == SkillType::Ranged) {
		attack.projectilePos = packet.getClass<Pos>();
	}
	attack.playerPos = packet.getClass<Pos>();

	return attack;
}