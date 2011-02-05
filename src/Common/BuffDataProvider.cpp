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
#include "BuffDataProvider.h"
#include "GameConstants.h"
#include "GameLogicUtilities.h"
#include "InitializeCommon.h"
#include "ItemDataProvider.h"
#include <iostream>
#include <iomanip>

using Initializing::outputWidth;

BuffDataProvider * BuffDataProvider::singleton = nullptr;

void BuffDataProvider::loadData() {
	using namespace BuffBytes;
	std::cout << std::setw(outputWidth) << std::left << "Initializing Buffs... ";

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
	skillsinfo[Jobs::DawnWarrior::SwordBooster].player.push_back(player);
	skillsinfo[Jobs::BlazeWizard::SpellBooster].player.push_back(player);
	skillsinfo[Jobs::WindArcher::BowBooster].player.push_back(player);
	skillsinfo[Jobs::NightWalker::ClawBooster].player.push_back(player);
	skillsinfo[Jobs::ThunderBreaker::KnucklerBooster].player.push_back(player);

	// Speed Infusion
	buff.type = 0x80;
	buff.byte = Byte9;
	buff.value = SkillX;
	player.buff = buff;
	skillsinfo[Jobs::Buccaneer::SpeedInfusion].player.push_back(player);
	skillsinfo[Jobs::ThunderBreaker::SpeedInfusion].player.push_back(player);

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
	skillsinfo[Jobs::BlazeWizard::MagicGuard].player.push_back(player);

	// Magic Armor, Iron Body
	buff.type = 0x02;
	buff.byte = Byte1;
	buff.value = SkillWdef;
	player.buff = buff;
	skillsinfo[Jobs::Magician::MagicArmor].player.push_back(player);
	skillsinfo[Jobs::Swordsman::IronBody].player.push_back(player);
	skillsinfo[Jobs::BlazeWizard::MagicArmor].player.push_back(player);
	skillsinfo[Jobs::DawnWarrior::IronBody].player.push_back(player);

	// Focus
	buff.type = 0x10;
	buff.byte = Byte1;
	buff.value = SkillAcc;
	player.buff = buff;
	skillsinfo[Jobs::Archer::Focus].player.push_back(player);
	skillsinfo[Jobs::WindArcher::Focus].player.push_back(player);
	buff.type = 0x20;
	buff.byte = Byte1;
	buff.value = SkillAvo;
	player.buff = buff;
	skillsinfo[Jobs::Archer::Focus].player.push_back(player);
	skillsinfo[Jobs::WindArcher::Focus].player.push_back(player);

	// Rage
	buff.type = 0x01;
	buff.byte = Byte1;
	buff.value = SkillWatk;
	player.buff = buff;
	skillsinfo[Jobs::Fighter::Rage].player.push_back(player);
	skillsinfo[Jobs::DawnWarrior::Rage].player.push_back(player);
	buff.type = 0x02;
	buff.byte = Byte1;
	buff.value = SkillWdef;
	player.buff = buff;
	skillsinfo[Jobs::Fighter::Rage].player.push_back(player);
	skillsinfo[Jobs::DawnWarrior::Rage].player.push_back(player);

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
	skillsinfo[Jobs::BlazeWizard::Meditation].player.push_back(player);

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

	// Begin map buffs
	// Nimble Feet
	buff.type = 0x80;
	buff.byte = Byte1;
	buff.value = SkillSpeed;
	player.buff = buff;
	player.hasmapval = true;
	player.hasmapentry = true;
	skillsinfo[Jobs::Beginner::NimbleFeet].player.push_back(player);
	skillsinfo[Jobs::Noblesse::NimbleFeet].player.push_back(player);
	map.buff = buff;
	map.useval = true;
	skillsinfo[Jobs::Beginner::NimbleFeet].map.push_back(map);
	skillsinfo[Jobs::Noblesse::NimbleFeet].map.push_back(map);

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
	skillsinfo[Jobs::DawnWarrior::SoulCharge].player.push_back(player);
	skillsinfo[Jobs::ThunderBreaker::LightningCharge].player.push_back(player);
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
	skillsinfo[Jobs::DawnWarrior::SoulCharge].player.push_back(player);
	skillsinfo[Jobs::ThunderBreaker::LightningCharge].player.push_back(player);
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
	skillsinfo[Jobs::DawnWarrior::SoulCharge].map.push_back(map);
	skillsinfo[Jobs::ThunderBreaker::LightningCharge].map.push_back(map);

	// Wind Walk
	buff.type = 0x80;
	buff.byte = Byte1;
	buff.value = SkillSpeed;
	player.buff = buff;
	player.hasmapval = true;
	player.hasmapentry = true;
	skillsinfo[Jobs::WindArcher::WindWalk].player.push_back(player);
	map.buff = buff;
	map.useval = true;
	skillsinfo[Jobs::WindArcher::WindWalk].map.push_back(map);

	// Dash
	buff.type = 0x10;
	buff.byte = Byte9;
	buff.value = SkillX;
	player.buff = buff;
	player.hasmapval = true;
	skillsinfo[Jobs::Pirate::Dash].player.push_back(player);
	skillsinfo[Jobs::ThunderBreaker::Dash].player.push_back(player);
	buff.type = 0x20;
	map.buff = buff;
	map.useval = true;
	skillsinfo[Jobs::Pirate::Dash].map.push_back(map);
	skillsinfo[Jobs::ThunderBreaker::Dash].map.push_back(map);
	buff.type = 0x20;
	buff.byte = Byte9;
	buff.value = SkillY;
	player.buff = buff;
	player.hasmapval = true;
	skillsinfo[Jobs::Pirate::Dash].player.push_back(player);
	skillsinfo[Jobs::ThunderBreaker::Dash].player.push_back(player);
	buff.type = 0x40;
	map.buff = buff;
	map.useval = true;
	skillsinfo[Jobs::Pirate::Dash].map.push_back(map);
	skillsinfo[Jobs::ThunderBreaker::Dash].map.push_back(map);

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
	skillsinfo[Jobs::NightWalker::Haste].player.push_back(player);
	map.buff = buff;
	map.useval = true;
	skillsinfo[Jobs::Assassin::Haste].map.push_back(map);
	skillsinfo[Jobs::Bandit::Haste].map.push_back(map);
	skillsinfo[Jobs::Gm::Haste].map.push_back(map);
	skillsinfo[Jobs::SuperGm::Haste].map.push_back(map);
	skillsinfo[Jobs::NightWalker::Haste].map.push_back(map);
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
	skillsinfo[Jobs::NightWalker::Haste].player.push_back(player);

	// Dark Sight
	buff.type = 0x80;
	buff.byte = Byte1;
	buff.value = SkillSpeed;
	player.buff = buff;
	player.hasmapval = true;
	player.hasmapentry = true;
	skillsinfo[Jobs::Rogue::DarkSight].player.push_back(player);
	skillsinfo[Jobs::NightWalker::DarkSight].player.push_back(player);
	map.buff = buff;
	map.useval = true;
	skillsinfo[Jobs::Rogue::DarkSight].map.push_back(map);
	skillsinfo[Jobs::NightWalker::DarkSight].map.push_back(map);
	buff.type = 0x04;
	buff.byte = Byte2;
	buff.value = SkillX;
	player.buff = buff;
	player.hasmapval = true;
	player.hasmapentry = true;
	skillsinfo[Jobs::Rogue::DarkSight].player.push_back(player);
	skillsinfo[Jobs::NightWalker::DarkSight].player.push_back(player);
	map.buff = buff;
	map.useval = false;
	skillsinfo[Jobs::Rogue::DarkSight].map.push_back(map);
	skillsinfo[Jobs::NightWalker::DarkSight].map.push_back(map);

	// Soul Arrow
	buff.type = 0x01;
	buff.byte = Byte3;
	buff.value = SkillX;
	player.buff = buff;
	player.hasmapval = true;
	player.hasmapentry = true;
	skillsinfo[Jobs::Hunter::SoulArrow].player.push_back(player);
	skillsinfo[Jobs::Crossbowman::SoulArrow].player.push_back(player);
	skillsinfo[Jobs::WindArcher::SoulArrow].player.push_back(player);
	map.buff = buff;
	map.useval = false;
	skillsinfo[Jobs::Hunter::SoulArrow].map.push_back(map);
	skillsinfo[Jobs::Crossbowman::SoulArrow].map.push_back(map);
	skillsinfo[Jobs::WindArcher::SoulArrow].map.push_back(map);

	// Energy Charge
	buff.type = 0x08;
	buff.byte = Byte9;
	buff.value = SkillSpecialProc;
	player.buff = buff;
	player.hasmapval = true;
	player.hasmapentry = true;
	skillsinfo[Jobs::Marauder::EnergyCharge].player.push_back(player);
	skillsinfo[Jobs::ThunderBreaker::EnergyCharge].player.push_back(player);
	map.buff = buff;
	map.useval = false;
	skillsinfo[Jobs::Marauder::EnergyCharge].map.push_back(map);
	skillsinfo[Jobs::ThunderBreaker::EnergyCharge].map.push_back(map);

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
	skillsinfo[Jobs::WindArcher::EagleEye].player.push_back(player);
	skillsinfo[Jobs::ThunderBreaker::Transformation].player.push_back(player);
	buff.type = 0x08;
	buff.byte = Byte1;
	buff.value = SkillMdef;
	player.buff = buff;
	player.hasmapval = false;
	player.hasmapentry = false;
	skillsinfo[Jobs::Marauder::Transformation].player.push_back(player);
	skillsinfo[Jobs::Buccaneer::SuperTransformation].player.push_back(player);
	skillsinfo[Jobs::WindArcher::EagleEye].player.push_back(player);
	skillsinfo[Jobs::ThunderBreaker::Transformation].player.push_back(player);
	buff.type = 0x80;
	buff.byte = Byte1;
	buff.value = SkillSpeed;
	player.buff = buff;
	player.hasmapval = true;
	player.hasmapentry = true;
	skillsinfo[Jobs::Marauder::Transformation].player.push_back(player);
	skillsinfo[Jobs::Buccaneer::SuperTransformation].player.push_back(player);
	skillsinfo[Jobs::WindArcher::EagleEye].player.push_back(player);
	skillsinfo[Jobs::ThunderBreaker::Transformation].player.push_back(player);
	map.buff = buff;
	map.useval = true;
	skillsinfo[Jobs::Marauder::Transformation].map.push_back(map);
	skillsinfo[Jobs::Buccaneer::SuperTransformation].map.push_back(map);
	skillsinfo[Jobs::WindArcher::EagleEye].map.push_back(map);
	skillsinfo[Jobs::ThunderBreaker::Transformation].map.push_back(map);
	buff.type = 0x01;
	buff.byte = Byte2;
	buff.value = SkillJump;
	player.buff = buff;
	player.hasmapval = false;
	player.hasmapentry = false;
	skillsinfo[Jobs::Marauder::Transformation].player.push_back(player);
	skillsinfo[Jobs::Buccaneer::SuperTransformation].player.push_back(player);
	skillsinfo[Jobs::WindArcher::EagleEye].player.push_back(player);
	skillsinfo[Jobs::ThunderBreaker::Transformation].player.push_back(player);
	buff.type = 0x02;
	buff.byte = Byte5;
	buff.value = SkillMorph;
	player.buff = buff;
	player.hasmapval = true;
	player.hasmapentry = true;
	skillsinfo[Jobs::Marauder::Transformation].player.push_back(player);
	skillsinfo[Jobs::Buccaneer::SuperTransformation].player.push_back(player);
	skillsinfo[Jobs::WindArcher::EagleEye].player.push_back(player);
	skillsinfo[Jobs::ThunderBreaker::Transformation].player.push_back(player);
	map.buff = buff;
	map.useval = true;
	skillsinfo[Jobs::Marauder::Transformation].map.push_back(map);
	skillsinfo[Jobs::Buccaneer::SuperTransformation].map.push_back(map);
	skillsinfo[Jobs::WindArcher::EagleEye].map.push_back(map);
	skillsinfo[Jobs::ThunderBreaker::Transformation].map.push_back(map);

	// Shadow Partner
	buff.type = 0x04;
	buff.byte = Byte4;
	buff.value = SkillSpecialProc;
	player.buff = buff;
	player.hasmapval = true;
	player.hasmapentry = true;
	skillsinfo[Jobs::Hermit::ShadowPartner].player.push_back(player);
	skillsinfo[Jobs::NightWalker::ShadowPartner].player.push_back(player);
	map.buff = buff;
	map.useval = false;
	skillsinfo[Jobs::Hermit::ShadowPartner].map.push_back(map);
	skillsinfo[Jobs::NightWalker::ShadowPartner].map.push_back(map);

	// Combo Attack
	buff.type = 0x20;
	buff.byte = Byte3;
	buff.value = SkillSpecialProc;
	player.buff = buff;
	player.hasmapval = true;
	player.hasmapentry = true;
	skillsinfo[Jobs::Crusader::ComboAttack].player.push_back(player);
	skillsinfo[Jobs::DawnWarrior::ComboAttack].player.push_back(player);
	map.buff = buff;
	map.useval = true;
	skillsinfo[Jobs::Crusader::ComboAttack].map.push_back(map);
	skillsinfo[Jobs::DawnWarrior::ComboAttack].map.push_back(map);
	// End map buffs

	// Begin mount buffs
	player = BuffInfo(); // Placed to clear any previous value pushes

	// Monster Rider
	buff.type = 0x40;
	buff.byte = Byte9;
	buff.value = SkillSpecialProc;
	player.buff = buff;
	player.hasmapval = true;
	skillsinfo[Jobs::Beginner::MonsterRider].player.push_back(player);
	skillsinfo[Jobs::Noblesse::MonsterRider].player.push_back(player);
	buff.type = 0x40;
	map.buff = buff;
	skillsinfo[Jobs::Beginner::MonsterRider].map.push_back(map);
	skillsinfo[Jobs::Noblesse::MonsterRider].map.push_back(map);

	// Battleship
	buff.type = 0x40;
	buff.byte = Byte9;
	buff.value = SkillSpecialProc;
	player.buff = buff;
	player.hasmapval = true;
	skillsinfo[Jobs::Corsair::Battleship].player.push_back(player);
	buff.type = 0x40;
	map.buff = buff;
	skillsinfo[Jobs::Corsair::Battleship].map.push_back(map);
	// End mount buffs

	// Begin very unusual buffs
	player = BuffInfo(); // Placed to clear any previous value pushes

	// Homing Beacon
	buff.type = 0x01;
	buff.byte = Byte10;
	buff.value = SkillNone;
	player.buff = buff;
	player.itemval = 1;
	skillsinfo[Jobs::Outlaw::HomingBeacon].player.push_back(player);
	// Bullseye
	buff.type = 0x01;
	buff.byte = Byte10;
	buff.value = SkillNone;
	player.buff = buff;
	player.itemval = 1;
	skillsinfo[Jobs::Corsair::Bullseye].player.push_back(player);
	// End very unusual buffs

	// Begin act buffs
	// Recovery
	buff.type = 0x04;
	buff.byte = Byte5;
	buff.value = SkillX;
	player.buff = buff;
	skillsinfo[Jobs::Beginner::Recovery].player.push_back(player);
	skillsinfo[Jobs::Noblesse::Recovery].player.push_back(player);
	act.type = ActHeal;
	act.time = 4900;
	act.value = SkillX;
	skillsinfo[Jobs::Beginner::Recovery].act = act;
	skillsinfo[Jobs::Beginner::Recovery].bact = true;
	skillsinfo[Jobs::Noblesse::Recovery].act = act;
	skillsinfo[Jobs::Noblesse::Recovery].bact = true;

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

	std::cout << "DONE" << std::endl;
}

