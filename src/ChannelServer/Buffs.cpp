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
#include "Buffs.h"
#include "BuffsPacket.h"
#include "GameConstants.h"
#include "GameLogicUtilities.h"
#include "Inventory.h"
#include "Player.h"
#include "SkillDataProvider.h"
#include "Skills.h"

Buffs * Buffs::singleton = 0;

Buffs::Buffs() {
	Buff buff;
	BuffAct act;
	BuffInfo player;
	BuffMapInfo map;
	MobAilmentInfo mob;

	// Boosters
	buff.type = 0x08;
	buff.byte = Byte2;
	buff.value = SkillX;
	player.buff = buff;
	skillsinfo[Jobs::Fighter::SwordBooster].player.push_back(player);
	skillsinfo[Jobs::Fighter::AxeBooster].player.push_back(player);
	skillsinfo[Jobs::Page::SwordBooster].player.push_back(player);
	skillsinfo[Jobs::Page::BwBooster].player.push_back(player);
	skillsinfo[Jobs::Spearman::SpearBooster].player.push_back(player);
	skillsinfo[Jobs::Spearman::PolearmBooster].player.push_back(player);
	skillsinfo[Jobs::FPMage::SpellBooster].player.push_back(player);
	skillsinfo[Jobs::ILMage::SpellBooster].player.push_back(player);
	skillsinfo[Jobs::Hunter::BowBooster].player.push_back(player);
	skillsinfo[Jobs::Crossbowman::CrossbowBooster].player.push_back(player);
	skillsinfo[Jobs::Assassin::ClawBooster].player.push_back(player);
	skillsinfo[Jobs::Bandit::DaggerBooster].player.push_back(player);
	skillsinfo[Jobs::Brawler::KnucklerBooster].player.push_back(player);
	skillsinfo[Jobs::Gunslinger::GunBooster].player.push_back(player);

	// Speed Infusion
	buff.type = 0x80;
	buff.byte = Byte8;
	buff.value = SkillX;
	player.buff = buff;
	skillsinfo[Jobs::Buccaneer::SpeedInfusion].player.push_back(player);

	// Maple Warriors
	buff.type = 0x08;
	buff.byte = Byte5;
	buff.value = SkillX;
	player.buff = buff;
	skillsinfo[Jobs::Hero::MapleWarrior].player.push_back(player);
	skillsinfo[Jobs::Paladin::MapleWarrior].player.push_back(player);
	skillsinfo[Jobs::DarkKnight::MapleWarrior].player.push_back(player);
	skillsinfo[Jobs::FPArchMage::MapleWarrior].player.push_back(player);
	skillsinfo[Jobs::ILArchMage::MapleWarrior].player.push_back(player);
	skillsinfo[Jobs::Bishop::MapleWarrior].player.push_back(player);
	skillsinfo[Jobs::Bowmaster::MapleWarrior].player.push_back(player);
	skillsinfo[Jobs::Marksman::MapleWarrior].player.push_back(player);
	skillsinfo[Jobs::NightLord::MapleWarrior].player.push_back(player);
	skillsinfo[Jobs::Shadower::MapleWarrior].player.push_back(player);
	skillsinfo[Jobs::Buccaneer::MapleWarrior].player.push_back(player);
	skillsinfo[Jobs::Corsair::MapleWarrior].player.push_back(player);

	// Magic Guard
	buff.type = 0x02;
	buff.byte = Byte2;
	buff.value = SkillX;
	player.buff = buff;
	skillsinfo[Jobs::Magician::MagicGuard].player.push_back(player);

	// Magic Armor, Iron Body
	buff.type = 0x02;
	buff.byte = Byte1;
	buff.value = SkillWdef;
	player.buff = buff;
	skillsinfo[Jobs::Magician::MagicArmor].player.push_back(player);
	skillsinfo[Jobs::Swordsman::IronBody].player.push_back(player);

	// Focus
	buff.type = 0x10;
	buff.byte = Byte1;
	buff.value = SkillAcc;
	player.buff = buff;
	skillsinfo[Jobs::Archer::Focus].player.push_back(player);
	buff.type = 0x20;
	buff.byte = Byte1;
	buff.value = SkillAvo;
	player.buff = buff;
	skillsinfo[Jobs::Archer::Focus].player.push_back(player);

	// Rage
	buff.type = 0x01;
	buff.byte = Byte1;
	buff.value = SkillWatk;
	player.buff = buff;
	skillsinfo[Jobs::Fighter::Rage].player.push_back(player);
	buff.type = 0x02;
	buff.byte = Byte1;
	buff.value = SkillWdef;
	player.buff = buff;
	skillsinfo[Jobs::Fighter::Rage].player.push_back(player);

	// Power Guard
	buff.type = 0x10;
	buff.byte = Byte2;
	buff.value = SkillX;
	player.buff = buff;
	skillsinfo[Jobs::Fighter::PowerGuard].player.push_back(player);
	skillsinfo[Jobs::Page::PowerGuard].player.push_back(player);

	// Iron Will
	buff.type = 0x02;
	buff.byte = Byte1;
	buff.value = SkillWdef;
	player.buff = buff;
	skillsinfo[Jobs::Spearman::IronWill].player.push_back(player);
	buff.type = 0x08;
	buff.byte = Byte1;
	buff.value = SkillMdef;
	player.buff = buff;
	skillsinfo[Jobs::Spearman::IronWill].player.push_back(player);

	// Hyper Body, Gm Hyper Body
	buff.type = 0x20;
	buff.byte = Byte2;
	buff.value = SkillX;
	player.buff = buff;
	skillsinfo[Jobs::Spearman::HyperBody].player.push_back(player);
	skillsinfo[Jobs::SuperGm::HyperBody].player.push_back(player);
	buff.type = 0x40;
	buff.byte = Byte2;
	buff.value = SkillY;
	player.buff = buff;
	skillsinfo[Jobs::Spearman::HyperBody].player.push_back(player);
	skillsinfo[Jobs::SuperGm::HyperBody].player.push_back(player);

	// Meditation
	buff.type = 0x04;
	buff.byte = Byte1;
	buff.value = SkillMatk;
	player.buff = buff;
	skillsinfo[Jobs::FPWizard::Meditation].player.push_back(player);
	skillsinfo[Jobs::ILWizard::Meditation].player.push_back(player);

	// Invincible
	buff.type = 0x80;
	buff.byte = Byte2;
	buff.value = SkillX;
	player.buff = buff;
	skillsinfo[Jobs::Cleric::Invincible].player.push_back(player);

	// Bless, Gm Bless
	buff.type = 0x01;
	buff.byte = Byte1;
	buff.value = SkillWatk;
	player.buff = buff;
	skillsinfo[Jobs::SuperGm::Bless].player.push_back(player);
	buff.type = 0x02;
	buff.byte = Byte1;
	buff.value = SkillWdef;
	player.buff = buff;
	skillsinfo[Jobs::Cleric::Bless].player.push_back(player);
	skillsinfo[Jobs::SuperGm::Bless].player.push_back(player);
	buff.type = 0x04;
	buff.byte = Byte1;
	buff.value = SkillMatk;
	player.buff = buff;
	skillsinfo[Jobs::SuperGm::Bless].player.push_back(player);
	buff.type = 0x08;
	buff.byte = Byte1;
	buff.value = SkillMdef;
	player.buff = buff;
	skillsinfo[Jobs::Cleric::Bless].player.push_back(player);
	skillsinfo[Jobs::SuperGm::Bless].player.push_back(player);
	buff.type = 0x10;
	buff.byte = Byte1;
	buff.value = SkillAcc;
	player.buff = buff;
	skillsinfo[Jobs::Cleric::Bless].player.push_back(player);
	skillsinfo[Jobs::SuperGm::Bless].player.push_back(player);
	buff.type = 0x20;
	buff.byte = Byte1;
	buff.value = SkillAvo;
	player.buff = buff;
	skillsinfo[Jobs::Cleric::Bless].player.push_back(player);
	skillsinfo[Jobs::SuperGm::Bless].player.push_back(player);

	// Gm Hide
	buff.type = 0x01;
	buff.byte = Byte8;
	buff.value = SkillSpecialProc;
	player.buff = buff;
	skillsinfo[Jobs::SuperGm::Hide].player.push_back(player);

	// Meso Guard
	buff.type = 0x10;
	buff.byte = Byte4;
	buff.value = SkillX;
	player.buff = buff;
	skillsinfo[Jobs::ChiefBandit::MesoGuard].player.push_back(player);

	// Sharp Eyes
	buff.type = 0x20;
	buff.byte = Byte5;
	buff.value = SkillSpecialProc;
	player.buff = buff;
	skillsinfo[Jobs::Bowmaster::SharpEyes].player.push_back(player);
	skillsinfo[Jobs::Marksman::SharpEyes].player.push_back(player);

	// Holy Symbol
	buff.type = 0x01;
	buff.byte = Byte4;
	buff.value = SkillX;
	player.buff = buff;
	skillsinfo[Jobs::Priest::HolySymbol].player.push_back(player);
	skillsinfo[Jobs::SuperGm::HolySymbol].player.push_back(player);

	// Enrage
	buff.type = 0x01;
	buff.byte = Byte1;
	buff.value = SkillWatk;
	player.buff = buff;
	skillsinfo[Jobs::Hero::Enrage].player.push_back(player);

	// Concentrate
	buff.type = 0x01;
	buff.byte = Byte1;
	buff.value = SkillWatk;
	player.buff = buff;
	skillsinfo[Jobs::Bowmaster::Concentrate].player.push_back(player);
	buff.type = 0x20;
	buff.byte = Byte6;
	buff.value = SkillX;
	player.buff = buff;
	skillsinfo[Jobs::Bowmaster::Concentrate].player.push_back(player);

	// Pickpocket
	buff.type = 0x08;
	buff.byte = Byte4;
	buff.value = SkillX;
	player.buff = buff;
	skillsinfo[Jobs::ChiefBandit::Pickpocket].player.push_back(player);

	// Meso Up
	buff.type = 0x08;
	buff.byte = Byte4;
	buff.value = SkillX;
	player.buff = buff;
	skillsinfo[Jobs::Hermit::MesoUp].player.push_back(player);

	// Infinity
	buff.type = 0x02;
	buff.byte = Byte6;
	buff.value = SkillX;
	player.buff = buff;
	skillsinfo[Jobs::FPArchMage::Infinity].player.push_back(player);
	skillsinfo[Jobs::ILArchMage::Infinity].player.push_back(player);
	skillsinfo[Jobs::Bishop::Infinity].player.push_back(player);

	// Echo of Hero
	buff.type = 0x80;
	buff.byte = Byte6;
	buff.value = SkillX;
	player.buff = buff;
	skillsinfo[Jobs::Beginner::EchoOfHero].player.push_back(player);

	// Power Stance
	buff.type = 0x10;
	buff.byte = Byte5;
	buff.value = SkillProp;
	player.buff = buff;
	skillsinfo[Jobs::Hero::PowerStance].player.push_back(player);
	skillsinfo[Jobs::Paladin::PowerStance].player.push_back(player);
	skillsinfo[Jobs::DarkKnight::PowerStance].player.push_back(player);

	// Mana Reflection
	buff.type = 0x40;
	buff.byte = Byte5;
	buff.value = SkillLv;
	player.buff = buff;
	skillsinfo[Jobs::FPArchMage::ManaReflection].player.push_back(player);
	skillsinfo[Jobs::ILArchMage::ManaReflection].player.push_back(player);
	skillsinfo[Jobs::Bishop::ManaReflection].player.push_back(player);

	// Hamstring
	buff.type = 0x08;
	buff.byte = Byte6;
	buff.value = SkillX;
	player.buff = buff;
	skillsinfo[Jobs::Bowmaster::Hamstring].player.push_back(player);

	// Blind
	buff.type = 0x10;
	buff.byte = Byte6;
	buff.value = SkillX;
	player.buff = buff;
	skillsinfo[Jobs::Marksman::Blind].player.push_back(player);

	// Dash
	buff.type = 0x10;
	buff.byte = Byte8;
	buff.value = SkillX;
	player.buff = buff;
	skillsinfo[Jobs::Pirate::Dash].player.push_back(player);
	buff.type = 0x20;
	buff.byte = Byte8;
	buff.value = SkillY;
	player.buff = buff;
	skillsinfo[Jobs::Pirate::Dash].player.push_back(player);

	// Dragon Roar
	buff.type = 0x02;
	buff.byte = Byte3;
	buff.value = SkillNone;
	player.buff = buff;
	player.itemval = 1;
	skillsinfo[Jobs::DragonKnight::DragonRoar].player.push_back(player);

	// Holy Shield
	buff.type = 0x04;
	buff.byte = Byte6;
	buff.value = SkillX;
	player.buff = buff;
	skillsinfo[Jobs::Bishop::HolyShield].player.push_back(player);
	// End regular buffs

	// Begin act buffs
	// Recovery
	buff.type = 0x04;
	buff.byte = Byte5;
	buff.value = SkillX;
	player.buff = buff;
	skillsinfo[Jobs::Beginner::Recovery].player.push_back(player);
	act.type = ActHeal;
	act.time = 4900;
	act.value = SkillX;
	skillsinfo[Jobs::Beginner::Recovery].act = act;
	skillsinfo[Jobs::Beginner::Recovery].bact = true;

	// Dragon Blood
	buff.type = 0x01;
	buff.byte = Byte1;
	buff.value = SkillWatk;
	player.buff = buff;
	skillsinfo[Jobs::DragonKnight::DragonBlood].player.push_back(player);
	buff.type = 0x80;
	buff.byte = Byte3;
	buff.value = SkillLv;
	player.buff = buff;
	skillsinfo[Jobs::DragonKnight::DragonBlood].player.push_back(player);
	act.type = ActHurt;
	act.time = 4000;
	act.value = SkillX;
	skillsinfo[Jobs::DragonKnight::DragonBlood].act = act;
	skillsinfo[Jobs::DragonKnight::DragonBlood].bact = true;
	// End act buffs

	// Begin map buffs
	// Nimble Feet
	buff.type = 0x80;
	buff.byte = Byte1;
	buff.value = SkillSpeed;
	player.buff = buff;
	player.hasmapval = true;
	player.hasmapentry = true;
	skillsinfo[Jobs::Beginner::NimbleFeet].player.push_back(player);
	map.buff = buff;
	map.useval = true;
	skillsinfo[Jobs::Beginner::NimbleFeet].map.push_back(map);

	// Shadow Stars
	buff.type = 0x01;
	buff.byte = Byte6;
	buff.value = SkillSpecialProc;
	player.buff = buff;
	player.hasmapval = true;
	player.hasmapentry = false;
	skillsinfo[Jobs::NightLord::ShadowStars].player.push_back(player);
	buff.type = 0x00;
	map.buff = buff;
	map.useval = false;
	skillsinfo[Jobs::NightLord::ShadowStars].map.push_back(map);

	// Skill Charges
	buff.type = 0x04;
	buff.byte = Byte1;
	buff.value = SkillMatk;
	player.buff = buff;
	player.hasmapval = false;
	player.hasmapentry = false;
	skillsinfo[Jobs::WhiteKnight::BwFireCharge].player.push_back(player);
	skillsinfo[Jobs::WhiteKnight::BwIceCharge].player.push_back(player);
	skillsinfo[Jobs::WhiteKnight::BwLitCharge].player.push_back(player);
	skillsinfo[Jobs::WhiteKnight::SwordFireCharge].player.push_back(player);
	skillsinfo[Jobs::WhiteKnight::SwordIceCharge].player.push_back(player);
	skillsinfo[Jobs::WhiteKnight::SwordLitCharge].player.push_back(player);
	skillsinfo[Jobs::Paladin::BwHolyCharge].player.push_back(player);
	skillsinfo[Jobs::Paladin::SwordHolyCharge].player.push_back(player);
	buff.type = 0x40;
	buff.byte = Byte3;
	buff.value = SkillMatk;
	player.buff = buff;
	player.hasmapval = true;
	player.hasmapentry = true;
	skillsinfo[Jobs::WhiteKnight::BwFireCharge].player.push_back(player);
	skillsinfo[Jobs::WhiteKnight::BwIceCharge].player.push_back(player);
	skillsinfo[Jobs::WhiteKnight::BwLitCharge].player.push_back(player);
	skillsinfo[Jobs::WhiteKnight::SwordFireCharge].player.push_back(player);
	skillsinfo[Jobs::WhiteKnight::SwordIceCharge].player.push_back(player);
	skillsinfo[Jobs::WhiteKnight::SwordLitCharge].player.push_back(player);
	skillsinfo[Jobs::Paladin::BwHolyCharge].player.push_back(player);
	skillsinfo[Jobs::Paladin::SwordHolyCharge].player.push_back(player);
	map.buff = buff;
	map.useval = true;
	skillsinfo[Jobs::WhiteKnight::BwFireCharge].map.push_back(map);
	skillsinfo[Jobs::WhiteKnight::BwIceCharge].map.push_back(map);
	skillsinfo[Jobs::WhiteKnight::BwLitCharge].map.push_back(map);
	skillsinfo[Jobs::WhiteKnight::SwordFireCharge].map.push_back(map);
	skillsinfo[Jobs::WhiteKnight::SwordIceCharge].map.push_back(map);
	skillsinfo[Jobs::WhiteKnight::SwordLitCharge].map.push_back(map);
	skillsinfo[Jobs::Paladin::BwHolyCharge].map.push_back(map);
	skillsinfo[Jobs::Paladin::SwordHolyCharge].map.push_back(map);

	// Haste
	buff.type = 0x80;
	buff.byte = Byte1;
	buff.value = SkillSpeed;
	player.buff = buff;
	player.hasmapval = true;
	player.hasmapentry = true;
	skillsinfo[Jobs::Assassin::Haste].player.push_back(player);
	skillsinfo[Jobs::Bandit::Haste].player.push_back(player);
	skillsinfo[Jobs::Gm::Haste].player.push_back(player);
	skillsinfo[Jobs::SuperGm::Haste].player.push_back(player);
	map.buff = buff;
	map.useval = true;
	skillsinfo[Jobs::Assassin::Haste].map.push_back(map);
	skillsinfo[Jobs::Bandit::Haste].map.push_back(map);
	skillsinfo[Jobs::Gm::Haste].map.push_back(map);
	skillsinfo[Jobs::SuperGm::Haste].map.push_back(map);
	buff.type = 0x01;
	buff.byte = Byte2;
	buff.value = SkillJump;
	player.buff = buff;
	player.hasmapval = false;
	player.hasmapentry = false;
	skillsinfo[Jobs::Assassin::Haste].player.push_back(player);
	skillsinfo[Jobs::Bandit::Haste].player.push_back(player);
	skillsinfo[Jobs::Gm::Haste].player.push_back(player);
	skillsinfo[Jobs::SuperGm::Haste].player.push_back(player);

	// Dark Sight
	buff.type = 0x80;
	buff.byte = Byte1;
	buff.value = SkillSpeed;
	player.buff = buff;
	player.hasmapval = true;
	player.hasmapentry = true;
	skillsinfo[Jobs::Rogue::DarkSight].player.push_back(player);
	map.buff = buff;
	map.useval = true;
	skillsinfo[Jobs::Rogue::DarkSight].map.push_back(map);
	buff.type = 0x04;
	buff.byte = Byte2;
	buff.value = SkillX;
	player.buff = buff;
	player.hasmapval = true;
	player.hasmapentry = true;
	skillsinfo[Jobs::Rogue::DarkSight].player.push_back(player);
	map.buff = buff;
	map.useval = false;
	skillsinfo[Jobs::Rogue::DarkSight].map.push_back(map);

	// Soul Arrow
	buff.type = 0x01;
	buff.byte = Byte3;
	buff.value = SkillX;
	player.buff = buff;
	player.hasmapval = true;
	player.hasmapentry = true;
	skillsinfo[Jobs::Hunter::SoulArrow].player.push_back(player);
	skillsinfo[Jobs::Crossbowman::SoulArrow].player.push_back(player);
	map.buff = buff;
	map.useval = false;
	skillsinfo[Jobs::Hunter::SoulArrow].map.push_back(map);
	skillsinfo[Jobs::Crossbowman::SoulArrow].map.push_back(map);

	// Energy Charge
	buff.type = 0x08;
	buff.byte = Byte8;
	buff.value = SkillSpecialProc;
	player.buff = buff;
	player.hasmapval = true;
	player.hasmapentry = true;
	skillsinfo[Jobs::Marauder::EnergyCharge].player.push_back(player);
	map.buff = buff;
	map.useval = false;
	skillsinfo[Jobs::Marauder::EnergyCharge].map.push_back(map);

	// Oak Barrel 
	buff.type = 0x02;
	buff.byte = Byte5;
	buff.value = SkillMorph;
	player.buff = buff;
	player.hasmapval = true;
	player.hasmapentry = true;
	skillsinfo[Jobs::Brawler::OakBarrel].player.push_back(player);
	map.buff = buff;
	map.useval = true;
	skillsinfo[Jobs::Brawler::OakBarrel].map.push_back(map);

	// Transformation, Super Transformation
	buff.type = 0x02;
	buff.byte = Byte1;
	buff.value = SkillWdef;
	player.buff = buff;
	player.hasmapval = false;
	player.hasmapentry = false;
	skillsinfo[Jobs::Marauder::Transformation].player.push_back(player);
	skillsinfo[Jobs::Buccaneer::SuperTransformation].player.push_back(player);
	buff.type = 0x08;
	buff.byte = Byte1;
	buff.value = SkillMdef;
	player.buff = buff;
	player.hasmapval = false;
	player.hasmapentry = false;
	skillsinfo[Jobs::Marauder::Transformation].player.push_back(player);
	skillsinfo[Jobs::Buccaneer::SuperTransformation].player.push_back(player);
	buff.type = 0x80;
	buff.byte = Byte1;
	buff.value = SkillSpeed;
	player.buff = buff;
	player.hasmapval = true;
	player.hasmapentry = true;
	skillsinfo[Jobs::Marauder::Transformation].player.push_back(player);
	skillsinfo[Jobs::Buccaneer::SuperTransformation].player.push_back(player);
	map.buff = buff;
	map.useval = true;
	skillsinfo[Jobs::Marauder::Transformation].map.push_back(map);
	skillsinfo[Jobs::Buccaneer::SuperTransformation].map.push_back(map);
	buff.type = 0x01;
	buff.byte = Byte2;
	buff.value = SkillJump;
	player.buff = buff;
	player.hasmapval = false;
	player.hasmapentry = false;
	skillsinfo[Jobs::Marauder::Transformation].player.push_back(player);
	skillsinfo[Jobs::Buccaneer::SuperTransformation].player.push_back(player);
	buff.type = 0x02;
	buff.byte = Byte5;
	buff.value = SkillMorph;
	player.buff = buff;
	player.hasmapval = true;
	player.hasmapentry = true;
	skillsinfo[Jobs::Marauder::Transformation].player.push_back(player);
	skillsinfo[Jobs::Buccaneer::SuperTransformation].player.push_back(player);
	map.buff = buff;
	map.useval = true;
	skillsinfo[Jobs::Marauder::Transformation].map.push_back(map);
	skillsinfo[Jobs::Buccaneer::SuperTransformation].map.push_back(map);

	// Shadow Partner
	buff.type = 0x04;
	buff.byte = Byte4;
	buff.value = SkillSpecialProc;
	player.buff = buff;
	player.hasmapval = true;
	player.hasmapentry = true;
	skillsinfo[Jobs::Hermit::ShadowPartner].player.push_back(player);
	map.buff = buff;
	map.useval = false;
	skillsinfo[Jobs::Hermit::ShadowPartner].map.push_back(map);
	
	// Combo Attack
	buff.type = 0x20;
	buff.byte = Byte3;
	buff.value = SkillSpecialProc;
	player.buff = buff;
	player.hasmapval = true;
	player.hasmapentry = true;
	skillsinfo[Jobs::Crusader::ComboAttack].player.push_back(player);
	map.buff = buff;
	map.useval = true;
	skillsinfo[Jobs::Crusader::ComboAttack].map.push_back(map);
	// End map buffs

	// Begin mount buffs
	player = BuffInfo(); // Placed to clear any previous value pushes

	// Monster Rider
	buff.type = 0x40;
	buff.byte = Byte8;
	buff.value = SkillSpecialProc;
	player.buff = buff;
	skillsinfo[Jobs::Beginner::MonsterRider].player.push_back(player);

	// Battleship
	buff.type = 0x40;
	buff.byte = Byte8;
	buff.value = SkillSpecialProc;
	player.buff = buff;
	skillsinfo[Jobs::Corsair::Battleship].player.push_back(player);
	// End mount buffs

	// Debuffs
	// Stun
	buff.type = 0x02;
	buff.byte = Byte3;
	buff.value = SkillNone;
	player.buff = buff;
	mobskillsinfo[MobSkills::Stun].mob.push_back(player);
	mobskillsinfo[MobSkills::Stun].delay = 0;

	// Poison
	buff.type = 0x04;
	buff.byte = Byte3;
	buff.value = SkillX;
	player.buff = buff;
	mobskillsinfo[MobSkills::Poison].mob.push_back(player);
	mobskillsinfo[MobSkills::Poison].delay = 500;

	// Seal
	buff.type = 0x08;
	buff.byte = Byte3;
	buff.value = SkillNone;
	player.buff = buff;
	mobskillsinfo[MobSkills::Seal].mob.push_back(player);
	mobskillsinfo[MobSkills::Seal].delay = 900;

	// Darkness
	buff.type = 0x10;
	buff.byte = Byte3;
	buff.value = SkillNone;
	player.buff = buff;
	mobskillsinfo[MobSkills::Darkness].mob.push_back(player);
	mobskillsinfo[MobSkills::Darkness].delay = 900;

	// Weakness
	buff.type = 0x40;
	buff.byte = Byte4;
	buff.value = SkillNone;
	player.buff = buff;
	mobskillsinfo[MobSkills::Weakness].mob.push_back(player);
	mobskillsinfo[MobSkills::Weakness].delay = 900;

	// Curse
	buff.type = 0x80;
	buff.byte = Byte4;
	buff.value = SkillNone;
	player.buff = buff;
	mobskillsinfo[MobSkills::Curse].mob.push_back(player);
	mobskillsinfo[MobSkills::Curse].delay = 900;

	// Slow
	buff.type = 0x01;
	buff.byte = Byte5;
	buff.value = SkillX;
	player.buff = buff;
	mobskillsinfo[MobSkills::Slow].mob.push_back(player);
	mobskillsinfo[MobSkills::Slow].delay = 900;

	// Seduce
	buff.type = 0x80;
	buff.byte = Byte5;
	buff.value = SkillX;
	player.buff = buff;
	mobskillsinfo[MobSkills::Seduce].mob.push_back(player);
	mobskillsinfo[MobSkills::Seduce].delay = 900;

	// Zombify
	buff.type = 0x40;
	buff.byte = Byte6;
	buff.value = SkillX;
	player.buff = buff;
	mobskillsinfo[MobSkills::Zombify].mob.push_back(player);
	mobskillsinfo[MobSkills::Zombify].delay = 900;

	// Crazy Skull
	buff.type = 0x08;
	buff.byte = Byte7;
	buff.value = SkillX;
	player.buff = buff;
	mobskillsinfo[MobSkills::CrazySkull].mob.push_back(player);
	mobskillsinfo[MobSkills::CrazySkull].delay = 900;
}

