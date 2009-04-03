/*
Copyright (C) 2008-2009 Vana Development Team

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
#include "Mobs.h"
#include "Drops.h"
#include "DropsPacket.h"
#include "GameConstants.h"
#include "Instance.h"
#include "Inventory.h"
#include "Levels.h"
#include "Maps.h"
#include "MobsPacket.h"
#include "Movement.h"
#include "PacketCreator.h"
#include "PacketReader.h"
#include "Randomizer.h"
#include "Skills.h"
#include "SkillsPacket.h"
#include "Summons.h"
#include "Timer/Time.h"
#include "Timer/Timer.h"
#include <functional>

using std::tr1::bind;

// Mob status stuff
const int32_t Mobs::mobstatuses[19] = {WATK, WDEF, MATK, MDEF, ACC, AVOID, SPEED, STUN, FREEZE, POISON,
	SEAL, WEAPON_ATTACK_UP, WEAPON_DEFENSE_UP, MAGIC_ATTACK_UP, MAGIC_DEFENSE_UP, DOOM, SHADOW_WEB,
	WEAPON_IMMUNITY, MAGIC_IMMUNITY};

/* Mob class */
Mob::Mob(int32_t id, int32_t mapid, int32_t mobid, Pos pos, int32_t spawnid, int16_t fh) :
MovableLife(fh, pos, 2),
originfh(fh),
id(id),
mapid(mapid),
spawnid(spawnid),
mobid(mobid),
status(0),
info(MobDataProvider::Instance()->getMobInfo(mobid)),
timers(new Timer::Container),
control(0)
{
	this->hp = info.hp;
	this->mp = info.mp;
	Instance *instance = Maps::getMap(mapid)->getInstance();
	if (instance != 0) {
		instance->sendMessage(MOB_SPAWN, mobid, id);
	}
}

void Mob::applyDamage(int32_t playerid, int32_t damage, bool poison) {
	if (damage > hp)
		damage = hp - poison; // Keep HP from hitting 0 for poison and from going below 0

	damages[playerid] += damage;
	hp -= damage;

	if (!poison) { // HP bar packet does nothing for showing damage when poison is damaging for whatever reason
		Player *player = Players::Instance()->getPlayer(playerid);

		if (info.hpcolor > 0) // Boss HP bars - Horntail's damage sponge isn't a boss in the data
			MobsPacket::showBossHP(player, mobid, hp, info);
		else { // Normal/Miniboss HP bars
			uint8_t percent = static_cast<uint8_t>(hp * 100 / info.hp);
			MobsPacket::showHP(player, id, percent, info.boss);
		}

		if (hp == 0) { // Time to die
			if (mobid == 8810018) { // Horntail damage sponge, we want to be sure that his parts have spawned after death before we clean up
				new Timer::Timer(bind(&Mob::cleanHorntail, this, mapid, player),
					Timer::Id(Timer::Types::HorntailTimer, id, 0),
					0, Timer::Time::fromNow(100));
			}
			die(Players::Instance()->getPlayer(playerid));
		}
	}
}

void Mob::addStatus(int32_t playerid, vector<StatusInfo> statusinfo) {
	for (size_t i = 0; i < statusinfo.size(); i++) {
		if (statusinfo[i].status == POISON && statuses.find(POISON) != statuses.end()) {
			continue; // Already poisoned, so do not poison again
		}
		statuses[statusinfo[i].status] = statusinfo[i];
		MobsPacket::applyStatus(this, statusinfo[i], 300);

		if (statusinfo[i].status == POISON) { // Damage timer for poison
			new Timer::Timer(bind(&Mob::applyDamage, this, playerid, statusinfo[i].val, true),
				Timer::Id(Timer::Types::MobStatusTimer, POISON, 1),
				getTimers(), 0, 1000);
		}

		new Timer::Timer(bind(&Mob::removeStatus, this, statusinfo[i].status),
			Timer::Id(Timer::Types::MobStatusTimer, statusinfo[i].status, 0),
			getTimers(), Timer::Time::fromNow(statusinfo[i].time * 1000));
	}
	// Calculate new status mask
	this->status = 0;
	for (unordered_map<int32_t, StatusInfo>::iterator iter = statuses.begin(); iter != statuses.end(); iter++) {
		this->status += iter->first;
	}
}