void BuffDataProvider::addItemInfo(int32_t itemid, const ConsumeInfo &cons) {
	using namespace BuffBytes;
	vector<uint8_t> types;
	vector<int8_t> bytes;
	vector<int16_t> values;

	if (cons.watk > 0) {
		types.push_back(0x01);
		bytes.push_back(Byte1);
		values.push_back(cons.watk);
	}
	if (cons.wdef > 0) {
		types.push_back(0x02);
		bytes.push_back(Byte1);
		values.push_back(cons.wdef);
	}
	if (cons.matk > 0) {
		types.push_back(0x04);
		bytes.push_back(Byte1);
		values.push_back(cons.matk);
	}
	if (cons.mdef > 0) {
		types.push_back(0x08);
		bytes.push_back(Byte1);
		values.push_back(cons.mdef);
	}
	if (cons.acc > 0) {
		types.push_back(0x10);
		bytes.push_back(Byte1);
		values.push_back(cons.acc);
	}
	if (cons.avo > 0) {
		types.push_back(0x20);
		bytes.push_back(Byte1);
		values.push_back(cons.avo);
	}
	if (cons.speed > 0) {
		types.push_back(0x80);
		bytes.push_back(Byte1);
		values.push_back(cons.speed);
	}
	if (cons.jump > 0) {
		types.push_back(0x01);
		bytes.push_back(Byte2);
		values.push_back(cons.jump);
	}
	if (cons.morphs.size() > 0) {
		types.push_back(0x02);
		bytes.push_back(Byte5);
		values.push_back(cons.morphs[0].morph); // TEMPORARY HACK UNTIL BUFF SYSTEM IS RESTRUCTURED
	}
	// Need some buff bytes/types for ALL of the following
	if (cons.iceresist > 0) {

	}
	if (cons.fireresist > 0) {

	}
	if (cons.poisonresist > 0) {

	}
	if (cons.lightningresist > 0) {

	}
	if (cons.cursedef > 0) {

	}
	if (cons.stundef > 0) {

	}
	if (cons.weaknessdef > 0) {

	}
	if (cons.darknessdef > 0) {

	}
	if (cons.sealdef > 0) {

	}
	if (cons.ignorewdef) {

	}
	if (cons.ignoremdef) {

	}
	if (cons.mesoup) {

	}
	if (cons.dropup) {
		switch (cons.dropup) {
			case 1: // Regular drop rate increase for all items, the only one I can parse at the moment
				break;
			//case 2: // Specific item drop rate increase
			//case 3: // Specific item range (itemid / 10000) increase
		}
	}

	if (bytes.size()) {
		Buff buff;
		BuffInfo player;
		BuffMapInfo map;

		itemid *= -1;

		if (isBuff(itemid)) { // Already loaded, don't want doubles
			skillsinfo[itemid].player.clear();
			skillsinfo[itemid].map.clear();
		}

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
}