void Buffs::addItemInfo(int32_t itemid, const vector<uint8_t> &types, const vector<int8_t> &bytes, const vector<int16_t> &values) {
	Buff buff;
	BuffInfo player;
	BuffMapInfo map;

	itemid *= -1;

	for (size_t i = 0; i < types.size(); i++) {
		buff = Buff();
		player = BuffInfo();
		map = BuffMapInfo();
		buff.byte = bytes[i];
		buff.type = types[i];
		buff.value = SkillNone;
		player.buff = buff;
		player.itemval = values[i];
		player.useval = true;
		if ((buff.byte == Byte1 && (buff.type & 0x80) > 0) || buff.byte == Byte5) {
			player.hasmapval = true;
			player.hasmapentry = true;
			skillsinfo[itemid].player.push_back(player);
			map.buff = buff;
			map.useval = true;
			skillsinfo[itemid].map.push_back(map);
		}
		else {
			skillsinfo[itemid].player.push_back(player);
		}
	}
}

int16_t Buffs::getValue(int8_t value, int32_t skillid, uint8_t level) {
	int16_t rvalue = 0;
	SkillLevelInfo *skill = SkillDataProvider::Instance()->getSkill(skillid, level);
	switch (value) {
		case SkillX: rvalue = skill->x; break;
		case SkillY: rvalue = skill->y; break;
		case SkillSpeed: rvalue = skill->speed; break;
		case SkillJump: rvalue = skill->jump; break;
		case SkillWatk: rvalue = skill->watk; break;
		case SkillWdef: rvalue = skill->wdef; break;
		case SkillMatk: rvalue = skill->matk; break;
		case SkillMdef: rvalue = skill->mdef; break;
		case SkillAcc: rvalue = skill->acc; break;
		case SkillAvo: rvalue = skill->avo; break;
		case SkillProp: rvalue = skill->prop; break;
		case SkillMorph: rvalue = skill->morph; break;
		case SkillLv: rvalue = level; break;
	}
	return rvalue;
}