void Mob::statusPacket(PacketCreator &packet) {
	if (status > 0) {
		packet.add<int32_t>(status);

		for (uint8_t i = 0; i < 19; i++) { // Val/skillid pairs must be ordered in the packet by status value ascending
			int32_t status = Mobs::mobstatuses[i];
			if (statuses.find(status) != statuses.end()) {
				packet.add<int16_t>(statuses[status].val);
				if (statuses[status].skillid >= 0) {
					packet.add<int32_t>(statuses[status].skillid);
				}
				else {
					packet.add<int16_t>(statuses[status].mobskill);
					packet.add<int16_t>(statuses[status].level);
				}
				packet.add<int16_t>(0);
			}
		}
	}
	else {
		packet.add<int16_t>(0);
		packet.add<int16_t>(0x800);
	}
}

void Mob::removeStatus(int32_t status) {
	this->status -= status;
	statuses.erase(status);
	if (status == POISON) // Stop poison damage timer
		getTimers()->removeTimer(Timer::Id(Timer::Types::MobStatusTimer, status, 1));
	MobsPacket::removeStatus(this, status);
}

void Mob::setControl(Player *control) {
	/*if (this->control != 0)
		MobsPacket::endControlMob(this->control, this);*/
	this->control = control;
	if (control != 0)
		MobsPacket::requestControl(control, this, false);
}

void Mob::endControl() {
	if (control != 0 && control->getMap() == getMapID())
		MobsPacket::endControlMob(control, this);
}

void Mob::die(Player *player) {
	endControl();
	int32_t highestdamager = 0;
	uint32_t highestdamage = 0;
	for (unordered_map<int32_t, uint32_t>::iterator iter = damages.begin(); iter != damages.end(); iter++) {
		if (iter->second > highestdamage) { // Find the highest damager to give drop ownership
			highestdamager = iter->first;
			highestdamage = iter->second;
		}
		Player *damager = Players::Instance()->getPlayer(iter->first);
		if (damager == 0 || damager->getMap() != this->mapid) // Only give EXP if the damager is in the same channel and on the same map
			continue;

		uint8_t multiplier = damager == player ? 10 : 8; // Multiplier for player to give the finishing blow is 1 and .8 for others. We therefore set this to 10 or 8 and divide the result in the formula found later on by 10.
		// Account for Holy Symbol
		int16_t hsrate = 0;
		if (damager->getActiveBuffs()->getActiveSkillLevel(Priest::HOLYSYMBOL) > 0)
			hsrate = Skills::skills[Priest::HOLYSYMBOL][damager->getActiveBuffs()->getActiveSkillLevel(Priest::HOLYSYMBOL)].x;
		else if (damager->getActiveBuffs()->getActiveSkillLevel(SuperGM::HOLYSYMBOL) > 0)
			hsrate = Skills::skills[SuperGM::HOLYSYMBOL][damager->getActiveBuffs()->getActiveSkillLevel(SuperGM::HOLYSYMBOL)].x;

		uint32_t exp = (info.exp * (multiplier * iter->second / info.hp)) / 10;
		Levels::giveEXP(damager, (exp + ((exp * hsrate) / 100)) * ChannelServer::Instance()->getExprate(), false, (damager == player));
	}

	// Spawn mob(s) the mob is supposed to spawn when it dies
	for (size_t i = 0; i < info.summon.size(); i++) {
		Maps::getMap(mapid)->spawnMob(info.summon[i], m_pos, -1, 0, this);
	}

	MobsPacket::dieMob(this);
	Drops::doDrops(highestdamager, mapid, mobid, getPos());
	player->getQuests()->updateQuestMob(mobid);
	Instance *instance = Maps::getMap(mapid)->getInstance();
	if (instance != 0) {
		instance->sendMessage(MOB_DEATH, mobid, id);
	}
	Maps::getMap(mapid)->removeMob(id, spawnid);

	delete this;
}

