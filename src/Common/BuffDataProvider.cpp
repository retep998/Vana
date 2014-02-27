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
#include "BuffDataProvider.hpp"
#include "GameConstants.hpp"
#include "GameLogicUtilities.hpp"
#include "InitializeCommon.hpp"
#include "ItemDataProvider.hpp"
#include <iomanip>
#include <iostream>

auto BuffDataProvider::loadData() -> void {
	using namespace BuffBytes;
	std::cout << std::setw(Initializing::OutputWidth) << std::left << "Initializing Buffs... ";

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
	m_skillInfo[Skills::Fighter::SwordBooster].player.push_back(player);
	m_skillInfo[Skills::Fighter::AxeBooster].player.push_back(player);
	m_skillInfo[Skills::Page::SwordBooster].player.push_back(player);
	m_skillInfo[Skills::Page::BwBooster].player.push_back(player);
	m_skillInfo[Skills::Spearman::SpearBooster].player.push_back(player);
	m_skillInfo[Skills::Spearman::PolearmBooster].player.push_back(player);
	m_skillInfo[Skills::FpMage::SpellBooster].player.push_back(player);
	m_skillInfo[Skills::IlMage::SpellBooster].player.push_back(player);
	m_skillInfo[Skills::Hunter::BowBooster].player.push_back(player);
	m_skillInfo[Skills::Crossbowman::CrossbowBooster].player.push_back(player);
	m_skillInfo[Skills::Assassin::ClawBooster].player.push_back(player);
	m_skillInfo[Skills::Bandit::DaggerBooster].player.push_back(player);
	m_skillInfo[Skills::Brawler::KnucklerBooster].player.push_back(player);
	m_skillInfo[Skills::Gunslinger::GunBooster].player.push_back(player);
	m_skillInfo[Skills::DawnWarrior::SwordBooster].player.push_back(player);
	m_skillInfo[Skills::BlazeWizard::SpellBooster].player.push_back(player);
	m_skillInfo[Skills::WindArcher::BowBooster].player.push_back(player);
	m_skillInfo[Skills::NightWalker::ClawBooster].player.push_back(player);
	m_skillInfo[Skills::ThunderBreaker::KnucklerBooster].player.push_back(player);

	// Speed Infusion
	buff.type = 0x80;
	buff.byte = Byte9;
	buff.value = SkillX;
	player.buff = buff;
	m_skillInfo[Skills::Buccaneer::SpeedInfusion].player.push_back(player);
	m_skillInfo[Skills::ThunderBreaker::SpeedInfusion].player.push_back(player);

	// Maple Warriors
	buff.type = 0x08;
	buff.byte = Byte5;
	buff.value = SkillX;
	player.buff = buff;
	m_skillInfo[Skills::Hero::MapleWarrior].player.push_back(player);
	m_skillInfo[Skills::Paladin::MapleWarrior].player.push_back(player);
	m_skillInfo[Skills::DarkKnight::MapleWarrior].player.push_back(player);
	m_skillInfo[Skills::FpArchMage::MapleWarrior].player.push_back(player);
	m_skillInfo[Skills::IlArchMage::MapleWarrior].player.push_back(player);
	m_skillInfo[Skills::Bishop::MapleWarrior].player.push_back(player);
	m_skillInfo[Skills::Bowmaster::MapleWarrior].player.push_back(player);
	m_skillInfo[Skills::Marksman::MapleWarrior].player.push_back(player);
	m_skillInfo[Skills::NightLord::MapleWarrior].player.push_back(player);
	m_skillInfo[Skills::Shadower::MapleWarrior].player.push_back(player);
	m_skillInfo[Skills::Buccaneer::MapleWarrior].player.push_back(player);
	m_skillInfo[Skills::Corsair::MapleWarrior].player.push_back(player);

	// Magic Guard
	buff.type = 0x02;
	buff.byte = Byte2;
	buff.value = SkillX;
	player.buff = buff;
	m_skillInfo[Skills::Magician::MagicGuard].player.push_back(player);
	m_skillInfo[Skills::BlazeWizard::MagicGuard].player.push_back(player);

	// Magic Armor, Iron Body
	buff.type = 0x02;
	buff.byte = Byte1;
	buff.value = SkillWdef;
	player.buff = buff;
	m_skillInfo[Skills::Magician::MagicArmor].player.push_back(player);
	m_skillInfo[Skills::Swordsman::IronBody].player.push_back(player);
	m_skillInfo[Skills::BlazeWizard::MagicArmor].player.push_back(player);
	m_skillInfo[Skills::DawnWarrior::IronBody].player.push_back(player);

	// Focus
	buff.type = 0x10;
	buff.byte = Byte1;
	buff.value = SkillAcc;
	player.buff = buff;
	m_skillInfo[Skills::Archer::Focus].player.push_back(player);
	m_skillInfo[Skills::WindArcher::Focus].player.push_back(player);
	buff.type = 0x20;
	buff.byte = Byte1;
	buff.value = SkillAvo;
	player.buff = buff;
	m_skillInfo[Skills::Archer::Focus].player.push_back(player);
	m_skillInfo[Skills::WindArcher::Focus].player.push_back(player);

	// Rage
	buff.type = 0x01;
	buff.byte = Byte1;
	buff.value = SkillWatk;
	player.buff = buff;
	m_skillInfo[Skills::Fighter::Rage].player.push_back(player);
	m_skillInfo[Skills::DawnWarrior::Rage].player.push_back(player);
	buff.type = 0x02;
	buff.byte = Byte1;
	buff.value = SkillWdef;
	player.buff = buff;
	m_skillInfo[Skills::Fighter::Rage].player.push_back(player);
	m_skillInfo[Skills::DawnWarrior::Rage].player.push_back(player);

	// Power Guard
	buff.type = 0x10;
	buff.byte = Byte2;
	buff.value = SkillX;
	player.buff = buff;
	m_skillInfo[Skills::Fighter::PowerGuard].player.push_back(player);
	m_skillInfo[Skills::Page::PowerGuard].player.push_back(player);

	// Iron Will
	buff.type = 0x02;
	buff.byte = Byte1;
	buff.value = SkillWdef;
	player.buff = buff;
	m_skillInfo[Skills::Spearman::IronWill].player.push_back(player);
	buff.type = 0x08;
	buff.byte = Byte1;
	buff.value = SkillMdef;
	player.buff = buff;
	m_skillInfo[Skills::Spearman::IronWill].player.push_back(player);

	// Hyper Body, Gm Hyper Body
	buff.type = 0x20;
	buff.byte = Byte2;
	buff.value = SkillX;
	player.buff = buff;
	m_skillInfo[Skills::Spearman::HyperBody].player.push_back(player);
	m_skillInfo[Skills::SuperGm::HyperBody].player.push_back(player);
	buff.type = 0x40;
	buff.byte = Byte2;
	buff.value = SkillY;
	player.buff = buff;
	m_skillInfo[Skills::Spearman::HyperBody].player.push_back(player);
	m_skillInfo[Skills::SuperGm::HyperBody].player.push_back(player);

	// Meditation
	buff.type = 0x04;
	buff.byte = Byte1;
	buff.value = SkillMatk;
	player.buff = buff;
	m_skillInfo[Skills::FpWizard::Meditation].player.push_back(player);
	m_skillInfo[Skills::IlWizard::Meditation].player.push_back(player);
	m_skillInfo[Skills::BlazeWizard::Meditation].player.push_back(player);

	// Invincible
	buff.type = 0x80;
	buff.byte = Byte2;
	buff.value = SkillX;
	player.buff = buff;
	m_skillInfo[Skills::Cleric::Invincible].player.push_back(player);

	// Bless, Gm Bless
	buff.type = 0x01;
	buff.byte = Byte1;
	buff.value = SkillWatk;
	player.buff = buff;
	m_skillInfo[Skills::SuperGm::Bless].player.push_back(player);
	buff.type = 0x02;
	buff.byte = Byte1;
	buff.value = SkillWdef;
	player.buff = buff;
	m_skillInfo[Skills::Cleric::Bless].player.push_back(player);
	m_skillInfo[Skills::SuperGm::Bless].player.push_back(player);
	buff.type = 0x04;
	buff.byte = Byte1;
	buff.value = SkillMatk;
	player.buff = buff;
	m_skillInfo[Skills::SuperGm::Bless].player.push_back(player);
	buff.type = 0x08;
	buff.byte = Byte1;
	buff.value = SkillMdef;
	player.buff = buff;
	m_skillInfo[Skills::Cleric::Bless].player.push_back(player);
	m_skillInfo[Skills::SuperGm::Bless].player.push_back(player);
	buff.type = 0x10;
	buff.byte = Byte1;
	buff.value = SkillAcc;
	player.buff = buff;
	m_skillInfo[Skills::Cleric::Bless].player.push_back(player);
	m_skillInfo[Skills::SuperGm::Bless].player.push_back(player);
	buff.type = 0x20;
	buff.byte = Byte1;
	buff.value = SkillAvo;
	player.buff = buff;
	m_skillInfo[Skills::Cleric::Bless].player.push_back(player);
	m_skillInfo[Skills::SuperGm::Bless].player.push_back(player);

	// GM Hide
	buff.type = 0x01;
	buff.byte = Byte8;
	buff.value = SkillSpecialProc;
	player.buff = buff;
	m_skillInfo[Skills::SuperGm::Hide].player.push_back(player);

	// Meso Guard
	buff.type = 0x10;
	buff.byte = Byte4;
	buff.value = SkillX;
	player.buff = buff;
	m_skillInfo[Skills::ChiefBandit::MesoGuard].player.push_back(player);

	// Sharp Eyes
	buff.type = 0x20;
	buff.byte = Byte5;
	buff.value = SkillSpecialProc;
	player.buff = buff;
	m_skillInfo[Skills::Bowmaster::SharpEyes].player.push_back(player);
	m_skillInfo[Skills::Marksman::SharpEyes].player.push_back(player);

	// Holy Symbol
	buff.type = 0x01;
	buff.byte = Byte4;
	buff.value = SkillX;
	player.buff = buff;
	m_skillInfo[Skills::Priest::HolySymbol].player.push_back(player);
	m_skillInfo[Skills::SuperGm::HolySymbol].player.push_back(player);

	// Enrage
	buff.type = 0x01;
	buff.byte = Byte1;
	buff.value = SkillWatk;
	player.buff = buff;
	m_skillInfo[Skills::Hero::Enrage].player.push_back(player);

	// Concentrate
	buff.type = 0x01;
	buff.byte = Byte1;
	buff.value = SkillWatk;
	player.buff = buff;
	m_skillInfo[Skills::Bowmaster::Concentrate].player.push_back(player);
	buff.type = 0x20;
	buff.byte = Byte6;
	buff.value = SkillX;
	player.buff = buff;
	m_skillInfo[Skills::Bowmaster::Concentrate].player.push_back(player);

	// Pickpocket/Meso Up
	buff.type = 0x08;
	buff.byte = Byte4;
	buff.value = SkillX;
	player.buff = buff;
	m_skillInfo[Skills::ChiefBandit::Pickpocket].player.push_back(player);
	m_skillInfo[Skills::Hermit::MesoUp].player.push_back(player);

	// Infinity
	buff.type = 0x02;
	buff.byte = Byte6;
	buff.value = SkillX;
	player.buff = buff;
	m_skillInfo[Skills::FpArchMage::Infinity].player.push_back(player);
	m_skillInfo[Skills::IlArchMage::Infinity].player.push_back(player);
	m_skillInfo[Skills::Bishop::Infinity].player.push_back(player);

	// Echo of Hero
	buff.type = 0x80;
	buff.byte = Byte6;
	buff.value = SkillX;
	player.buff = buff;
	m_skillInfo[Skills::Beginner::EchoOfHero].player.push_back(player);

	// Power Stance
	buff.type = 0x10;
	buff.byte = Byte5;
	buff.value = SkillProp;
	player.buff = buff;
	m_skillInfo[Skills::Hero::PowerStance].player.push_back(player);
	m_skillInfo[Skills::Paladin::PowerStance].player.push_back(player);
	m_skillInfo[Skills::DarkKnight::PowerStance].player.push_back(player);

	// Mana Reflection
	buff.type = 0x40;
	buff.byte = Byte5;
	buff.value = SkillLv;
	player.buff = buff;
	m_skillInfo[Skills::FpArchMage::ManaReflection].player.push_back(player);
	m_skillInfo[Skills::IlArchMage::ManaReflection].player.push_back(player);
	m_skillInfo[Skills::Bishop::ManaReflection].player.push_back(player);

	// Hamstring
	buff.type = 0x08;
	buff.byte = Byte6;
	buff.value = SkillX;
	player.buff = buff;
	m_skillInfo[Skills::Bowmaster::Hamstring].player.push_back(player);

	// Blind
	buff.type = 0x10;
	buff.byte = Byte6;
	buff.value = SkillX;
	player.buff = buff;
	m_skillInfo[Skills::Marksman::Blind].player.push_back(player);

	// Dragon Roar
	buff.type = 0x02;
	buff.byte = Byte3;
	buff.value = SkillNone;
	player.buff = buff;
	player.itemVal = 1;
	m_skillInfo[Skills::DragonKnight::DragonRoar].player.push_back(player);

	// Holy Shield
	buff.type = 0x04;
	buff.byte = Byte6;
	buff.value = SkillX;
	player.buff = buff;
	m_skillInfo[Skills::Bishop::HolyShield].player.push_back(player);
	// End regular buffs

	// Begin map buffs
	// Nimble Feet
	buff.type = 0x80;
	buff.byte = Byte1;
	buff.value = SkillSpeed;
	player.buff = buff;
	player.hasMapVal = true;
	player.hasMapEntry = true;
	m_skillInfo[Skills::Beginner::NimbleFeet].player.push_back(player);
	m_skillInfo[Skills::Noblesse::NimbleFeet].player.push_back(player);
	map.buff = buff;
	map.useVal = true;
	m_skillInfo[Skills::Beginner::NimbleFeet].map.push_back(map);
	m_skillInfo[Skills::Noblesse::NimbleFeet].map.push_back(map);

	// Shadow Stars
	buff.type = 0x01;
	buff.byte = Byte6;
	buff.value = SkillSpecialProc;
	player.buff = buff;
	player.hasMapVal = true;
	player.hasMapEntry = false;
	m_skillInfo[Skills::NightLord::ShadowStars].player.push_back(player);
	buff.type = 0x00;
	map.buff = buff;
	map.useVal = false;
	m_skillInfo[Skills::NightLord::ShadowStars].map.push_back(map);

	// Skill Charges
	buff.type = 0x04;
	buff.byte = Byte1;
	buff.value = SkillMatk;
	player.buff = buff;
	player.hasMapVal = false;
	player.hasMapEntry = false;
	m_skillInfo[Skills::WhiteKnight::BwFireCharge].player.push_back(player);
	m_skillInfo[Skills::WhiteKnight::BwIceCharge].player.push_back(player);
	m_skillInfo[Skills::WhiteKnight::BwLitCharge].player.push_back(player);
	m_skillInfo[Skills::WhiteKnight::SwordFireCharge].player.push_back(player);
	m_skillInfo[Skills::WhiteKnight::SwordIceCharge].player.push_back(player);
	m_skillInfo[Skills::WhiteKnight::SwordLitCharge].player.push_back(player);
	m_skillInfo[Skills::Paladin::BwHolyCharge].player.push_back(player);
	m_skillInfo[Skills::Paladin::SwordHolyCharge].player.push_back(player);
	m_skillInfo[Skills::DawnWarrior::SoulCharge].player.push_back(player);
	m_skillInfo[Skills::ThunderBreaker::LightningCharge].player.push_back(player);
	buff.type = 0x40;
	buff.byte = Byte3;
	buff.value = SkillMatk;
	player.buff = buff;
	player.hasMapVal = true;
	player.hasMapEntry = true;
	m_skillInfo[Skills::WhiteKnight::BwFireCharge].player.push_back(player);
	m_skillInfo[Skills::WhiteKnight::BwIceCharge].player.push_back(player);
	m_skillInfo[Skills::WhiteKnight::BwLitCharge].player.push_back(player);
	m_skillInfo[Skills::WhiteKnight::SwordFireCharge].player.push_back(player);
	m_skillInfo[Skills::WhiteKnight::SwordIceCharge].player.push_back(player);
	m_skillInfo[Skills::WhiteKnight::SwordLitCharge].player.push_back(player);
	m_skillInfo[Skills::Paladin::BwHolyCharge].player.push_back(player);
	m_skillInfo[Skills::Paladin::SwordHolyCharge].player.push_back(player);
	m_skillInfo[Skills::DawnWarrior::SoulCharge].player.push_back(player);
	m_skillInfo[Skills::ThunderBreaker::LightningCharge].player.push_back(player);
	map.buff = buff;
	map.useVal = true;
	m_skillInfo[Skills::WhiteKnight::BwFireCharge].map.push_back(map);
	m_skillInfo[Skills::WhiteKnight::BwIceCharge].map.push_back(map);
	m_skillInfo[Skills::WhiteKnight::BwLitCharge].map.push_back(map);
	m_skillInfo[Skills::WhiteKnight::SwordFireCharge].map.push_back(map);
	m_skillInfo[Skills::WhiteKnight::SwordIceCharge].map.push_back(map);
	m_skillInfo[Skills::WhiteKnight::SwordLitCharge].map.push_back(map);
	m_skillInfo[Skills::Paladin::BwHolyCharge].map.push_back(map);
	m_skillInfo[Skills::Paladin::SwordHolyCharge].map.push_back(map);
	m_skillInfo[Skills::DawnWarrior::SoulCharge].map.push_back(map);
	m_skillInfo[Skills::ThunderBreaker::LightningCharge].map.push_back(map);

	// Wind Walk
	buff.type = 0x80;
	buff.byte = Byte1;
	buff.value = SkillSpeed;
	player.buff = buff;
	player.hasMapVal = true;
	player.hasMapEntry = true;
	m_skillInfo[Skills::WindArcher::WindWalk].player.push_back(player);
	map.buff = buff;
	map.useVal = true;
	m_skillInfo[Skills::WindArcher::WindWalk].map.push_back(map);

	// Dash
	buff.type = 0x10;
	buff.byte = Byte9;
	buff.value = SkillX;
	player.buff = buff;
	player.hasMapVal = true;
	m_skillInfo[Skills::Pirate::Dash].player.push_back(player);
	m_skillInfo[Skills::ThunderBreaker::Dash].player.push_back(player);
	buff.type = 0x20;
	map.buff = buff;
	map.useVal = true;
	m_skillInfo[Skills::Pirate::Dash].map.push_back(map);
	m_skillInfo[Skills::ThunderBreaker::Dash].map.push_back(map);
	buff.type = 0x20;
	buff.byte = Byte9;
	buff.value = SkillY;
	player.buff = buff;
	player.hasMapVal = true;
	m_skillInfo[Skills::Pirate::Dash].player.push_back(player);
	m_skillInfo[Skills::ThunderBreaker::Dash].player.push_back(player);
	buff.type = 0x40;
	map.buff = buff;
	map.useVal = true;
	m_skillInfo[Skills::Pirate::Dash].map.push_back(map);
	m_skillInfo[Skills::ThunderBreaker::Dash].map.push_back(map);

	// Haste
	buff.type = 0x80;
	buff.byte = Byte1;
	buff.value = SkillSpeed;
	player.buff = buff;
	player.hasMapVal = true;
	player.hasMapEntry = true;
	m_skillInfo[Skills::Assassin::Haste].player.push_back(player);
	m_skillInfo[Skills::Bandit::Haste].player.push_back(player);
	m_skillInfo[Skills::Gm::Haste].player.push_back(player);
	m_skillInfo[Skills::SuperGm::Haste].player.push_back(player);
	m_skillInfo[Skills::NightWalker::Haste].player.push_back(player);
	map.buff = buff;
	map.useVal = true;
	m_skillInfo[Skills::Assassin::Haste].map.push_back(map);
	m_skillInfo[Skills::Bandit::Haste].map.push_back(map);
	m_skillInfo[Skills::Gm::Haste].map.push_back(map);
	m_skillInfo[Skills::SuperGm::Haste].map.push_back(map);
	m_skillInfo[Skills::NightWalker::Haste].map.push_back(map);
	buff.type = 0x01;
	buff.byte = Byte2;
	buff.value = SkillJump;
	player.buff = buff;
	player.hasMapVal = false;
	player.hasMapEntry = false;
	m_skillInfo[Skills::Assassin::Haste].player.push_back(player);
	m_skillInfo[Skills::Bandit::Haste].player.push_back(player);
	m_skillInfo[Skills::Gm::Haste].player.push_back(player);
	m_skillInfo[Skills::SuperGm::Haste].player.push_back(player);
	m_skillInfo[Skills::NightWalker::Haste].player.push_back(player);

	// Dark Sight
	buff.type = 0x80;
	buff.byte = Byte1;
	buff.value = SkillSpeed;
	player.buff = buff;
	player.hasMapVal = true;
	player.hasMapEntry = true;
	m_skillInfo[Skills::Rogue::DarkSight].player.push_back(player);
	m_skillInfo[Skills::NightWalker::DarkSight].player.push_back(player);
	map.buff = buff;
	map.useVal = true;
	m_skillInfo[Skills::Rogue::DarkSight].map.push_back(map);
	m_skillInfo[Skills::NightWalker::DarkSight].map.push_back(map);
	buff.type = 0x04;
	buff.byte = Byte2;
	buff.value = SkillX;
	player.buff = buff;
	player.hasMapVal = true;
	player.hasMapEntry = true;
	m_skillInfo[Skills::Rogue::DarkSight].player.push_back(player);
	m_skillInfo[Skills::NightWalker::DarkSight].player.push_back(player);
	map.buff = buff;
	map.useVal = false;
	m_skillInfo[Skills::Rogue::DarkSight].map.push_back(map);
	m_skillInfo[Skills::NightWalker::DarkSight].map.push_back(map);

	// Soul Arrow
	buff.type = 0x01;
	buff.byte = Byte3;
	buff.value = SkillX;
	player.buff = buff;
	player.hasMapVal = true;
	player.hasMapEntry = true;
	m_skillInfo[Skills::Hunter::SoulArrow].player.push_back(player);
	m_skillInfo[Skills::Crossbowman::SoulArrow].player.push_back(player);
	m_skillInfo[Skills::WindArcher::SoulArrow].player.push_back(player);
	map.buff = buff;
	map.useVal = false;
	m_skillInfo[Skills::Hunter::SoulArrow].map.push_back(map);
	m_skillInfo[Skills::Crossbowman::SoulArrow].map.push_back(map);
	m_skillInfo[Skills::WindArcher::SoulArrow].map.push_back(map);

	// Energy Charge
	buff.type = 0x08;
	buff.byte = Byte9;
	buff.value = SkillSpecialProc;
	player.buff = buff;
	player.hasMapVal = true;
	player.hasMapEntry = true;
	m_skillInfo[Skills::Marauder::EnergyCharge].player.push_back(player);
	m_skillInfo[Skills::ThunderBreaker::EnergyCharge].player.push_back(player);
	map.buff = buff;
	map.useVal = false;
	m_skillInfo[Skills::Marauder::EnergyCharge].map.push_back(map);
	m_skillInfo[Skills::ThunderBreaker::EnergyCharge].map.push_back(map);

	// Oak Barrel
	buff.type = 0x02;
	buff.byte = Byte5;
	buff.value = SkillMorph;
	player.buff = buff;
	player.hasMapVal = true;
	player.hasMapEntry = true;
	m_skillInfo[Skills::Brawler::OakBarrel].player.push_back(player);
	map.buff = buff;
	map.useVal = true;
	m_skillInfo[Skills::Brawler::OakBarrel].map.push_back(map);

	// Transformation, Super Transformation
	buff.type = 0x02;
	buff.byte = Byte1;
	buff.value = SkillWdef;
	player.buff = buff;
	player.hasMapVal = false;
	player.hasMapEntry = false;
	m_skillInfo[Skills::Marauder::Transformation].player.push_back(player);
	m_skillInfo[Skills::Buccaneer::SuperTransformation].player.push_back(player);
	m_skillInfo[Skills::WindArcher::EagleEye].player.push_back(player);
	m_skillInfo[Skills::ThunderBreaker::Transformation].player.push_back(player);
	buff.type = 0x08;
	buff.byte = Byte1;
	buff.value = SkillMdef;
	player.buff = buff;
	player.hasMapVal = false;
	player.hasMapEntry = false;
	m_skillInfo[Skills::Marauder::Transformation].player.push_back(player);
	m_skillInfo[Skills::Buccaneer::SuperTransformation].player.push_back(player);
	m_skillInfo[Skills::WindArcher::EagleEye].player.push_back(player);
	m_skillInfo[Skills::ThunderBreaker::Transformation].player.push_back(player);
	buff.type = 0x80;
	buff.byte = Byte1;
	buff.value = SkillSpeed;
	player.buff = buff;
	player.hasMapVal = true;
	player.hasMapEntry = true;
	m_skillInfo[Skills::Marauder::Transformation].player.push_back(player);
	m_skillInfo[Skills::Buccaneer::SuperTransformation].player.push_back(player);
	m_skillInfo[Skills::WindArcher::EagleEye].player.push_back(player);
	m_skillInfo[Skills::ThunderBreaker::Transformation].player.push_back(player);
	map.buff = buff;
	map.useVal = true;
	m_skillInfo[Skills::Marauder::Transformation].map.push_back(map);
	m_skillInfo[Skills::Buccaneer::SuperTransformation].map.push_back(map);
	m_skillInfo[Skills::WindArcher::EagleEye].map.push_back(map);
	m_skillInfo[Skills::ThunderBreaker::Transformation].map.push_back(map);
	buff.type = 0x01;
	buff.byte = Byte2;
	buff.value = SkillJump;
	player.buff = buff;
	player.hasMapVal = false;
	player.hasMapEntry = false;
	m_skillInfo[Skills::Marauder::Transformation].player.push_back(player);
	m_skillInfo[Skills::Buccaneer::SuperTransformation].player.push_back(player);
	m_skillInfo[Skills::WindArcher::EagleEye].player.push_back(player);
	m_skillInfo[Skills::ThunderBreaker::Transformation].player.push_back(player);
	buff.type = 0x02;
	buff.byte = Byte5;
	buff.value = SkillMorph;
	player.buff = buff;
	player.hasMapVal = true;
	player.hasMapEntry = true;
	m_skillInfo[Skills::Marauder::Transformation].player.push_back(player);
	m_skillInfo[Skills::Buccaneer::SuperTransformation].player.push_back(player);
	m_skillInfo[Skills::WindArcher::EagleEye].player.push_back(player);
	m_skillInfo[Skills::ThunderBreaker::Transformation].player.push_back(player);
	map.buff = buff;
	map.useVal = true;
	m_skillInfo[Skills::Marauder::Transformation].map.push_back(map);
	m_skillInfo[Skills::Buccaneer::SuperTransformation].map.push_back(map);
	m_skillInfo[Skills::WindArcher::EagleEye].map.push_back(map);
	m_skillInfo[Skills::ThunderBreaker::Transformation].map.push_back(map);

	// Shadow Partner
	buff.type = 0x04;
	buff.byte = Byte4;
	buff.value = SkillSpecialProc;
	player.buff = buff;
	player.hasMapVal = true;
	player.hasMapEntry = true;
	m_skillInfo[Skills::Hermit::ShadowPartner].player.push_back(player);
	m_skillInfo[Skills::NightWalker::ShadowPartner].player.push_back(player);
	map.buff = buff;
	map.useVal = false;
	m_skillInfo[Skills::Hermit::ShadowPartner].map.push_back(map);
	m_skillInfo[Skills::NightWalker::ShadowPartner].map.push_back(map);

	// Combo Attack
	buff.type = 0x20;
	buff.byte = Byte3;
	buff.value = SkillSpecialProc;
	player.buff = buff;
	player.hasMapVal = true;
	player.hasMapEntry = true;
	m_skillInfo[Skills::Crusader::ComboAttack].player.push_back(player);
	m_skillInfo[Skills::DawnWarrior::ComboAttack].player.push_back(player);
	map.buff = buff;
	map.useVal = true;
	m_skillInfo[Skills::Crusader::ComboAttack].map.push_back(map);
	m_skillInfo[Skills::DawnWarrior::ComboAttack].map.push_back(map);
	// End map buffs

	// Begin mount buffs
	player = BuffInfo(); // Placed to clear any previous value pushes

	// Monster Rider
	buff.type = 0x40;
	buff.byte = Byte9;
	buff.value = SkillSpecialProc;
	player.buff = buff;
	player.hasMapVal = true;
	m_skillInfo[Skills::Beginner::MonsterRider].player.push_back(player);
	m_skillInfo[Skills::Noblesse::MonsterRider].player.push_back(player);
	buff.type = 0x40;
	map.buff = buff;
	m_skillInfo[Skills::Beginner::MonsterRider].map.push_back(map);
	m_skillInfo[Skills::Noblesse::MonsterRider].map.push_back(map);

	// Battleship
	buff.type = 0x40;
	buff.byte = Byte9;
	buff.value = SkillSpecialProc;
	player.buff = buff;
	player.hasMapVal = true;
	m_skillInfo[Skills::Corsair::Battleship].player.push_back(player);
	buff.type = 0x40;
	map.buff = buff;
	m_skillInfo[Skills::Corsair::Battleship].map.push_back(map);
	// End mount buffs

	// Begin very unusual buffs
	player = BuffInfo(); // Placed to clear any previous value pushes

	// Homing Beacon/Bullseye
	buff.type = 0x01;
	buff.byte = Byte10;
	buff.value = SkillNone;
	player.buff = buff;
	player.itemVal = 1;
	m_skillInfo[Skills::Outlaw::HomingBeacon].player.push_back(player);
	m_skillInfo[Skills::Corsair::Bullseye].player.push_back(player);
	// End very unusual buffs

	// Begin act buffs
	// Recovery
	buff.type = 0x04;
	buff.byte = Byte5;
	buff.value = SkillX;
	player.buff = buff;
	m_skillInfo[Skills::Beginner::Recovery].player.push_back(player);
	m_skillInfo[Skills::Noblesse::Recovery].player.push_back(player);
	act.type = ActHeal;
	act.time = 4900;
	act.value = SkillX;
	m_skillInfo[Skills::Beginner::Recovery].act = act;
	m_skillInfo[Skills::Beginner::Recovery].hasAction = true;
	m_skillInfo[Skills::Noblesse::Recovery].act = act;
	m_skillInfo[Skills::Noblesse::Recovery].hasAction = true;

	// Dragon Blood
	buff.type = 0x01;
	buff.byte = Byte1;
	buff.value = SkillWatk;
	player.buff = buff;
	m_skillInfo[Skills::DragonKnight::DragonBlood].player.push_back(player);
	buff.type = 0x80;
	buff.byte = Byte3;
	buff.value = SkillLv;
	player.buff = buff;
	m_skillInfo[Skills::DragonKnight::DragonBlood].player.push_back(player);
	act.type = ActHurt;
	act.time = 4000;
	act.value = SkillX;
	m_skillInfo[Skills::DragonKnight::DragonBlood].act = act;
	m_skillInfo[Skills::DragonKnight::DragonBlood].hasAction = true;
	// End act buffs

	// Debuffs
	// Stun
	buff.type = 0x02;
	buff.byte = Byte3;
	buff.value = SkillNone;
	player.buff = buff;
	m_mobSkillInfo[MobSkills::Stun].mob.push_back(player);
	m_mobSkillInfo[MobSkills::Stun].delay = 0;

	// Poison
	buff.type = 0x04;
	buff.byte = Byte3;
	buff.value = SkillX;
	player.buff = buff;
	m_mobSkillInfo[MobSkills::Poison].mob.push_back(player);
	m_mobSkillInfo[MobSkills::Poison].delay = 500;

	// Seal
	buff.type = 0x08;
	buff.byte = Byte3;
	buff.value = SkillNone;
	player.buff = buff;
	m_mobSkillInfo[MobSkills::Seal].mob.push_back(player);
	m_mobSkillInfo[MobSkills::Seal].delay = 900;

	// Darkness
	buff.type = 0x10;
	buff.byte = Byte3;
	buff.value = SkillNone;
	player.buff = buff;
	m_mobSkillInfo[MobSkills::Darkness].mob.push_back(player);
	m_mobSkillInfo[MobSkills::Darkness].delay = 900;

	// Weakness
	buff.type = 0x40;
	buff.byte = Byte4;
	buff.value = SkillNone;
	player.buff = buff;
	m_mobSkillInfo[MobSkills::Weakness].mob.push_back(player);
	m_mobSkillInfo[MobSkills::Weakness].delay = 900;

	// Curse
	buff.type = 0x80;
	buff.byte = Byte4;
	buff.value = SkillNone;
	player.buff = buff;
	m_mobSkillInfo[MobSkills::Curse].mob.push_back(player);
	m_mobSkillInfo[MobSkills::Curse].delay = 900;

	// Slow
	buff.type = 0x01;
	buff.byte = Byte5;
	buff.value = SkillX;
	player.buff = buff;
	m_mobSkillInfo[MobSkills::Slow].mob.push_back(player);
	m_mobSkillInfo[MobSkills::Slow].delay = 900;

	// Seduce
	buff.type = 0x80;
	buff.byte = Byte5;
	buff.value = SkillX;
	player.buff = buff;
	m_mobSkillInfo[MobSkills::Seduce].mob.push_back(player);
	m_mobSkillInfo[MobSkills::Seduce].delay = 900;

	// Zombify
	buff.type = 0x40;
	buff.byte = Byte6;
	buff.value = SkillX;
	player.buff = buff;
	m_mobSkillInfo[MobSkills::Zombify].mob.push_back(player);
	m_mobSkillInfo[MobSkills::Zombify].delay = 900;

	// Crazy Skull
	buff.type = 0x08;
	buff.byte = Byte7;
	buff.value = SkillX;
	player.buff = buff;
	m_mobSkillInfo[MobSkills::CrazySkull].mob.push_back(player);
	m_mobSkillInfo[MobSkills::CrazySkull].delay = 900;

	std::cout << "DONE" << std::endl;
}