int16_t Buffs::getMobSkillValue(int8_t value, uint8_t skillid, uint8_t level) {
	int16_t rvalue = 0;
	MobSkillLevelInfo *skill = SkillDataProvider::Instance()->getMobSkill(skillid, level);
	switch (value) {
		case SkillX: rvalue = static_cast<int16_t>(skill->x); break;
		case SkillY: rvalue = static_cast<int16_t>(skill->y); break;
		case SkillProp: rvalue = skill->prop; break;
		case SkillLv: rvalue = level; break;
	}
	return rvalue;
}

int32_t Buffs::parseMountInfo(Player *player, int32_t skillid, uint8_t level) {
	int32_t mountid = 0;
	switch (skillid) {
		case Jobs::Beginner::MonsterRider:
			mountid = player->getInventory()->getEquippedId(EquipSlots::Mount);
			break;
		case Jobs::Corsair::Battleship:
			mountid = Items::BattleshipMount;
			break;
	}
	return mountid;
}

ActiveBuff Buffs::parseBuffInfo(Player *player, int32_t skillid, uint8_t level) {
	ActiveBuff playerskill;
	BuffInfo cur;
	SkillLevelInfo *skill = SkillDataProvider::Instance()->getSkill(skillid, level);
	for (size_t i = 0; i < skillsinfo[skillid].player.size(); i++) {
		cur = skillsinfo[skillid].player[i];
		int8_t val = cur.buff.value;
		if (GameLogicUtilities::isMaxDarkSight(skillid, level) && val == SkillSpeed) // Cancel speed change for maxed dark sight
			continue;
		playerskill.types[cur.buff.byte] += cur.buff.type;
		int16_t value = 0;
		if (cur.hasmapval)
			playerskill.hasmapbuff = true;
		if (val == SkillNone)
			value = cur.itemval;
		else {
			switch (skillid) {
				case Jobs::Bowmaster::SharpEyes:
				case Jobs::Marksman::SharpEyes:
				case Jobs::Hermit::ShadowPartner:
					value = skill->x * 256 + skill->y;
					break;
				case Jobs::Crusader::ComboAttack:
					value = player->getActiveBuffs()->getCombo() + 1;
					break;
				case Jobs::NightLord::ShadowStars:
					value = (player->getInventory()->doShadowStars() % 10000) + 1;
					break;
				case Jobs::Marauder::Transformation:
				case Jobs::Buccaneer::SuperTransformation:
					value = getValue(val, skillid, level);
					if (val == SkillMorph)
						value += (player->getStats()->getGender() * 100); // Females are +100
					break;
				case Jobs::Marauder::EnergyCharge:
					value = player->getActiveBuffs()->getEnergyChargeLevel();
					break;
				default:
					value = getValue(val, skillid, level);
					break;
			}
		}
		playerskill.vals.push_back(value);
	}
	return playerskill;
}