void Mob::die(bool showpacket) {
	if (showpacket) {
		endControl();
		MobsPacket::dieMob(this);
		Instance *instance = Maps::getMap(mapid)->getInstance();
		if (instance != 0) {
			instance->sendMessage(MOB_DEATH, mobid, id);
		}
	}
	Maps::getMap(mapid)->removeMob(id, spawnid);
	delete this;
}

void Mob::cleanHorntail(int32_t mapid, Player *player) {
	for (int8_t q = 0; q < 8; q++) {
		Maps::getMap(mapid)->killMobs(player, (8810010 + q)); // Dead Horntail's parts
	}
}

/* Mobs namespace */
void Mobs::monsterControl(Player *player, PacketReader &packet) {
	int32_t mobid = packet.get<int32_t>();

	Mob *mob = Maps::getMap(player->getMap())->getMob(mobid);

	if (mob == 0) {
		return;
	}

	int16_t moveid = packet.get<int16_t>();
	bool useskill = (packet.get<int8_t>() != 0);
	int32_t skill = packet.get<int32_t>();
	packet.skipBytes(10);
	Pos cpos = Movement::parseMovement(mob, packet);
	if (cpos - mob->getPos() > 300) {
		if (player->addWarning())
			return;
	}
	MobsPacket::moveMobResponse(player, mobid, moveid, useskill, mob->getMP());
	packet.reset(19);
	MobsPacket::moveMob(player, mobid, useskill, skill, packet.getBuffer(), packet.getBufferLength());
}

