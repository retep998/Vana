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
#include "Drop.h"
#include "DropHandler.h"
#include "DropsPacket.h"
#include "GameConstants.h"
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
#include "SkillDataProvider.h"
#include "Skills.h"
#include "SkillsPacket.h"
#include "Summons.h"
#include "Timer/Time.h"
#include "Timer/Timer.h"
#include <functional>

using std::tr1::bind;

void PlayerHandler::handleDamage(Player *player, PacketReader &packet) {
	const int8_t BumpDamage = -1;
	const int8_t MapDamage = -2;

	packet.skipBytes(4); // Ticks
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
	Mob *mob = 0;
	ReturnDamageInfo pgmr;

	if (type != MapDamage) {
		packet.skipBytes(4); // Real mob ID, no reason to rely on the packet

		mapmobid = packet.get<int32_t>();
		mob = Maps::getMap(player->getMap())->getMob(mapmobid);
		if (mob == 0) {
			// Hacking
			return;
		}

		mobid = mob->getMobId();
		if (type != BumpDamage) {
			int32_t attackerid = (mob->hasLink() ? mob->getLink() : mobid);
			MobAttackInfo *attack = MobDataProvider::Instance()->getMobAttack(attackerid, type);
			if (attack == 0) {
				// Hacking, I think
				return;
			}
			disease = attack->disease;
			level = attack->level;
			mpburn = attack->mpburn;
			deadlyattack = attack->deadlyattack;
		}
		hit = packet.get<int8_t>(); // Knock direction
		pgmr.reduction = packet.get<int8_t>();
		packet.skipBytes(1); // I think reduction is a short, but it's a byte in the S -> C packet, so..
		if (pgmr.reduction != 0) {
			if (!packet.getBool())
				pgmr.isphysical = false;
			pgmr.mapmobid = packet.get<int32_t>();
			packet.skipBytes(1); // 0x06 for Power Guard, 0x00 for Mana Reflection?
			packet.skipBytes(4); // Mob position garbage
			pgmr.pos.x = packet.get<int16_t>();
			pgmr.pos.y = packet.get<int16_t>();
			pgmr.damage = damage;
			if (pgmr.isphysical) // Only Power Guard decreases damage
				damage = (damage - (damage * pgmr.reduction / 100));
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

	if (disease > 0 && damage != 0) { // Fake/Guardian don't prevent disease
		player->getActiveBuffs()->addDebuff(disease, level);
	}

	if (damage > 0 && !player->hasGmEquip()) {
		if (player->getActiveBuffs()->hasMesoGuard() && player->getInventory()->getMesos() > 0) {
			int32_t sid = player->getActiveBuffs()->getMesoGuard();
			int16_t mesorate = player->getActiveBuffs()->getActiveSkillInfo(sid)->x; // Meso Guard meso %
			int16_t mesoloss = (int16_t)(mesorate * damage / 2 / 100);
			int32_t mesos = player->getInventory()->getMesos();
			int32_t newmesos = mesos - mesoloss;

			if (newmesos < 0) { // Special damage calculation for not having enough mesos
				double reduction = 2.0 - ((double)(mesos / mesoloss)) / 2.0;
				damage = (uint16_t)(damage / reduction); // This puts us pretty close to the damage observed clientside, needs improvement
			}
			else {
				damage /= 2; // Usually displays 1 below the actual damage but is sometimes accurate - no clue why
			}

			player->getInventory()->setMesos(newmesos);
			player->getStats()->damageHp((uint16_t) damage);

			if (deadlyattack && player->getStats()->getMp() > 0)
				player->getStats()->setMp(1);
			if (mpburn > 0)
				player->getStats()->damageMp(mpburn);

			applieddamage = true;

			SkillsPacket::showSkillEffect(player, sid);
		}
		if (player->getActiveBuffs()->hasMagicGuard()) {
			int16_t mp = player->getStats()->getMp();
			int16_t hp = player->getStats()->getHp();

			if (deadlyattack) {
				if (mp > 0)
					player->getStats()->setMp(1);
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

			if (deadlyattack && player->getStats()->getMp() > 0)
				player->getStats()->setMp(1);
			if (mpburn > 0)
				player->getStats()->damageMp(mpburn);

			applieddamage = true;
		}

		if (!applieddamage) {
			if (deadlyattack) {
				if (player->getStats()->getMp() > 0)
					player->getStats()->setMp(1);
				player->getStats()->setHp(1);
			}
			else {
				player->getStats()->damageHp((uint16_t) damage);
			}

			if (mpburn > 0)
				player->getStats()->damageMp(mpburn);

			if (player->getActiveBuffs()->getActiveSkillLevel(Jobs::Corsair::Battleship) > 0) {
				player->getActiveBuffs()->reduceBattleshipHp((uint16_t) damage);
			}
		}
		int32_t morph = player->getActiveBuffs()->getCurrentMorph();
		if (morph < 0 || (morph != 0 && player->getStats()->getHp() == 0)) {
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
	packet.skipBytes(4);
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
	packet.reset(11);
	MovementHandler::parseMovement(player, packet);
	packet.reset(11);
	PlayersPacket::showMoving(player, packet.getBuffer(), packet.getBufferLength());
	if (player->getFh() == 0) {
		int32_t mapid = player->getMap();
		Pos playerpos = player->getPos();
		Map *map = Maps::getMap(mapid);

		Pos floor = map->findFloor(playerpos);
		if (floor.y == playerpos.y) { // There are no footholds below the player
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
		case Jobs::Hero::MonsterMagnet: // Monster Magnet x3
		case Jobs::Paladin::MonsterMagnet:
		case Jobs::DarkKnight::MonsterMagnet:
		case Jobs::Marksman::PiercingArrow: // Pierce
		case Jobs::FPArchMage::BigBang: // Big Bang x3
		case Jobs::ILArchMage::BigBang:
		case Jobs::Bishop::BigBang: {
			SpecialSkillInfo info;
			info.skillid = skillid;
			info.level = packet.get<int8_t>();
			info.direction = packet.get<int8_t>();
			info.w_speed = packet.get<int8_t>();
			player->setSpecialSkill(info);
			SkillsPacket::showSpecialSkill(player, info);
			break;
		}
		case Jobs::ChiefBandit::Chakra: { // Chakra
			int16_t dex = player->getStats()->getDex(true);
			int16_t luk = player->getStats()->getLuk(true);
			int16_t recovery = player->getSkills()->getSkillInfo(skillid)->y;
			int16_t maximum = (luk * 66 / 10 + dex) * 2 / 10 * (recovery / 100 + 1);
			int16_t minimum = (luk * 33 / 10 + dex) * 2 / 10 * (recovery / 100 + 1);
			// Maximum = (luk * 6.6 + dex) * 0.2 * (recovery% / 100 + 1)
			// Minimum = (luk * 3.3 + dex) * 0.2 * (recovery% / 100 + 1)
			// I used 66 / 10 and 2 / 10 respectively to get 6.6 and 0.2 without using floating points
			int16_t range = maximum - minimum;
			player->getStats()->modifyHp(Randomizer::Instance()->randShort(range) + minimum);
			break;
		}
	}
}

void PlayerHandler::handleMonsterBook(Player *player, PacketReader &packet) {
	int32_t cardid = packet.get<int32_t>();
	if (cardid != 0 && player->getMonsterBook()->getCard(cardid) == 0) {
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
		// Hack
		player->addWarning();
		return;
	}
	Player *receiver = 0;
	bool has_to_name = packet.get<int8_t>() == 2;
	int8_t sort = packet.get<int8_t>();
	bool use_whisper = packet.getBool();
	int8_t type = packet.get<int8_t>();
	int32_t character_id = packet.get<int32_t>();

	if (player->getId() != character_id)
		return;

	string line1 = packet.getString();
	string line2 = packet.getString();
	string line3 = packet.getString();
	string line4 = packet.getString();
	string line5 = packet.getString();
	if (has_to_name)
		receiver = PlayerDataProvider::Instance()->getPlayer(packet.getString());

	int32_t time = 15;
	switch (type) {
		case 1: time = 30; break;
		case 2: time = 60; break;
	}

	MapleTvs::Instance()->addMessage(player, receiver, line1, line2, line3, line4, line5, 5075000 + type, time);
	if (sort == 1)
		InventoryPacket::showSuperMegaphone(player, player->getMedalName() + " : " + line1 + line2 + line3 + line4 + line5, use_whisper);
}

void PlayerHandler::useMeleeAttack(Player *player, PacketReader &packet) {
	PlayersPacket::useMeleeAttack(player, packet);
	packet.reset(2);
	packet.skipBytes(1); // Useless
	uint8_t tbyte = packet.get<int8_t>();
	int8_t targets = tbyte / 0x10;
	int8_t hits = tbyte % 0x10;
	int8_t damagedtargets = 0;
	int32_t skillid = packet.get<int32_t>();
	uint8_t level = player->getSkills()->getSkillLevel(skillid);
	switch (skillid) {
		case Jobs::Gunslinger::Grenade:
		case Jobs::Brawler::CorkscrewBlow:
			packet.skipBytes(4); // Charge
			break;
	}
	packet.skipBytes(4); // Unk
	packet.skipBytes(8); // In order: Display [1], Animation [1], Weapon subclass [1], Weapon speed [1], Tick count [4]
	if (skillid != Jobs::All::RegularAttack)
		Skills::useAttackSkill(player, skillid);
	int32_t map = player->getMap();
	uint32_t totaldmg = 0;
	uint8_t pplevel = player->getActiveBuffs()->getActiveSkillLevel(Jobs::ChiefBandit::Pickpocket); // Check for active pickpocket level
	SkillLevelInfo *picking = SkillDataProvider::Instance()->getSkill(Jobs::ChiefBandit::Pickpocket, pplevel);
	packet.skipBytes(4); // Unknown int added in V.74+
	for (int8_t i = 0; i < targets; i++) {
		int32_t targettotal = 0;
		int32_t mapmobid = packet.get<int32_t>();
		int8_t connectedhits = 0;
		Mob *mob = Maps::getMap(map)->getMob(mapmobid);
		if (mob == 0)
			continue;
		packet.skipBytes(3); // Useless
		packet.skipBytes(1); // State
		packet.skipBytes(8); // Useless
		if (skillid != Jobs::ChiefBandit::MesoExplosion)
			packet.skipBytes(1); // Distance, first half for non-Meso Explosion
		int8_t num = packet.get<int8_t>(); // Distance, second half for non-Meso Explosion OR hits for Meso Explosion
		hits = skillid == Jobs::ChiefBandit::MesoExplosion ? num : hits;
		vector<int32_t> ppdamages; // Pickpocket
		Pos origin = mob->getPos(); // Info for pickpocket before mob is set to 0 (in the case that mob dies)
		for (int8_t k = 0; k < hits; k++) {
			int32_t damage = packet.get<int32_t>();
			targettotal += damage;
			if (damage != 0)
				connectedhits++;
			if (skillid != Jobs::ChiefBandit::MesoExplosion && pplevel > 0) { // Make sure this is a melee attack and not meso explosion, plus pickpocket being active
				if (Randomizer::Instance()->randInt(99) < picking->prop) {
					ppdamages.push_back(damage);
				}
			}
			if (mob == 0) {
				if (skillid != Jobs::ChiefBandit::MesoExplosion && pplevel > 0) // Roll along after the mob is dead to finish getting damage values for pickpocket
					continue;
				else {
					packet.skipBytes(4 * (hits - 1 - k));
					break;
				}
			}
			if (skillid == Jobs::Paladin::HeavensHammer) {
				damage = (mob->isBoss() ? Stats::MaxDamage : (mob->getHp() - 1)); // If a Paladin wants to prove that it does something else, feel free
			}
			else if (skillid == Jobs::Bandit::Steal && !mob->isBoss()) {
				DropHandler::doDrops(player->getId(), map, mob->getInfo()->level, mob->getMobId(), mob->getPos(), false, false, mob->getTauntEffect(), true);
			}
			int32_t temphp = mob->getHp();
			mob->applyDamage(player->getId(), damage);
			if (temphp - damage <= 0) // Mob was killed, so set the Mob pointer to 0
				mob = 0;
		}
		if (targettotal > 0) {
			if (mob != 0 && mob->getHp() > 0) {
				MobHandler::handleMobStatus(player->getId(), mob, skillid, level, player->getInventory()->getEquippedId(EquipSlots::Weapon), connectedhits); // Mob status handler (freeze, stun, etc)
				if (mob->getHp() < mob->getSelfDestructHp()) {
					mob->explode();
				}
			}
			damagedtargets++;
		}
		totaldmg += targettotal;
		uint8_t ppdamagesize = (uint8_t)(ppdamages.size());
		for (uint8_t pickpocket = 0; pickpocket < ppdamagesize; pickpocket++) { // Drop stuff for Pickpocket
			Pos pppos;
			pppos.x = origin.x + (ppdamagesize % 2 == 0 ? 5 : 0) + (ppdamagesize / 2) - 20 * ((ppdamagesize / 2) - pickpocket);
			pppos.y = origin.y;
			clock_t pptime = 175 * pickpocket;
			int32_t ppmesos = ((ppdamages[pickpocket] * picking->x) / 10000); // TODO: Check on this formula in different situations
			Drop *ppdrop = new Drop(player->getMap(), ppmesos, pppos, player->getId(), true);
			ppdrop->setTime(100);
			new Timer::Timer(bind(&Drop::doDrop, ppdrop, origin),
				Timer::Id(Timer::Types::PickpocketTimer, player->getId(), player->getActiveBuffs()->getPickpocketCounter()),
				0, Timer::Time::fromNow(pptime));
		}
		packet.skipBytes(4); // 4 bytes of unknown purpose, new in .56
	}
	packet.skipBytes(4); // Character positioning, end of packet, might eventually be useful for hacking detection

	if (player->getSkills()->hasEnergyCharge())
		player->getActiveBuffs()->increaseEnergyChargeLevel(damagedtargets);

	switch (skillid) {
		case Jobs::ChiefBandit::MesoExplosion: {
			uint8_t items = packet.get<int8_t>();
			int32_t map = player->getMap();
			for (uint8_t i = 0; i < items; i++) {
				int32_t objId = packet.get<int32_t>();
				packet.skipBytes(1); // Boolean for hit a monster
				Drop *drop = Maps::getMap(map)->getDrop(objId);
				if (drop != 0) {
					DropsPacket::explodeDrop(drop);
					Maps::getMap(map)->removeDrop(drop->getId());
					delete drop;
				}
			}
			break;
		}
		case Jobs::Marauder::EnergyDrain:
		case Jobs::ThunderBreaker::EnergyDrain: {
			int32_t hpRecover = totaldmg * player->getSkills()->getSkillInfo(skillid)->x / 100;
			if (hpRecover > player->getStats()->getMHp())
				player->getStats()->setHp(player->getStats()->getMHp());
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
			int8_t roarlv = player->getSkills()->getSkillLevel(skillid);
			int16_t x_value = SkillDataProvider::Instance()->getSkill(skillid, roarlv)->x;
			uint16_t reduction = (player->getStats()->getMHp() / 100) * x_value;
			if ((player->getStats()->getHp() - reduction) > 0)
				player->getStats()->damageHp(reduction);
			else {
				// Hacking
				return;
			}
			Buffs::addBuff(player, Jobs::DragonKnight::DragonRoar, roarlv, 0);
			break;
		}
		case Jobs::DragonKnight::Sacrifice: {
			int16_t hp_damage_x = player->getSkills()->getSkillInfo(skillid)->x;
			uint16_t hp_damage = (uint16_t) totaldmg * hp_damage_x / 100;
			if ((player->getStats()->getHp() - hp_damage) < 1)
				player->getStats()->setHp(1);
			else
				player->getStats()->damageHp(hp_damage);
			break;
		}
		case Jobs::WhiteKnight::ChargeBlow: {
			int8_t acb_level = player->getSkills()->getSkillLevel(Jobs::Paladin::AdvancedCharge);
			int16_t acb_x = 0;
			if (acb_level > 0)
				acb_x = SkillDataProvider::Instance()->getSkill(Jobs::Paladin::AdvancedCharge, acb_level)->x;
			if ((acb_x != 100) && (acb_x == 0 || Randomizer::Instance()->randShort(99) > (acb_x - 1)))
				player->getActiveBuffs()->stopCharge();
			break;
		}
		default:
			if (totaldmg > 0)
				player->getActiveBuffs()->addCombo();
			break;
	}
}

void PlayerHandler::useRangedAttack(Player *player, PacketReader &packet) {
	PlayersPacket::useRangedAttack(player, packet);
	packet.reset(2); // Passing to the display function causes the buffer to be eaten, we need it
	packet.skipBytes(1); // Number of portals taken (not kidding)
	uint8_t tbyte = packet.get<int8_t>();
	int8_t targets = tbyte / 0x10;
	int8_t hits = tbyte % 0x10;
	int32_t skillid = packet.get<int32_t>();
	packet.skipBytes(4); // Unk
	switch (skillid) {
		case Jobs::Bowmaster::Hurricane:
		case Jobs::WindArcher::Hurricane:
		case Jobs::Marksman::PiercingArrow:
		case Jobs::NightWalker::PoisonBomb:
		case Jobs::Corsair::RapidFire:
			packet.skipBytes(4); // Charge time
			packet.skipBytes(1); // Projectile display
			if (skillid != Jobs::Marksman::PiercingArrow && player->getSpecialSkill() == 0) { // Only Piercing Arrow doesn't fit the mold
				SpecialSkillInfo info;
				info.skillid = skillid;
				info.direction = packet.get<int8_t>();
				packet.skipBytes(1); // Weapon subclass
				info.w_speed = packet.get<int8_t>();
				info.level = player->getSkills()->getSkillLevel(info.skillid);
				player->setSpecialSkill(info);
				SkillsPacket::showSpecialSkill(player, info);
			}
			else
				packet.skipBytes(3);
			break;
		default:
			packet.skipBytes(4); // Projectile display [1], direction/animation [1], weapon subclass [1], weapon speed [1]
			break;
	}
	packet.skipBytes(4); // 0.74 added sumthin
	packet.skipBytes(4); // Ticks
	int16_t pos = packet.get<int16_t>();
	packet.skipBytes(2); // Cash Shop star cover
	packet.skipBytes(1); // 0x00 = AoE, 0x41 = other
	if (skillid != Jobs::Hermit::ShadowMeso && player->getActiveBuffs()->hasShadowStars())
		packet.skipBytes(4); // Star ID added by Shadow Stars
	Skills::useAttackSkillRanged(player, skillid, pos);
	int32_t mhp = 0;
	uint32_t totaldmg = damageMobs(player, packet, targets, hits, skillid, mhp);
	if (skillid == Jobs::Assassin::Drain) { // Drain
		int16_t drain_x = player->getSkills()->getSkillInfo(skillid)->x;
		int32_t hpRecover = totaldmg * drain_x / 100;
		if (hpRecover > mhp)
			hpRecover = mhp;
		if (hpRecover > (player->getStats()->getMHp() / 2))
			hpRecover = player->getStats()->getMHp() / 2;
		if (hpRecover > player->getStats()->getMHp())
			player->getStats()->setHp(player->getStats()->getMHp());
		else
			player->getStats()->modifyHp((int16_t) hpRecover);
	}
	else if (skillid == Jobs::DawnWarrior::SoulBlade && totaldmg > 0)
		player->getActiveBuffs()->addCombo();
}

void PlayerHandler::useSpellAttack(Player *player, PacketReader &packet) {
	PlayersPacket::useSpellAttack(player, packet);
	packet.reset(2);
	packet.skipBytes(1);
	uint8_t tbyte = packet.get<int8_t>();
	int8_t targets = tbyte / 0x10;
	int8_t hits = tbyte % 0x10;
	int32_t skillid = packet.get<int32_t>();
	switch (skillid) {
		case Jobs::FPArchMage::BigBang: // Big Bang has a 4 byte charge time after skillid
		case Jobs::ILArchMage::BigBang:
		case Jobs::Bishop::BigBang:
			packet.skipBytes(4);
			break;
	}
	MpEaterInfo eater;
	eater.id = player->getSkills()->getMpEater();
	eater.level = player->getSkills()->getSkillLevel(eater.id);
	if (eater.level > 0) {
		SkillLevelInfo *eaaat = SkillDataProvider::Instance()->getSkill(eater.id, eater.level);
		eater.prop = eaaat->prop;
		eater.x = eaaat->x;
	}
	packet.skipBytes(4); //added in 0.74
	packet.skipBytes(4); // Unk
	packet.skipBytes(2); // Display, direction/animation
	packet.skipBytes(2); // Weapon subclass, casting speed
	packet.skipBytes(4); // Ticks
	if (skillid != Jobs::Cleric::Heal) // Heal is sent as both an attack and as a used skill - always used, sometimes attack
		Skills::useAttackSkill(player, skillid);
	int32_t useless = 0;
	uint32_t totaldmg = damageMobs(player, packet, targets, hits, skillid, useless, &eater);
	switch (skillid) {
		case Jobs::FPMage::PoisonMist:
		case Jobs::BlazeWizard::FlameGear: {
			uint8_t level = player->getSkills()->getSkillLevel(skillid);
			Mist *mist = new Mist(player->getMap(), player, player->getPos(), SkillDataProvider::Instance()->getSkill(skillid, level), skillid, level, true);
			break;
		}
	}
}

void PlayerHandler::useEnergyChargeAttack(Player *player, PacketReader &packet) {
	PlayersPacket::useEnergyChargeAttack(player, packet);
	packet.reset(2);
	packet.skipBytes(1);
	uint8_t tbyte = packet.get<int8_t>();
	int8_t targets = tbyte / 0x10;
	int8_t hits = tbyte % 0x10;
	int32_t skillid = packet.get<int32_t>();
	packet.skipBytes(4); // Unk
	packet.skipBytes(2); // Display, direction/animation
	packet.skipBytes(2); // Weapon subclass, casting speed
	packet.skipBytes(4); // Ticks
	int32_t mapmobid = packet.get<int32_t>();
	Mob *mob = Maps::getMap(player->getMap())->getMob(mapmobid);
	if (mob == 0)
		return;
	packet.skipBytes(14); // ???
	int32_t damage = packet.get<int32_t>();
	mob->applyDamage(player->getId(), damage);
	packet.skipBytes(8); // End of packet	
}

void PlayerHandler::useSummonAttack(Player *player, PacketReader &packet) {
	PlayersPacket::useSummonAttack(player, packet);
	packet.reset(2);
	packet.skipBytes(4); // Summon ID
	Summon *summon = player->getSummons()->getSummon();
	if (summon == 0) {
		// Hacking or some other form of tomfoolery
		return;
	}
	packet.skipBytes(5);
	int8_t targets = packet.get<int8_t>();
	int32_t useless = 0;
	damageMobs(player, packet, targets, 1, summon->getSummonId(), useless);
}

uint32_t PlayerHandler::damageMobs(Player *player, PacketReader &packet, int8_t targets, int8_t hits, int32_t skillid, int32_t &extra, MpEaterInfo *eater) {
	int32_t map = player->getMap();
	uint32_t total = 0;
	int32_t firsthit = 0;
	uint8_t level = player->getSkills()->getSkillLevel(skillid);
	for (int8_t i = 0; i < targets; i++) {
		int32_t targettotal = 0;
		int32_t mapmobid = packet.get<int32_t>();
		int8_t connectedhits = 0;
		Mob *mob = Maps::getMap(map)->getMob(mapmobid);
		if (mob == 0)
			return 0;
		if (skillid == Jobs::Cleric::Heal && !mob->isUndead()) {
			// Hacking
			return 0;
		}
		packet.skipBytes(3); // Useless
		packet.skipBytes(1); // State
		packet.skipBytes(8); // Useless
		packet.skipBytes(2); // Distance
		for (int8_t k = 0; k < hits; k++) {
			int32_t damage = packet.get<int32_t>();
			targettotal += damage;
			if (damage != 0)
				connectedhits++;
			if (firsthit == 0)
				firsthit = damage;
			if (mob == 0) {
				packet.skipBytes(4 * (hits - 1 - k));
				break;
			}
			extra = mob->getMaxHp();
			if (eater != 0 && damage != 0 && !eater->onlyonce) { // MP Eater
				mob->mpEat(player, eater);
			}
			if (skillid == Jobs::Ranger::MortalBlow || skillid == Jobs::Sniper::MortalBlow) {
				SkillLevelInfo *sk = player->getSkills()->getSkillInfo(skillid);
				int32_t hp_p = mob->getMaxHp() * sk->x / 100; // Percentage of HP required for Mortal Blow activation
				if ((mob->getHp() < hp_p) && (Randomizer::Instance()->randShort(99) < sk->y)) {
					damage = mob->getHp();
				}
			}
			else if (skillid == Jobs::Outlaw::HomingBeacon || skillid == Jobs::Corsair::Bullseye) {
				Buffs::addBuff(player, skillid, level, 0, mapmobid);
			}
			int32_t temphp = mob->getHp();
			mob->applyDamage(player->getId(), damage);
			if (temphp - damage <= 0) // Mob was killed, so set the Mob pointer to 0
				mob = 0;
		}
		if (mob != 0 && targettotal > 0 && mob->getHp() > 0) {
			MobHandler::handleMobStatus(player->getId(), mob, skillid, level, player->getInventory()->getEquippedId(EquipSlots::Weapon), connectedhits, firsthit); // Mob status handler (freeze, stun, etc)
			if (mob->getHp() < mob->getSelfDestructHp()) {
				mob->explode();
			}
		}
		total += targettotal;
		if (!GameLogicUtilities::isSummon(skillid))
			packet.skipBytes(4); // 4 bytes of unknown purpose, new in .56
	}
	packet.skipBytes(4); // Character positioning, end of packet, might eventually be useful for hacking detection
	return total;
}