ActiveMapBuff Buffs::parseBuffMapInfo(Player *player, int32_t skillid, uint8_t level) {
	ActiveMapBuff mapskill;
	BuffInfo cur;
	BuffMapInfo map;
	int32_t maps = 0;
	for (size_t i = 0; i < skillsinfo[skillid].player.size(); i++) {
		cur = skillsinfo[skillid].player[i];
		if (!cur.hasmapval)
			continue;
		map = skillsinfo[skillid].map[maps++];
		int8_t val = map.buff.value;
		if (GameLogicUtilities::isMaxDarkSight(skillid, level) && val == SkillSpeed) { // Cancel speed update for maxed dark sight
			continue;
		}
		mapskill.bytes.push_back(map.buff.byte);
		mapskill.types.push_back(map.buff.type);
		mapskill.typelist[map.buff.byte] += map.buff.type;
		mapskill.usevals.push_back(map.useval);
		if (map.useval) {
			int16_t value = 0;
			if (val == SkillNone) {
				value = cur.itemval;
			}
			else {
				switch (skillid) {
					case Jobs::Crusader::ComboAttack:
						value = player->getActiveBuffs()->getCombo() + 1;
						break;
					default:
						value = getValue(val, skillid, level);
						break;
				}
			}
			mapskill.values.push_back(value);
		}
	}
	return mapskill;
}