void Mobs::damageMob(Player *player, PacketReader &packet) {
	MobsPacket::damageMob(player, packet);
	packet.reset(2);
	packet.skipBytes(1); // Useless
	uint8_t tbyte = packet.get<int8_t>();
	int8_t targets = tbyte / 0x10;
	if (player->getSkills()->getSkillLevel(Marauder::ENERGYCHARGE) > 0)
		player->getActiveBuffs()->increaseEnergyChargeLevel(targets);
	int8_t hits = tbyte % 0x10;
	int32_t skillid = packet.get<int32_t>();
	switch (skillid) {
		case Gunslinger::GRENADE:
		case Infighter::CORKSCREWBLOW:
			packet.skipBytes(4); // Charge
			break;
	}
	packet.skipBytes(8); // In order: Display [1], Animation [1], Weapon subclass [1], Weapon speed [1], Tick count [4]
	if (skillid > 0)
		Skills::useAttackSkill(player, skillid);
	int32_t useless = 0;
	uint32_t totaldmg = damageMobInternal(player, packet, targets, hits, skillid, useless, 0, true);
	switch (skillid) {
		case ChiefBandit::MESOEXPLOSION: { // Meso Explosion
			uint8_t items = packet.get<int8_t>();
			int32_t map = player->getMap();
			for (uint8_t i = 0; i < items; i++) {
				int32_t objID = packet.get<int32_t>();
				packet.skipBytes(1); // Boolean for hit a monster
				Drop *drop = Maps::getMap(map)->getDrop(objID);
				if (drop != 0) {
					DropsPacket::explodeDrop(drop);
					Maps::getMap(map)->removeDrop(drop->getID());
					delete drop;
				}
			}
			break;
		}
		case Crusader::SWORDPANIC: // Crusader finishers
		case Crusader::SWORDCOMA:
		case Crusader::AXEPANIC:
		case Crusader::AXECOMA:
			player->getActiveBuffs()->setCombo(0, true);
			break;
		case Crusader::SHOUT: // Shout
		case GM::SUPERDRAGONROAR: // Super Dragon Roar
			break;
		case DragonKnight::DRAGONROAR: { // Dragon Roar
			int8_t roarlv = player->getSkills()->getSkillLevel(skillid);
			int16_t x_value = Skills::skills[skillid][roarlv].x;
			int16_t y_value = Skills::skills[skillid][roarlv].y; // Stun length in seconds
			uint16_t reduction = (player->getMHP() / 100) * x_value;
			if ((player->getHP() - reduction) > 0)
				player->damageHP(reduction);
			else {
				// Hacking
				return;
			}
			// TODO: Add stun here
			break;
		}
		case DragonKnight::SACRIFICE: { // Sacrifice
			int16_t hp_damage_x = Skills::skills[skillid][player->getSkills()->getSkillLevel(skillid)].x;
			uint16_t hp_damage = (uint16_t) totaldmg * hp_damage_x / 100;
			if ((player->getHP() - hp_damage) < 1)
				player->setHP(1);
			else
				player->damageHP(hp_damage);
			break;
		}
		case WhiteKnight::CHARGEBLOW: { // Charge Blow
			int8_t acb_level = player->getSkills()->getSkillLevel(skillid);
			int16_t acb_x = 0;
			if (acb_level > 0)
				acb_x = Skills::skills[skillid][acb_level].x;
			int32_t charge_id = 0;
			if (player->getActiveBuffs()->getActiveSkillLevel(WhiteKnight::SWORDFIRECHARGE) > 0) // Fire - Sword
				charge_id = WhiteKnight::SWORDFIRECHARGE;
			else if (player->getActiveBuffs()->getActiveSkillLevel(WhiteKnight::BWFIRECHARGE) > 0) // Fire - BW
				charge_id = WhiteKnight::BWFIRECHARGE;
			else if (player->getActiveBuffs()->getActiveSkillLevel(WhiteKnight::SWORDICECHARGE) > 0) // Ice - Sword
				charge_id = WhiteKnight::SWORDICECHARGE;
			else if (player->getActiveBuffs()->getActiveSkillLevel(WhiteKnight::BWICECHARGE) > 0) // Ice - BW
				charge_id = WhiteKnight::BWICECHARGE;
			else if (player->getActiveBuffs()->getActiveSkillLevel(WhiteKnight::SWORDLITCHARGE) > 0) // Lightning - Sword
				charge_id = WhiteKnight::SWORDLITCHARGE;
			else if (player->getActiveBuffs()->getActiveSkillLevel(WhiteKnight::BWLITCHARGE) > 0) // Lightning - BW
				charge_id = WhiteKnight::BWLITCHARGE;
			else if (player->getActiveBuffs()->getActiveSkillLevel(Paladin::SWORDHOLYCHARGE) > 0) // Holy - Sword
				charge_id = Paladin::SWORDHOLYCHARGE;
			else if (player->getActiveBuffs()->getActiveSkillLevel(Paladin::BWHOLYCHARGE) > 0) // Holy - BW
				charge_id = Paladin::BWHOLYCHARGE;
			if (charge_id == 0) {
				// Hacking
				return;
			}
			if ((acb_x != 100) && (acb_x == 0 || Randomizer::Instance()->randShort(99) > (acb_x - 1)))
				Skills::stopSkill(player, charge_id);
			break;
		}
		default:
			if (totaldmg > 0)
				player->getActiveBuffs()->addCombo();
			break;
	}
}

