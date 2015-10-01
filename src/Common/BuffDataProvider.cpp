/*
Copyright (C) 2008-2015 Vana Development Team

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
#include "BuffSource.hpp"
#include "GameConstants.hpp"
#include "GameLogicUtilities.hpp"
#include "InitializeCommon.hpp"
#include "ItemDataProvider.hpp"
#include <iomanip>
#include <iostream>

namespace Vana {

auto BuffDataProvider::processSkills(Buff value, const init_list_t<skill_id_t> &skills) -> void {
	for (const auto &s : skills) {
		if (m_buffs.find(s) != std::end(m_buffs)) throw std::invalid_argument{"skill is already present"};
		m_buffs[s] = value;
	}
}

auto BuffDataProvider::loadData() -> void {
	std::cout << std::setw(Initializing::OutputWidth) << std::left << "Initializing Buffs... ";

	auto physicalAttack = BuffInfo::fromPlayerOnly(1, BuffSkillValue::Watk);
	auto physicalDefense = BuffInfo::fromPlayerOnly(2, BuffSkillValue::Wdef);
	auto magicAttack = BuffInfo::fromPlayerOnly(3, BuffSkillValue::Matk);
	auto magicDefense = BuffInfo::fromPlayerOnly(4, BuffSkillValue::Mdef);
	auto accuracy = BuffInfo::fromPlayerOnly(5, BuffSkillValue::Accuracy);
	auto avoid = BuffInfo::fromPlayerOnly(6, BuffSkillValue::Avoid);
	auto craft = BuffInfo::fromPlayerOnly(7, BuffSkillValue::Predefined);
	auto speed = BuffInfo::fromMapMovement(8, BuffSkillValue::Speed, BuffMapInfo{1, BuffSkillValue::Speed});
	auto jump = BuffInfo::fromPlayerOnlyMovement(9, BuffSkillValue::Jump);
	auto magicGuard = BuffInfo::fromPlayerOnly(10, BuffSkillValue::X);
	auto darkSight = BuffInfo::fromMapNoMovement(11, BuffSkillValue::X, BuffMapInfo{});
	auto booster = BuffInfo::fromPlayerOnly(12, BuffSkillValue::X);
	auto powerGuard = BuffInfo::fromPlayerOnly(13, BuffSkillValue::X);
	auto hyperBodyHp = BuffInfo::fromPlayerOnly(14, BuffSkillValue::X);
	auto hyperBodyMp = BuffInfo::fromPlayerOnly(15, BuffSkillValue::Y);
	auto invincible = BuffInfo::fromPlayerOnly(16, BuffSkillValue::X);
	auto soulArrow = BuffInfo::fromMapNoMovement(17, BuffSkillValue::X, BuffMapInfo{});
	auto stun = BuffInfo::fromMapMovement(18, 1, BuffMapInfo{4, BuffSkillValue::BitpackedSkillAndLevel32});
	auto poison = BuffInfo::fromMapNoMovement(19, BuffSkillValue::X, BuffMapInfo{BuffSkillValue::SpecialProcessing});
	auto seal = BuffInfo::fromMapNoMovement(20, 2, BuffMapInfo{4, BuffSkillValue::BitpackedSkillAndLevel32});
	auto darkness = BuffInfo::fromMapNoMovement(21, 1, BuffMapInfo{4, BuffSkillValue::BitpackedSkillAndLevel32});
	auto combo = BuffInfo::fromMapNoMovement(22, BuffSkillValue::SpecialProcessing, BuffMapInfo{1, BuffSkillValue::SpecialProcessing});
	auto charge = BuffInfo::fromMapNoMovement(23, BuffSkillValue::Matk, BuffMapInfo{4, BuffSkillValue::SkillId});
	auto timedHurt = BuffInfo::fromPlayerOnly(24, BuffSkillValue::Level);
	auto holySymbol = BuffInfo::fromPlayerOnly(25, BuffSkillValue::X);
	auto mesoUp = BuffInfo::fromPlayerOnly(26, BuffSkillValue::X);
	auto shadowPartner = BuffInfo::fromMapNoMovement(27, BuffSkillValue::BitpackedXy16, BuffMapInfo{});
	auto pickpocket = BuffInfo::fromPlayerOnly(28, BuffSkillValue::X);
	auto mesoGuard = BuffInfo::fromPlayerOnly(29, BuffSkillValue::X);
	// 30 - ???
	auto weakness = BuffInfo::fromMapMovement(31, 1, BuffMapInfo{4, BuffSkillValue::BitpackedSkillAndLevel32});
	auto curse = BuffInfo::fromMapNoMovement(32, 1, BuffMapInfo{4, BuffSkillValue::BitpackedSkillAndLevel32});
	auto slow = BuffInfo::fromPlayerOnlyMovement(33, BuffSkillValue::X);
	auto morph = BuffInfo::fromMapMovement(34, BuffSkillValue::Morph, BuffMapInfo{2, BuffSkillValue::Morph});
	auto timedHeal = BuffInfo::fromPlayerOnly(35, BuffSkillValue::X);
	// No, I have no idea why Maple Warrior is a "movement" buff
	auto mapleWarrior = BuffInfo::fromPlayerOnlyMovement(36, BuffSkillValue::X);
	auto powerStance = BuffInfo::fromPlayerOnly(37, BuffSkillValue::Prop);
	auto sharpEyes = BuffInfo::fromPlayerOnly(38, BuffSkillValue::BitpackedXy16);
	auto manaReflection = BuffInfo::fromPlayerOnly(39, BuffSkillValue::Level);
	auto seduce = BuffInfo::fromMapMovement(40, BuffSkillValue::X, BuffMapInfo{4, BuffSkillValue::BitpackedSkillAndLevel32});
	auto shadowStars = BuffInfo::fromMapNoMovement(41, BuffSkillValue::SpecialProcessing, BuffMapInfo{4, BuffSkillValue::SpecialProcessing});
	auto infinity = BuffInfo::fromPlayerOnly(42, BuffSkillValue::X);
	auto holyShield = BuffInfo::fromPlayerOnly(43, BuffSkillValue::X);
	auto hamstring = BuffInfo::fromPlayerOnly(44, BuffSkillValue::X);
	auto blind = BuffInfo::fromPlayerOnly(45, BuffSkillValue::X);
	auto concentrate = BuffInfo::fromPlayerOnly(46, BuffSkillValue::X);
	auto zombify = BuffInfo::fromMapNoMovement(47, BuffSkillValue::X, BuffMapInfo{4, BuffSkillValue::BitpackedSkillAndLevel32});
	auto echo = BuffInfo::fromPlayerOnly(48, BuffSkillValue::X);
	// 49 - ???
	// 50 - ???
	// 51 - ???
	auto crazySkull = BuffInfo::fromMapNoMovement(52, BuffSkillValue::X, BuffMapInfo{4, BuffSkillValue::BitpackedSkillAndLevel32});
	// 53 - ???
	auto ignoreWeaponImmunity = BuffInfo::fromPlayerOnly(54, BuffSkillValue::Predefined); // Value is a percentage, so 100 means always ignore
	auto ignoreMagicImmunity = BuffInfo::fromPlayerOnly(55, BuffSkillValue::Predefined); // Value is a percentage, so 100 means always ignore
	// 56 - ???
	// ??? - not sure what normally goes here, I use it for GM Hide because it appears to have no ill effects
	auto unk = BuffInfo::fromPlayerOnly(57, BuffSkillValue::SpecialProcessing);
	// 58 - ???
	// 59 - ???
	// 60 - ???
	// 61 - ???
	auto spark = BuffInfo::fromPlayerOnly(62, BuffSkillValue::X);
	// 63 - ???
	auto dawnFinalAttack = BuffInfo::fromPlayerOnly(64, 1);
	auto windFinalAttack = BuffInfo::fromPlayerOnly(65, 1);
	auto elementalReset = BuffInfo::fromPlayerOnly(66, BuffSkillValue::X);
	auto windWalk = BuffInfo::fromMapNoMovement(67, 1, BuffMapInfo{});
	auto energyCharge = BuffInfo::fromMapNoMovement(68, BuffSkillValue::SpecialPacket, BuffMapInfo{BuffSkillValue::SpecialPacket});
	auto dashSpeed = BuffInfo::fromMapMovement(69, BuffSkillValue::SpecialPacket, BuffMapInfo{BuffSkillValue::SpecialPacket});
	auto dashJump = BuffInfo::fromMapMovement(70, BuffSkillValue::SpecialPacket, BuffMapInfo{BuffSkillValue::SpecialPacket});
	auto mount = BuffInfo::fromMapMovement(71, BuffSkillValue::SpecialPacket, BuffMapInfo{BuffSkillValue::SpecialPacket});
	auto speedInfusion = BuffInfo::fromPlayerOnly(72, BuffSkillValue::SpecialPacket);
	auto homingBeacon = BuffInfo::fromPlayerOnly(73, BuffSkillValue::SpecialPacket);

	processSkills(
		Buff{{
			magicGuard,
		}}, {
			Skills::Magician::MagicGuard,
			Skills::BlazeWizard::MagicGuard,
		});

	processSkills(
		Buff{{
			speed,
			darkSight,
		}}, {
			Skills::Rogue::DarkSight,
			Skills::NightWalker::DarkSight,
		});

	processSkills(
		Buff{{
			booster,
		}}, {
			Skills::Fighter::SwordBooster,
			Skills::Fighter::AxeBooster,
			Skills::Page::SwordBooster,
			Skills::Page::BwBooster,
			Skills::Spearman::SpearBooster,
			Skills::Spearman::PolearmBooster,
			Skills::FpMage::SpellBooster,
			Skills::IlMage::SpellBooster,
			Skills::Hunter::BowBooster,
			Skills::Crossbowman::CrossbowBooster,
			Skills::Assassin::ClawBooster,
			Skills::Bandit::DaggerBooster,
			Skills::Brawler::KnucklerBooster,
			Skills::Gunslinger::GunBooster,
			Skills::DawnWarrior::SwordBooster,
			Skills::BlazeWizard::SpellBooster,
			Skills::WindArcher::BowBooster,
			Skills::NightWalker::ClawBooster,
			Skills::ThunderBreaker::KnucklerBooster,
		});

	processSkills(
		Buff{{
			powerGuard,
		}}, {
			Skills::Fighter::PowerGuard,
			Skills::Page::PowerGuard,
		});

	processSkills(
		Buff{{
			hyperBodyHp,
			hyperBodyMp,
		}}, {
			Skills::Spearman::HyperBody,
			Skills::SuperGm::HyperBody,
		});

	processSkills(
		Buff{{
			invincible,
		}}, {
			Skills::Cleric::Invincible,
		});

	processSkills(
		Buff{{
			soulArrow,
		}}, {
			Skills::Hunter::SoulArrow,
			Skills::Crossbowman::SoulArrow,
			Skills::WindArcher::SoulArrow,
		});

	processSkills(
		Buff{{
			combo,
		}}, {
			Skills::Crusader::ComboAttack,
			Skills::DawnWarrior::ComboAttack,
		});

	processSkills(
		Buff{{
			magicAttack,
			charge,
		}}, {
			Skills::WhiteKnight::BwFireCharge,
			Skills::WhiteKnight::BwIceCharge,
			Skills::WhiteKnight::BwLitCharge,
			Skills::WhiteKnight::SwordFireCharge,
			Skills::WhiteKnight::SwordIceCharge,
			Skills::WhiteKnight::SwordLitCharge,
			Skills::Paladin::BwHolyCharge,
			Skills::Paladin::SwordHolyCharge,
			Skills::DawnWarrior::SoulCharge,
			Skills::ThunderBreaker::LightningCharge,
		});

	processSkills(
		Buff{{
			physicalAttack,
			timedHurt.
				withAct(BuffAction::Hurt, BuffSkillValue::X, seconds_t{4}),
		}}, {
			Skills::DragonKnight::DragonBlood,
		});

	processSkills(
		Buff{{
			holySymbol,
		}}, {
			Skills::Priest::HolySymbol,
			Skills::SuperGm::HolySymbol,
		});

	processSkills(
		Buff{{
			mesoUp,
		}}, {
			Skills::Hermit::MesoUp,
		});

	processSkills(
		Buff{{
			shadowPartner,
		}}, {
			Skills::Hermit::ShadowPartner,
			Skills::NightWalker::ShadowPartner,
		});

	processSkills(
		Buff{{
			pickpocket,
		}}, {
			Skills::ChiefBandit::Pickpocket,
		});

	processSkills(
		Buff{{
			mesoGuard,
		}}, {
			Skills::ChiefBandit::MesoGuard,
		});

	processSkills(
		Buff{{
			timedHeal.
				withAct(BuffAction::Heal, BuffSkillValue::X, milliseconds_t{4900}),
		}}, {
			Skills::Beginner::Recovery,
			Skills::Noblesse::Recovery,
		});

	processSkills(
		Buff{{
			mapleWarrior,
		}}, {
			Skills::Hero::MapleWarrior,
			Skills::Paladin::MapleWarrior,
			Skills::DarkKnight::MapleWarrior,
			Skills::FpArchMage::MapleWarrior,
			Skills::IlArchMage::MapleWarrior,
			Skills::Bishop::MapleWarrior,
			Skills::Bowmaster::MapleWarrior,
			Skills::Marksman::MapleWarrior,
			Skills::NightLord::MapleWarrior,
			Skills::Shadower::MapleWarrior,
			Skills::Buccaneer::MapleWarrior,
			Skills::Corsair::MapleWarrior,
		});

	processSkills(
		Buff{{
			powerStance,
		}}, {
			Skills::Hero::PowerStance,
			Skills::Paladin::PowerStance,
			Skills::DarkKnight::PowerStance,
		});

	processSkills(
		Buff{{
			sharpEyes,
		}}, {
			Skills::Bowmaster::SharpEyes,
			Skills::Marksman::SharpEyes,
		});

	processSkills(
		Buff{{
			manaReflection,
		}}, {
			Skills::FpArchMage::ManaReflection,
			Skills::IlArchMage::ManaReflection,
			Skills::Bishop::ManaReflection,
		});

	processSkills(
		Buff{{
			shadowStars,
		}}, {
			Skills::NightLord::ShadowStars,
		});

	processSkills(
		Buff{{
			infinity,
		}}, {
			Skills::FpArchMage::Infinity,
			Skills::IlArchMage::Infinity,
			Skills::Bishop::Infinity,
		});

	processSkills(
		Buff{{
			holyShield,
		}}, {
			Skills::Bishop::HolyShield,
		});

	processSkills(
		Buff{{
			hamstring,
		}}, {
			Skills::Bowmaster::Hamstring,
		});

	processSkills(
		Buff{{
			blind,
		}}, {
			Skills::Marksman::Blind,
		});

	processSkills(
		Buff{{
			physicalAttack,
			concentrate,
		}}, {
			Skills::Bowmaster::Concentrate,
		});

	processSkills(
		Buff{{
			physicalAttack,
			echo,
		}}, {
			Skills::Beginner::EchoOfHero,
			Skills::Noblesse::EchoOfHero,
		});

	processSkills(
		Buff{{
			unk,
		}}, {
			Skills::SuperGm::Hide,
		});

	processSkills(
		Buff{{
			spark,
		}}, {
			Skills::ThunderBreaker::Spark,
		});

	processSkills(
		Buff{{
			dawnFinalAttack,
		}}, {
			Skills::DawnWarrior::FinalAttack,
		});

	processSkills(
		Buff{{
			windFinalAttack,
		}}, {
			Skills::WindArcher::FinalAttack,
		});

	processSkills(
		Buff{{
			elementalReset,
		}}, {
			Skills::BlazeWizard::ElementalReset,
		});

	processSkills(
		Buff{{
			speed,
			windWalk,
		}}, {
			Skills::WindArcher::WindWalk,
		});

	processSkills(
		Buff{{
			energyCharge,
		}}, {
			Skills::Marauder::EnergyCharge,
			Skills::ThunderBreaker::EnergyCharge,
		});

	processSkills(
		Buff{{
			dashSpeed,
			dashJump,
		}}, {
			Skills::Pirate::Dash,
			Skills::ThunderBreaker::Dash,
		});

	processSkills(
		Buff{{
			mount,
		}}, {
			Skills::Beginner::MonsterRider,
			Skills::Noblesse::MonsterRider,
		});

	processSkills(
		Buff{{
			speedInfusion,
		}}, {
			Skills::Buccaneer::SpeedInfusion,
			Skills::ThunderBreaker::SpeedInfusion,
		});

	processSkills(
		Buff{{
			homingBeacon,
		}}, {
			Skills::Outlaw::HomingBeacon,
			Skills::Corsair::Bullseye,
		});

	processSkills(
		Buff{{
			physicalDefense,
		}}, {
			Skills::Magician::MagicArmor,
			Skills::Swordsman::IronBody,
			Skills::BlazeWizard::MagicArmor,
			Skills::DawnWarrior::IronBody,
		});

	processSkills(
		Buff{{
			accuracy,
			avoid,
		}}, {
			Skills::Archer::Focus,
			Skills::WindArcher::Focus,
		});

	processSkills(
		Buff{{
			physicalAttack,
			physicalDefense,
		}}, {
			Skills::Fighter::Rage,
			Skills::DawnWarrior::Rage,
		});

	processSkills(
		Buff{{
			physicalDefense,
			magicDefense,
		}}, {
			Skills::Spearman::IronWill,
		});

	processSkills(
		Buff{{
			magicAttack,
		}}, {
			Skills::FpWizard::Meditation,
			Skills::IlWizard::Meditation,
			Skills::BlazeWizard::Meditation,
		});

	processSkills(
		Buff{{
			physicalAttack,
			physicalDefense,
			magicDefense,
			accuracy,
			avoid,
		}}, {
			Skills::Cleric::Bless,
		});

	processSkills(
		Buff{{
			physicalAttack,
			physicalDefense,
			magicAttack,
			magicDefense,
			accuracy,
			avoid,
		}}, {
			Skills::SuperGm::Bless,
		});

	processSkills(
		Buff{{
			physicalAttack,
		}}, {
			Skills::Hero::Enrage,
		});

	processSkills(
		Buff{{
			stun.
				withPredefinedValue(1).
				withMapInfo({}),
		}}, {
			Skills::DragonKnight::DragonRoar,
		});

	processSkills(
		Buff{{
			speed,
		}}, {
			Skills::Beginner::NimbleFeet,
			Skills::Noblesse::NimbleFeet,
		});

	processSkills(
		Buff{{
			speed,
			jump,
		}}, {
			Skills::Assassin::Haste,
			Skills::Bandit::Haste,
			Skills::NightWalker::Haste,
			Skills::Gm::Haste,
			Skills::SuperGm::Haste,
		});

	processSkills(
		Buff{{
			morph,
		}}, {
			Skills::Brawler::OakBarrel,
		});

	processSkills(
		Buff{{
			physicalDefense,
			magicDefense,
			speed,
			jump,
			morph.
				withValue(BuffSkillValue::GenderSpecificMorph).
				withMapInfo(BuffMapInfo{2, BuffSkillValue::GenderSpecificMorph}),
		}}, {
			Skills::Marauder::Transformation,
			Skills::Buccaneer::SuperTransformation,
			Skills::WindArcher::EagleEye,
			Skills::ThunderBreaker::Transformation,
		});

	processSkills(
		Buff{{
			physicalDefense,
			magicDefense,
			mount,
		}}, {
			Skills::Corsair::Battleship,
		});

	m_mobSkillInfo[MobSkills::Stun] = Buff{stun};
	m_mobSkillInfo[MobSkills::Poison] = Buff{poison};
	m_mobSkillInfo[MobSkills::Seal] = Buff{seal};
	m_mobSkillInfo[MobSkills::Darkness] = Buff{darkness};
	m_mobSkillInfo[MobSkills::Weakness] = Buff{weakness};
	m_mobSkillInfo[MobSkills::Curse] = Buff{curse};
	m_mobSkillInfo[MobSkills::Slow] = Buff{slow};
	m_mobSkillInfo[MobSkills::Seduce] = Buff{seduce};
	m_mobSkillInfo[MobSkills::CrazySkull] = Buff{crazySkull};
	m_mobSkillInfo[MobSkills::Zombify] = Buff{zombify};

	m_basics.physicalAttack = physicalAttack;
	m_basics.physicalDefense = physicalDefense;
	m_basics.magicAttack = magicAttack;
	m_basics.magicDefense = magicDefense;
	m_basics.accuracy = accuracy;
	m_basics.avoid = avoid;
	m_basics.speed = speed;
	m_basics.craft = craft;
	m_basics.jump = jump;
	m_basics.magicGuard = magicGuard;
	m_basics.darkSight = darkSight;
	m_basics.booster = booster;
	m_basics.powerGuard = powerGuard;
	m_basics.hyperBodyHp = hyperBodyHp;
	m_basics.hyperBodyMp = hyperBodyMp;
	m_basics.invincible = invincible;
	m_basics.soulArrow = soulArrow;
	m_basics.stun = stun;
	m_basics.poison = poison;
	m_basics.seal = seal;
	m_basics.darkness = darkness;
	m_basics.combo = combo;
	m_basics.charge = charge;
	m_basics.timedHurt = timedHurt;
	m_basics.holySymbol = holySymbol;
	m_basics.mesoUp = mesoUp;
	m_basics.shadowPartner = shadowPartner;
	m_basics.pickpocket = pickpocket;
	m_basics.mesoGuard = mesoGuard;
	m_basics.weakness = weakness;
	m_basics.curse = curse;
	m_basics.slow = slow;
	m_basics.morph = morph;
	m_basics.timedHeal = timedHeal;
	m_basics.mapleWarrior = mapleWarrior;
	m_basics.powerStance = powerStance;
	m_basics.sharpEyes = sharpEyes;
	m_basics.manaReflection = manaReflection;
	m_basics.seduce = seduce;
	m_basics.shadowStars = shadowStars;
	m_basics.infinity = infinity;
	m_basics.holyShield = holyShield;
	m_basics.hamstring = hamstring;
	m_basics.blind = blind;
	m_basics.concentrate = concentrate;
	m_basics.zombify = zombify;
	m_basics.echo = echo;
	m_basics.crazySkull = crazySkull;
	m_basics.ignoreWeaponImmunity = ignoreWeaponImmunity;
	m_basics.ignoreMagicImmunity = ignoreMagicImmunity;
	m_basics.spark = spark;
	m_basics.dawnWarriorFinalAttack = dawnFinalAttack;
	m_basics.windWalkerFinalAttack = windFinalAttack;
	m_basics.elementalReset = elementalReset;
	m_basics.windWalk = windWalk;
	m_basics.energyCharge = energyCharge;
	m_basics.dashSpeed = dashSpeed;
	m_basics.dashJump = dashJump;
	m_basics.mount = mount;
	m_basics.speedInfusion = speedInfusion;
	m_basics.homingBeacon = homingBeacon;

	std::cout << "DONE" << std::endl;
}

auto BuffDataProvider::addItemInfo(item_id_t itemId, const ConsumeInfo &cons) -> void {
	vector_t<BuffInfo> values;

	if (cons.wAtk > 0) {
		values.push_back(m_basics.physicalAttack.withPredefinedValue(cons.wAtk));
	}
	if (cons.wDef > 0) {
		values.push_back(m_basics.physicalDefense.withPredefinedValue(cons.wDef));
	}
	if (cons.mAtk > 0) {
		values.push_back(m_basics.magicAttack.withPredefinedValue(cons.mAtk));
	}
	if (cons.mDef > 0) {
		values.push_back(m_basics.magicDefense.withPredefinedValue(cons.mDef));
	}
	if (cons.acc > 0) {
		values.push_back(m_basics.accuracy.withPredefinedValue(cons.acc));
	}
	if (cons.avo > 0) {
		values.push_back(m_basics.avoid.withPredefinedValue(cons.avo));
	}
	if (cons.speed > 0) {
		values.push_back(m_basics.speed.
			withPredefinedValue(cons.speed).
			withMapInfo(BuffMapInfo{1, cons.speed}));
	}
	if (cons.jump > 0) {
		values.push_back(m_basics.jump.withPredefinedValue(cons.jump));
	}
	if (cons.morphs.size() > 0) {
		for (const auto &m : cons.morphs) {
			values.push_back(m_basics.morph.
				withPredefinedValue(m.morph).
				withChance(m.chance).
				withMapInfo(BuffMapInfo{2, m.morph}));
		}
	}

	// TODO FIXME buffs
	// Need some buff bytes/types for ALL of the following
	if (cons.preventDrown) {

	}
	if (cons.preventFreeze) {

	}
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
		values.push_back(m_basics.ignoreWeaponImmunity.withPredefinedValue(cons.chance));
	}
	if (cons.ignoreMdef) {
		values.push_back(m_basics.ignoreMagicImmunity.withPredefinedValue(cons.chance));
	}
	if (cons.mesoUp) {

	}
	if (cons.partyDropUp) {

	}
	if (cons.dropUp) {
		switch (cons.dropUp) {
			case 1: // Regular drop rate increase for all items, the only one I can parse at the moment
				break;
			//case 2: // Specific item drop rate increase
			//case 3: // Specific item range (itemId / 10000) increase
		}
	}

	if (values.size() > 0) {
		m_items[itemId] = Buff{values};
	}
}

auto BuffDataProvider::isBuff(const BuffSource &source) const -> bool {
	switch (source.getType()) {
		case BuffSourceType::Skill: return m_buffs.find(source.getSkillId()) != std::end(m_buffs);
		case BuffSourceType::MobSkill: return m_mobSkillInfo.find(source.getMobSkillId()) != std::end(m_mobSkillInfo);
		case BuffSourceType::Item: return m_items.find(source.getItemId()) != std::end(m_items);
	}
	throw NotImplementedException{"BuffSourceType"};
}

auto BuffDataProvider::isDebuff(const BuffSource &source) const -> bool {
	if (source.getType() != BuffSourceType::MobSkill) return false;
	return m_mobSkillInfo.find(source.getMobSkillId()) != std::end(m_mobSkillInfo);
}

auto BuffDataProvider::getInfo(const BuffSource &source) const -> const Buff & {
	switch (source.getType()) {
		case BuffSourceType::Skill: return m_buffs.find(source.getSkillId())->second;
		case BuffSourceType::MobSkill: return m_mobSkillInfo.find(source.getMobSkillId())->second;
		case BuffSourceType::Item: return m_items.find(source.getItemId())->second;
	}
	throw NotImplementedException{"BuffSourceType"};
}

auto BuffDataProvider::getBuffsByEffect() const -> const BuffInfoByEffect & {
	return m_basics;
}

}