ActiveMapBuff Buffs::parseBuffMapEntryInfo(Player *player, int32_t skillid, uint8_t level) {
	ActiveMapBuff mapskill;
	BuffInfo cur;
	BuffMapInfo map;
	int8_t mapctr = 0;
	for (size_t i = 0; i < skillsinfo[skillid].player.size(); i++) {
		cur = skillsinfo[skillid].player[i];
		if (!cur.hasmapentry) {
			continue;
		}
		map = skillsinfo[skillid].map[mapctr++];
		int8_t val = map.buff.value;
		if (GameLogicUtilities::isMaxDarkSight(skillid, level) && val == SkillSpeed) { // Cancel speed update for maxed dark sight
			continue;
		}
		mapskill.bytes.push_back(map.buff.byte);
		mapskill.types.push_back(map.buff.type);
		mapskill.typelist[map.buff.byte] += map.buff.type;
		mapskill.usevals.push_back(map.useval);
		if (map.useval) {
			int16_t value = 0;
			if (val == SkillNone) {
				value = cur.itemval;
			}
			else {
				switch (skillid) {
					case Jobs::Crusader::ComboAttack:
						value = player->getActiveBuffs()->getCombo() + 1;
						break;
					default:
						value = getValue(val, skillid, level);
						break;
				}
			}
			mapskill.values.push_back(value);
		}
	}
	return mapskill;
}