void Mobs::damageMobRanged(Player *player, PacketReader &packet) {
	MobsPacket::damageMobRanged(player, packet);
	packet.reset(2); // Passing to the display function causes the buffer to be eaten, we need it
	packet.skipBytes(1); // Number of portals taken (not kidding)
	uint8_t tbyte = packet.get<int8_t>();
	int8_t targets = tbyte / 0x10;
	int8_t hits = tbyte % 0x10;
	int32_t skillid = packet.get<int32_t>();
	uint8_t display = 0;
	switch (skillid) {
		case Bowmaster::HURRICANE:
		case Marksman::PIERCINGARROW:
		case Corsair::RAPIDFIRE:
			packet.skipBytes(4); // Charge time
			display = packet.get<int8_t>();
			if ((skillid == Bowmaster::HURRICANE || skillid == Corsair::RAPIDFIRE) && player->getSpecialSkill() == 0) { // Only Hurricane constantly does damage and display it if not displayed
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
			display = packet.get<int8_t>(); // Projectile display
			packet.skipBytes(1); // Direction/animation
			packet.skipBytes(1); // Weapon subclass
			packet.skipBytes(1); // Weapon speed
			break;
	}
	packet.skipBytes(4); // Ticks
	int16_t pos = packet.get<int16_t>();
	packet.skipBytes(2); // Cash Shop star cover
	packet.skipBytes(1); // 0x00 = AoE, 0x41 = other
	if (skillid != Hermit::SHADOWMESO && ((display & 0x40) > 0))
		packet.skipBytes(4); // Star ID added by Shadow Claw
	Skills::useAttackSkillRanged(player, skillid, pos, display);
	int32_t mhp = 0;
	uint32_t totaldmg = damageMobInternal(player, packet, targets, hits, skillid, mhp);
	if (skillid == Assassin::DRAIN) { // Drain
		int16_t drain_x = Skills::skills[4101005][player->getSkills()->getSkillLevel(4101005)].x;
		int32_t hpRecover = totaldmg * drain_x / 100;
		if (hpRecover > mhp)
			hpRecover = mhp;
		if (hpRecover > (player->getMHP() / 2))
			hpRecover = player->getMHP() / 2;
		if (hpRecover > player->getMHP())
			player->setHP(player->getMHP());
		else
			player->modifyHP((int16_t) hpRecover);
	}
}

void Mobs::damageMobSpell(Player *player, PacketReader &packet) {
	MobsPacket::damageMobSpell(player, packet);
	packet.reset(2);
	packet.skipBytes(1);
	uint8_t tbyte = packet.get<int8_t>();
	int8_t targets = tbyte / 0x10;
	int8_t hits = tbyte % 0x10;
	int32_t skillid = packet.get<int32_t>();
	switch (skillid) {
		case FPArchMage::BIGBANG: // Big Bang has a 4 byte charge time after skillid
		case ILArchMage::BIGBANG:
		case Bishop::BIGBANG:
			packet.skipBytes(4);
			break;
	}
	MPEaterInfo eater;
	eater.id = (player->getJob() / 10) * 100000;
	eater.level = player->getSkills()->getSkillLevel(eater.id);
	if (eater.level > 0) {
		eater.prop = Skills::skills[eater.id][eater.level].prop;
		eater.x = Skills::skills[eater.id][eater.level].x;
	}
	packet.skipBytes(2); // Display, direction/animation
	packet.skipBytes(2); // Weapon subclass, casting speed
	packet.skipBytes(4); // Ticks
	if (skillid != Cleric::HEAL) // Heal is sent as both an attack and as a use skill
		// Prevent this from incurring cost since Heal is always a used skill but only an attack in certain circumstances
		Skills::useAttackSkill(player, skillid);
	int32_t useless = 0;
	uint32_t totaldmg = damageMobInternal(player, packet, targets, hits, skillid, useless, &eater);
}

void Mobs::damageMobEnergyCharge(Player *player, PacketReader &packet) {
	MobsPacket::damageMobEnergyCharge(player, packet);
	packet.reset(2);
	packet.skipBytes(1);
	uint8_t tbyte = packet.get<int8_t>();
	int8_t targets = tbyte / 0x10;
	int8_t hits = tbyte % 0x10;
	int32_t skillid = packet.get<int32_t>();
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

void Mobs::damageMobSummon(Player *player, PacketReader &packet) {
	MobsPacket::damageMobSummon(player, packet);
	packet.reset(2);
	packet.skipBytes(4); // Summon ID
	Summon *summon = player->getSummons()->getSummon();
	if (summon == 0)
		// Hacking or some other form of tomfoolery
		return;
	packet.skipBytes(5);
	int8_t targets = packet.get<int8_t>();
	int32_t useless = 0;
	damageMobInternal(player, packet, targets, 1, summon->getSummonID(), useless);
}

uint32_t Mobs::damageMobInternal(Player *player, PacketReader &packet, int8_t targets, int8_t hits, int32_t skillid, int32_t &extra, MPEaterInfo *eater, bool ismelee) {
	int32_t map = player->getMap();
	uint32_t total = 0;
	uint8_t pplevel = player->getActiveBuffs()->getActiveSkillLevel(ChiefBandit::PICKPOCKET); // Check for active pickpocket level
	for (int8_t i = 0; i < targets; i++) {
		int32_t mapmobid = packet.get<int32_t>();
		Mob *mob = Maps::getMap(map)->getMob(mapmobid);
		if (mob == 0)
			return 0;
		uint8_t weapontype = (uint8_t) HelperFunctions::getItemType(player->getInventory()->getEquippedID(11));
		handleMobStatus(player, mob, skillid, weapontype); // Mob status handler (freeze, stun, etc)
		int32_t mobid = mob->getMobID();
		Mob *htabusetaker = 0;
		switch (mobid) {
			case 8810002:
			case 8810003:
			case 8810004:
			case 8810005:
			case 8810006:
			case 8810007:
			case 8810008:
			case 8810009:
				htabusetaker = Maps::getMap(map)->getMob(8810018, false);
				break;
		}
		packet.skipBytes(3); // Useless
		packet.skipBytes(1); // State
		packet.skipBytes(8); // Useless
		if (skillid != ChiefBandit::MESOEXPLOSION)
			packet.skipBytes(1); // Distance, first half for non-Meso Explosion
		int8_t num = packet.get<int8_t>(); // Distance, second half for non-Meso Explosion OR hits for Meso Explosion
		hits = skillid == ChiefBandit::MESOEXPLOSION ? num : hits;
		Pos origin = mob->getPos(); // Info for
		vector<int32_t> ppdamages; // Pickpocket
		for (int8_t k = 0; k < hits; k++) {
			int32_t damage = packet.get<int32_t>();
			total += damage;
			if (ismelee && skillid != ChiefBandit::MESOEXPLOSION && pplevel > 0) { // Make sure this is a melee attack and not meso explosion, plus pickpocket being active
				if (Randomizer::Instance()->randInt(99) < Skills::skills[ChiefBandit::PICKPOCKET][pplevel].prop) {
					ppdamages.push_back(damage);
				}
			}
			if (mob == 0) {
				if (ismelee && skillid != ChiefBandit::MESOEXPLOSION && pplevel > 0) // Roll along after the mob is dead to finish getting damage values for pickpocket
					continue;
				else {
					packet.skipBytes(4 * (hits - 1 - k));
					break;
				}
			}

			extra = mob->getMHP();
			if (eater != 0) { // MP Eater
				int32_t cmp = mob->getMP();
				if ((!eater->onlyonce) && (damage != 0) && (cmp > 0) && (Randomizer::Instance()->randInt(99) < eater->prop)) {
					eater->onlyonce = true;
					int32_t mp = mob->getMMP() * eater->x / 100;
					if (mp > cmp)
						mp = cmp;
					mob->setMP(cmp - mp);
					player->modifyMP((int16_t) mp);
					SkillsPacket::showSkillEffect(player, eater->id);
				}
			}

			if (skillid == Paladin::HEAVENSHAMMER && mob->isBoss()) {
				// Damage calculation goes in here, I think? Hearing conflicted views.
			}
			else {
				if (skillid == Paladin::HEAVENSHAMMER)
					damage = mob->getHP() - 1;

				int32_t temphp = mob->getHP();
				mob->applyDamage(player->getId(), damage);

				if (htabusetaker != 0) {
					if (temphp - damage < 0) // Horntail will die before all of his parts otherwise
						damage = temphp; // Damage isn't used again from here on anyway
					htabusetaker->applyDamage(player->getId(), damage);
				}
				if (temphp - damage <= 0) // Mob was killed, so set the Mob pointer to 0
					mob = 0;
			}
		}
		for (uint8_t pp = 0; pp < (uint8_t) ppdamages.size(); pp++) { // Drop stuff for Pickpocket
			Pos pos;
			pos.x = origin.x - 25 * ((ppdamages.size() - pp) / 2);
			pos.y = origin.y;
			clock_t time = 150 * pp;
			int32_t mesos = ((ppdamages[pp] * Skills::skills[ChiefBandit::PICKPOCKET][pplevel].x) / 10000); // TODO: Check on this formula in different situations
			Drop *drop = new Drop(player->getMap(), mesos, pos, player->getId(), true);
			drop->setTime(100);
			new Timer::Timer(bind(&Drop::doDrop, drop, origin),
				Timer::Id(Timer::Types::BuffTimer, ChiefBandit::PICKPOCKET, pp),
				0, Timer::Time::fromNow(time));
		}
		if (!HelperFunctions::isSummon(skillid))
			packet.skipBytes(4); // 4 bytes of unknown purpose, new in .56
	}
	packet.skipBytes(4); // Character positioning, end of packet, might eventually be useful for hacking detection
	return total;
}

void Mobs::handleMobStatus(Player *player, Mob *mob, int32_t skillid, uint8_t weapon_type) {
	uint8_t level = skillid > 0 ? player->getSkills()->getSkillLevel(skillid) : 0;
	vector<StatusInfo> statuses;
	if (mob->canFreeze()) { // Freezing stuff
		switch (skillid) {
			case ILWizard::COLDBEAM: // Cold Beam
			case ILMage::ICESTRIKE: // Ice Strike
			case ILMage::ELEMENTCOMPOSITION: // Element Composition
			case Sniper::BLIZZARD: // Blizzard (Sniper)
			case ILArchMage::BLIZZARD: // Blizzard (Arch Mage)
			case Outlaw::ICESPLITTER: // Cooling Effect
				statuses.push_back(StatusInfo(FREEZE, FREEZE, skillid, Skills::skills[skillid][level].time));
				break;
			case FPArchMage::ELQUINES: // Elquines
			case Marksman::FROSTPREY: // Frostprey
				statuses.push_back(StatusInfo(FREEZE, FREEZE, skillid, Skills::skills[skillid][level].x));
				break;
		}
		if ((weapon_type == WEAPON_1H_SWORD || weapon_type == WEAPON_2H_SWORD) && player->getActiveBuffs()->getActiveSkillLevel(WhiteKnight::SWORDICECHARGE) > 0) { // Ice Charge Sword
			statuses.push_back(StatusInfo(FREEZE, FREEZE, WhiteKnight::SWORDICECHARGE, Skills::skills[WhiteKnight::SWORDICECHARGE][player->getActiveBuffs()->getActiveSkillLevel(WhiteKnight::SWORDICECHARGE)].y));
		}
		else if ((weapon_type == WEAPON_1H_MACE || weapon_type == WEAPON_2H_MACE) && player->getActiveBuffs()->getActiveSkillLevel(WhiteKnight::BWICECHARGE) > 0) { // Blizzard Charge BW
			statuses.push_back(StatusInfo(FREEZE, FREEZE, WhiteKnight::BWICECHARGE, Skills::skills[WhiteKnight::BWICECHARGE][player->getActiveBuffs()->getActiveSkillLevel(WhiteKnight::BWICECHARGE)].y));
		}
	}
	if (mob->canPoison()) { // Poisoning stuff
		if ((skillid == FPWizard::POISONBREATH || skillid == FPMage::POISONMIST || skillid == FPMage::ELEMENTCOMPOSITION) && Randomizer::Instance()->randInt(99) < Skills::skills[skillid][level].prop) { // Poison brace, Element composition, and Poison mist
			int16_t pdamage = (int16_t)(mob->getMHP() / (70 - level));
			statuses.push_back(StatusInfo(POISON, pdamage, skillid, Skills::skills[skillid][level].time));
		}
	}
	if (!mob->isBoss()) { // Seal, Stun, etc
		switch (skillid) {
			case Hunter::ARROWBOMB: // Arrow Bomb
			case Crusader::SWORDCOMA: // Coma: Sword
			case Crusader::AXECOMA: // Coma: Axe
			case Crusader::SHOUT: // Shout
			case WhiteKnight::CHARGEBLOW: // Charged Blow
			case ChiefBandit::ASSAULTER: // Assaulter
			case Shadower::BOOMERANGSTEP: // Boomerang Step
			case Gunslinger::BLANKSHOT: // Fake Shot
				if (Randomizer::Instance()->randInt(99) < Skills::skills[skillid][level].prop) {
					statuses.push_back(StatusInfo(STUN, STUN, skillid, Skills::skills[skillid][level].time));
				}
				break;
			case Infighter::BACKSPINBLOW: // Backspin Blow
			case Infighter::DOUBLEUPPERCUT: // Double Uppercut
			case Buccaneer::DEMOLITION: // Demolition
			case Buccaneer::SNATCH: // Snatch
				statuses.push_back(StatusInfo(STUN, STUN, skillid, Skills::skills[skillid][level].time));
				break;
			case Ranger::SILVERHAWK: // Silver Hawk
			case Sniper::GOLDENEAGLE: // Golden Eagle
				if (Randomizer::Instance()->randInt(99) < Skills::skills[skillid][level].prop) {
					statuses.push_back(StatusInfo(STUN, STUN, skillid, Skills::skills[skillid][level].x));
				}
				break;
			case FPMage::SEAL: // Seal - F/P
			case ILMage::SEAL: // Seal - I/L
				if (Randomizer::Instance()->randInt(99) < Skills::skills[skillid][level].prop) {
					statuses.push_back(StatusInfo(STUN, STUN, skillid, Skills::skills[skillid][level].time));
				}
				break;
			case Priest::DOOM: // Doom
				if (Randomizer::Instance()->randInt(99) < Skills::skills[skillid][level].prop) {
					statuses.push_back(StatusInfo(DOOM, 0x100, skillid, Skills::skills[skillid][level].time));
				}
				break;
			case Hermit::SHADOWWEB: // Shadow Web
				if (Randomizer::Instance()->randInt(99) < Skills::skills[skillid][level].prop) {
					statuses.push_back(StatusInfo(SHADOW_WEB, 0x100, skillid, Skills::skills[skillid][level].time));
				}
				break;
		}
	}
	if (skillid == Rogue::DISORDER) { // Disorder
		clock_t time = Skills::skills[skillid][level].time;
		statuses.push_back(StatusInfo(WATK, Skills::skills[skillid][level].x, skillid, time));
		statuses.push_back(StatusInfo(WDEF, Skills::skills[skillid][level].y, skillid, time));
	}
	else if (skillid == Page::THREATEN) { // Threaten
		clock_t time = Skills::skills[skillid][level].time;
		statuses.push_back(StatusInfo(WATK, Skills::skills[skillid][level].x, skillid, time));
		statuses.push_back(StatusInfo(WDEF, Skills::skills[skillid][level].y, skillid, time));
	}
	else if (skillid == FPWizard::SLOW || skillid == ILWizard::SLOW) { // Slow
		statuses.push_back(StatusInfo(SPEED, Skills::skills[skillid][level].x, skillid, Skills::skills[skillid][level].time));
	}
	if (weapon_type == WEAPON_BOW && player->getActiveBuffs()->getActiveSkillLevel(Bowmaster::HAMSTRING) > 0) { // Hamstring
		uint8_t hamlevel = player->getActiveBuffs()->getActiveSkillLevel(Bowmaster::HAMSTRING);
		statuses.push_back(StatusInfo(SPEED, Skills::skills[Bowmaster::HAMSTRING][hamlevel].x, Bowmaster::HAMSTRING, Skills::skills[Bowmaster::HAMSTRING][hamlevel].y));
	}
	if (weapon_type == WEAPON_CROSSBOW && player->getActiveBuffs()->getActiveSkillLevel(Marksman::BLIND) > 0) { // Blind
		uint8_t blindlevel = player->getActiveBuffs()->getActiveSkillLevel(Marksman::BLIND);
		statuses.push_back(StatusInfo(ACC, -Skills::skills[Marksman::BLIND][blindlevel].x, Marksman::BLIND, Skills::skills[Marksman::BLIND][blindlevel].y));
	}

	if (statuses.size() > 0)
		mob->addStatus(player->getId(), statuses);
}