auto BuffDataProvider::addItemInfo(item_id_t itemId, const ConsumeInfo &cons) -> void {
	using namespace BuffBytes;
	vector_t<uint8_t> types;
	vector_t<int8_t> bytes;
	vector_t<int16_t> values;

	if (cons.wAtk > 0) {
		types.push_back(0x01);
		bytes.push_back(Byte1);
		values.push_back(cons.wAtk);
	}
	if (cons.wDef > 0) {
		types.push_back(0x02);
		bytes.push_back(Byte1);
		values.push_back(cons.wDef);
	}
	if (cons.mAtk > 0) {
		types.push_back(0x04);
		bytes.push_back(Byte1);
		values.push_back(cons.mAtk);
	}
	if (cons.mDef > 0) {
		types.push_back(0x08);
		bytes.push_back(Byte1);
		values.push_back(cons.mDef);
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
	if (cons.iceResist > 0) {

	}
	if (cons.fireResist > 0) {

	}
	if (cons.poisonResist > 0) {

	}
	if (cons.lightningResist > 0) {

	}
	if (cons.curseDef > 0) {

	}
	if (cons.stunDef > 0) {

	}
	if (cons.weaknessDef > 0) {

	}
	if (cons.darknessDef > 0) {

	}
	if (cons.sealDef > 0) {

	}
	if (cons.ignoreWdef) {

	}
	if (cons.ignoreMdef) {

	}
	if (cons.mesoUp) {

	}
	if (cons.dropUp) {
		switch (cons.dropUp) {
			case 1: // Regular drop rate increase for all items, the only one I can parse at the moment
				break;
			//case 2: // Specific item drop rate increase
			//case 3: // Specific item range (itemId / 10000) increase
		}
	}

	if (bytes.size()) {
		Buff buff;
		BuffInfo player;
		BuffMapInfo map;

		itemId *= -1;

		if (isBuff(itemId)) {
			// Already loaded, don't want doubles
			m_skillInfo[itemId].player.clear();
			m_skillInfo[itemId].map.clear();
		}

		for (size_t i = 0; i < types.size(); i++) {
			buff = Buff();
			player = BuffInfo();
			map = BuffMapInfo();
			buff.byte = bytes[i];
			buff.type = types[i];
			buff.value = SkillNone;
			player.buff = buff;
			player.itemVal = values[i];
			player.useVal = true;
			if ((buff.byte == Byte1 && (buff.type & 0x80) > 0) || buff.byte == Byte5) {
				player.hasMapVal = true;
				player.hasMapEntry = true;
				m_skillInfo[itemId].player.push_back(player);
				map.buff = buff;
				map.useVal = true;
				m_skillInfo[itemId].map.push_back(map);
			}
			else {
				m_skillInfo[itemId].player.push_back(player);
			}
		}
	}
}

auto BuffDataProvider::isBuff(skill_id_t skillId) const -> bool {
	return m_skillInfo.find(skillId) != std::end(m_skillInfo);
}

auto BuffDataProvider::isDebuff(mob_skill_id_t skillId) const -> bool {
	return m_mobSkillInfo.find(skillId) != std::end(m_mobSkillInfo);
}

auto BuffDataProvider::getSkillInfo(skill_id_t skillId) const -> const SkillInfo & {
	return m_skillInfo.find(skillId)->second;
}

auto BuffDataProvider::getMobSkillInfo(mob_skill_id_t skillId) const -> const MobAilmentInfo & {
	return m_mobSkillInfo.find(skillId)->second;
}