vector<Buff> Buffs::parseBuffs(int32_t skillid, uint8_t level) {
	vector<Buff> ret;
	for (size_t i = 0; i < skillsinfo[skillid].player.size(); i++) {
		BuffInfo cur = skillsinfo[skillid].player[i];
		if (GameLogicUtilities::isMaxDarkSight(skillid, level) && cur.buff.value == SkillSpeed) { // Cancel speed update for maxed dark sight
			continue;
		}
		ret.push_back(cur.buff);
	}
	return ret;
}

ActiveBuff Buffs::parseMobBuffInfo(Player *player, uint8_t skillid, uint8_t level) {
	ActiveBuff playerskill;
	BuffInfo cur;
	for (size_t i = 0; i < mobskillsinfo[skillid].mob.size(); i++) {
		cur = mobskillsinfo[skillid].mob[i];
		int8_t val = cur.buff.value;
		playerskill.types[cur.buff.byte] += cur.buff.type;
		playerskill.hasmapbuff = true;
		int16_t value = (val == SkillNone ? 1 : getMobSkillValue(val, skillid, level));
		playerskill.vals.push_back(value);
	}
	return playerskill;
}

ActiveMapBuff Buffs::parseMobBuffMapInfo(Player *player, uint8_t skillid, uint8_t level) {
	ActiveMapBuff mapskill;
	BuffInfo cur;
	for (size_t i = 0; i < mobskillsinfo[skillid].mob.size(); i++) {
		cur = mobskillsinfo[skillid].mob[i];
		int8_t val = cur.buff.value;
		mapskill.bytes.push_back(cur.buff.byte);
		mapskill.types.push_back(cur.buff.type);
		mapskill.typelist[cur.buff.byte] += cur.buff.type;
		mapskill.usevals.push_back(cur.useval);
		int16_t value = (val == SkillNone ? 1 : getMobSkillValue(val, skillid, level));
		mapskill.values.push_back(value);
	}
	return mapskill;
}

ActiveMapBuff Buffs::parseMobBuffMapEntryInfo(Player *player, uint8_t skillid, uint8_t level) {
	ActiveMapBuff mapskill;
	BuffInfo cur;
	for (size_t i = 0; i < mobskillsinfo[skillid].mob.size(); i++) {
		cur = mobskillsinfo[skillid].mob[i];
		int8_t val = cur.buff.value;
		mapskill.bytes.push_back(cur.buff.byte);
		mapskill.types.push_back(cur.buff.type);
		mapskill.typelist[cur.buff.byte] += cur.buff.type;
		mapskill.usevals.push_back(true);
		mapskill.usevals.push_back(false);
		mapskill.values.push_back(skillid);
		mapskill.values.push_back(level);
	}
	mapskill.debuff = true;
	return mapskill;
}

vector<Buff> Buffs::parseMobBuffs(uint8_t skillid) {
	vector<Buff> ret;
	for (size_t i = 0; i < mobskillsinfo[skillid].mob.size(); i++) {
		BuffInfo cur = mobskillsinfo[skillid].mob[i];
		ret.push_back(cur.buff);
	}
	return ret;
}

bool Buffs::addBuff(Player *player, int32_t skillid, uint8_t level, int16_t addedinfo) {
	if (skillsinfo.find(skillid) == skillsinfo.end())
		return false; // Not a buff, so return false

	int32_t mountid = parseMountInfo(player, skillid, level);
	SkillLevelInfo *skill = SkillDataProvider::Instance()->getSkill(skillid, level);
	int32_t time = skill->time;
	switch (skillid) {
		case Jobs::DragonKnight::DragonRoar:
			time = skill->y;
			break;
		case Jobs::Beginner::MonsterRider:
		case Jobs::Corsair::Battleship:
			if (mountid == 0) {
				// Hacking
				return true;
			}
			player->getActiveBuffs()->setMountInfo(skillid, mountid);
			break;
		case Jobs::SuperGm::Hide:
			time = 2100000; // Make sure that it doesn't end any time soon
			break;
		case Jobs::Spearman::HyperBody:
		case Jobs::SuperGm::HyperBody:
			player->getStats()->setHyperBody(skill->x, skill->y);
			break;
		case Jobs::Crusader::ComboAttack:
			player->getActiveBuffs()->setCombo(0, false);
			break;
		case Jobs::Hero::Enrage:
			if (player->getActiveBuffs()->getCombo() != 10)
				return true;
			player->getActiveBuffs()->setCombo(0, true);
			break;
		case Jobs::Fighter::SwordBooster:
		case Jobs::Fighter::AxeBooster:
		case Jobs::Page::SwordBooster:
		case Jobs::Page::BwBooster:
		case Jobs::Spearman::SpearBooster:
		case Jobs::Spearman::PolearmBooster:
		case Jobs::FPMage::SpellBooster:
		case Jobs::ILMage::SpellBooster:
		case Jobs::Hunter::BowBooster:
		case Jobs::Crossbowman::CrossbowBooster:
		case Jobs::Assassin::ClawBooster:
		case Jobs::Bandit::DaggerBooster:
		case Jobs::Brawler::KnucklerBooster:
		case Jobs::Gunslinger::GunBooster:
			player->getActiveBuffs()->setBooster(skillid); // Makes switching equips MUCH easier
			break;
		case Jobs::WhiteKnight::BwFireCharge:
		case Jobs::WhiteKnight::BwIceCharge:
		case Jobs::WhiteKnight::BwLitCharge:
		case Jobs::WhiteKnight::SwordFireCharge:
		case Jobs::WhiteKnight::SwordIceCharge:
		case Jobs::WhiteKnight::SwordLitCharge:
		case Jobs::Paladin::BwHolyCharge:
		case Jobs::Paladin::SwordHolyCharge:
			player->getActiveBuffs()->setCharge(skillid); // Makes switching equips/Charged Blow easier
			break;
	}
	vector<Buff> buffs = parseBuffs(skillid, level);
	ActiveBuff playerskill = parseBuffInfo(player, skillid, level);
	ActiveMapBuff mapskill = parseBuffMapInfo(player, skillid, level);
	ActiveMapBuff enterskill = parseBuffMapEntryInfo(player, skillid, level);

	if (mountid > 0)
		BuffsPacket::useMount(player, skillid, time, playerskill, addedinfo, mountid);
	else {
		switch (skillid) {
			case Jobs::Pirate::Dash:
				BuffsPacket::usePirateBuff(player, skillid, time, playerskill, mapskill);
				break;
			case Jobs::Marauder::EnergyCharge:
				BuffsPacket::usePirateBuff(player, 0, (player->getActiveBuffs()->getEnergyChargeLevel() == 10000 ? time : 0), playerskill, mapskill);
				break;
			case Jobs::Buccaneer::SpeedInfusion:
				BuffsPacket::useSpeedInfusion(player, skillid, time, playerskill, mapskill, addedinfo);
				break;
			default:
				BuffsPacket::useSkill(player, skillid, time, playerskill, mapskill, addedinfo);
		}
	}
	if (skillid != player->getSkills()->getEnergyCharge() || player->getActiveBuffs()->getEnergyChargeLevel() == 10000) {
		PlayerActiveBuffs *playerbuffs = player->getActiveBuffs();
		playerbuffs->addBuffInfo(skillid, buffs);
		playerbuffs->addMapEntryBuffInfo(enterskill);
		playerbuffs->setActiveSkillLevel(skillid, level);
		playerbuffs->removeBuff(skillid);
		playerbuffs->addBuff(skillid, time);
		doAct(player, skillid, level);
	}
	return true;
}

void Buffs::addBuff(Player *player, int32_t itemid, int32_t time) {
	itemid *= -1; // Make the Item ID negative for the packet and to discern from skill buffs
	vector<Buff> buffs = parseBuffs(itemid, 0);
	ActiveBuff pskill = parseBuffInfo(player, itemid, 0);
	ActiveMapBuff mskill = parseBuffMapInfo(player, itemid, 0);
	ActiveMapBuff meskill = parseBuffMapEntryInfo(player, itemid, 0);

	BuffsPacket::useSkill(player, itemid, time, pskill, mskill, 0);

	PlayerActiveBuffs *playerbuffs = player->getActiveBuffs();
	playerbuffs->removeBuff(itemid);
	playerbuffs->addBuffInfo(itemid, buffs);
	playerbuffs->addBuff(itemid, time);
	playerbuffs->addMapEntryBuffInfo(meskill);
	playerbuffs->setActiveSkillLevel(itemid, 1);
}

void Buffs::endBuff(Player *player, int32_t skill) {
	PlayerActiveBuffs *playerbuffs = player->getActiveBuffs();
	switch (skill) {
		case Jobs::Beginner::MonsterRider:
		case Jobs::Corsair::Battleship:
			playerbuffs->setMountInfo(0, 0);
		case Jobs::Crusader::ComboAttack:
			playerbuffs->setCombo(0, false);
			break;
		case Jobs::Spearman::HyperBody:
		case Jobs::SuperGm::HyperBody:
			player->getStats()->setHyperBody(0, 0);
			player->getStats()->setHp(player->getStats()->getHp());
			player->getStats()->setMp(player->getStats()->getMp());
			break;
		case Jobs::Marauder::EnergyCharge:
			playerbuffs->resetEnergyChargeLevel();
			break;
		case Jobs::Fighter::SwordBooster:
		case Jobs::Fighter::AxeBooster:
		case Jobs::Page::SwordBooster:
		case Jobs::Page::BwBooster:
		case Jobs::Spearman::SpearBooster:
		case Jobs::Spearman::PolearmBooster:
		case Jobs::FPMage::SpellBooster:
		case Jobs::ILMage::SpellBooster:
		case Jobs::Hunter::BowBooster:
		case Jobs::Crossbowman::CrossbowBooster:
		case Jobs::Assassin::ClawBooster:
		case Jobs::Bandit::DaggerBooster:
		case Jobs::Brawler::KnucklerBooster:
		case Jobs::Gunslinger::GunBooster:
			playerbuffs->setBooster(0);
			break;
		case Jobs::WhiteKnight::BwFireCharge:
		case Jobs::WhiteKnight::BwIceCharge:
		case Jobs::WhiteKnight::BwLitCharge:
		case Jobs::WhiteKnight::SwordFireCharge:
		case Jobs::WhiteKnight::SwordIceCharge:
		case Jobs::WhiteKnight::SwordLitCharge:
		case Jobs::Paladin::BwHolyCharge:
		case Jobs::Paladin::SwordHolyCharge:
			playerbuffs->setCharge(0);
			break;
	}
	uint8_t level = playerbuffs->getActiveSkillLevel(skill);
	vector<Buff> buffs = parseBuffs(skill, level);
	ActiveMapBuff meskill = parseBuffMapEntryInfo(player, skill, level);
	ActiveBuff pskill = playerbuffs->removeBuffInfo(skill, buffs);

	BuffsPacket::endSkill(player, pskill);

	playerbuffs->deleteMapEntryBuffInfo(meskill);
	playerbuffs->setActiveSkillLevel(skill, 0);
}

void Buffs::doAct(Player *player, int32_t skillid, uint8_t level) {
	if (skillsinfo[skillid].bact) {
		int16_t value = getValue(skillsinfo[skillid].act.value, skillid, level);
		player->getActiveBuffs()->addAct(skillid, skillsinfo[skillid].act.type, value, skillsinfo[skillid].act.time);
	}
}

void Buffs::addDebuff(Player *player, uint8_t skillid, uint8_t level) {
	int16_t time = SkillDataProvider::Instance()->getMobSkill(skillid, level)->time;
	vector<Buff> buffs = parseMobBuffs(skillid);
	ActiveBuff playerskill = parseMobBuffInfo(player, skillid, level);
	ActiveMapBuff mapskill = parseMobBuffMapInfo(player, skillid, level);
	ActiveMapBuff enterskill = parseMobBuffMapEntryInfo(player, skillid, level);

	BuffsPacket::giveDebuff(player, skillid, level, time, mobskillsinfo[skillid].delay, playerskill, mapskill);

	PlayerActiveBuffs *playerbuffs = player->getActiveBuffs();
	playerbuffs->setActiveSkillLevel(skillid, level);
	playerbuffs->addBuffInfo(skillid, buffs);
	playerbuffs->addMapEntryBuffInfo(enterskill);
	playerbuffs->addBuff(skillid, time);
}

void Buffs::endDebuff(Player *player, uint8_t skill) {
	PlayerActiveBuffs *playerbuffs = player->getActiveBuffs();
	vector<Buff> buffs = parseMobBuffs(skill);
	ActiveMapBuff meskill = parseMobBuffMapEntryInfo(player, skill, 1);
	ActiveBuff pskill = playerbuffs->removeBuffInfo(skill, buffs);

	BuffsPacket::endDebuff(player, pskill);

	playerbuffs->deleteMapEntryBuffInfo(meskill);
	playerbuffs->setActiveSkillLevel(skill, 0);
}