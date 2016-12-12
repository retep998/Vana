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
#pragma once

#include "Types.hpp"

enum Action {
	ActHeal,
	ActHurt
};

enum SkillValues {
	SkillNone, // Predefined value
	SkillSpecialProc, // Special processing required
	SkillX,
	SkillY,
	SkillSpeed,
	SkillJump,
	SkillWatk,
	SkillWdef,
	SkillMatk,
	SkillMdef,
	SkillAcc,
	SkillAvo,
	SkillProp,
	SkillMorph,
	SkillLv,
	SkillMobCount,
	SkillRange,
	SkillDamage,
};

namespace BuffBytes {
	const int8_t ByteQuantity = 16;
	const int8_t EntryByteQuantity = 8;

	enum ByteTypes : uint8_t {
		Byte13,
		Byte14,
		Byte15,
		Byte16,
		Byte9,
		Byte10,
		Byte11,
		Byte12,
		Byte5,
		Byte6,
		Byte7,
		Byte8,
		Byte1,
		Byte2,
		Byte3,
		Byte4
	};
}

namespace StatusEffects {
	namespace Mob {
		const uint8_t MaxVenomCount = 3;

		enum MobStatus : uint32_t {
			// Groups of 5 for easier counting
			Watk = 0x01,
			Wdef = 0x02,
			Matk = 0x04,
			Mdef = 0x08,
			Acc = 0x10,

			Avoid = 0x20,
			Speed = 0x40,
			Stun = 0x80,
			Freeze = 0x100,
			Poison = 0x200,

			Seal = 0x400,
			NoClue1 = 0x800,
			WeaponAttackUp = 0x1000,
			WeaponDefenseUp = 0x2000,
			MagicAttackUp = 0x4000,

			MagicDefenseUp = 0x8000,
			Doom = 0x10000,
			ShadowWeb = 0x20000,
			WeaponImmunity = 0x40000,
			MagicImmunity = 0x80000,

			NoClue2 = 0x100000,
			NoClue3 = 0x200000,
			NinjaAmbush = 0x400000,
			NoClue4 = 0x800000,
			VenomousWeapon = 0x1000000,

			NoClue5 = 0x2000000,
			NoClue6 = 0x4000000,
			Empty = 0x8000000, // All mobs have this when they spawn
			Hypnotize = 0x10000000,
			WeaponDamageReflect = 0x20000000,

			MagicDamageReflect = 0x40000000,
			NoClue7 = 0x80000000 // Not any more bits you can use with 4 bytes
		};
	}
	namespace Player {
		enum PlayerStatus : int16_t {
			Curse = 0x01,
			Weakness = 0x02,
			Darkness = 0x04,
			Seal = 0x08,
			Poison = 0x10,
			Stun = 0x20,
			Slow = 0x40,
			Seduce = 0x80,
			Zombify = 0x100,
			CrazySkull = 0x200
		};
	}
}

namespace MobSkills {
	enum : mob_skill_id_t {
		WeaponAttackUp = 100,
		WeaponAttackUpAoe = 110,
		MagicAttackUp = 101,
		MagicAttackUpAoe = 111,
		WeaponDefenseUp = 102,
		WeaponDefenseUpAoe = 112,
		MagicDefenseUp = 103,
		MagicDefenseUpAoe = 113,
		HealAoe = 114,
		SpeedUpAoe = 115,
		Seal = 120,
		Darkness = 121,
		Weakness = 122,
		Stun = 123,
		Curse = 124,
		Poison = 125,
		Slow = 126,
		Dispel = 127,
		Seduce = 128,
		SendToTown = 129,
		PoisonMist = 131,
		CrazySkull = 132,
		Zombify = 133,
		WeaponImmunity = 140,
		MagicImmunity = 141,
		ArmorSkill = 142,
		WeaponDamageReflect = 143,
		MagicDamageReflect = 144,
		AnyDamageReflect = 145,
		McWeaponAttackUp = 150,
		McMagicAttackUp = 151,
		McWeaponDefenseUp = 152,
		McMagicDefenseUp = 153,
		McAccuracyUp = 154,
		McAvoidUp = 155,
		McSpeedUp = 156,
		McSeal = 157, // Not actually used in Monster Carnival
		Summon = 200
	};
}

namespace Skills {
	namespace All {
		enum Skills : skill_id_t {
			RegularAttack = 0
		};
	}
	namespace Beginner {
		enum Skills : skill_id_t {
			BlessingOfTheFairy = 12,
			EchoOfHero = 1005,
			FollowTheLead = 8,
			MonsterRider = 1004,
			NimbleFeet = 1002,
			Recovery = 1001,
			ThreeSnails = 1000,
			LegendarySpirit = 1003,
			Maker = 1007,
		};
	}
	namespace Swordsman {
		enum Skills : skill_id_t {
			ImprovedMaxHpIncrease = 1000001,
			IronBody = 1001003,
		};
	}
	namespace Fighter {
		enum Skills : skill_id_t {
			AxeBooster = 1101005,
			AxeMastery = 1100001,
			PowerGuard = 1101007,
			Rage = 1101006,
			SwordBooster = 1101004,
			SwordMastery = 1100000,
		};
	}
	namespace Crusader {
		enum Skills : skill_id_t {
			ArmorCrash = 1111007,
			AxeComa = 1111006,
			AxePanic = 1111004,
			ComboAttack = 1111002,
			Shout = 1111008,
			SwordComa = 1111005,
			SwordPanic = 1111003,
		};
	}
	namespace Hero {
		enum Skills : skill_id_t {
			Achilles = 1120004,
			AdvancedComboAttack = 1120003,
			Enrage = 1121010,
			Guardian = 1120005,
			HerosWill = 1121011,
			MapleWarrior = 1121000,
			MonsterMagnet = 1121001,
			PowerStance = 1121002,
		};
	}
	namespace Page {
		enum Skills : skill_id_t {
			BwBooster = 1201005,
			BwMastery = 1200001,
			PowerGuard = 1201007,
			SwordBooster = 1201004,
			SwordMastery = 1200000,
			Threaten = 1201006,
		};
	}
	namespace WhiteKnight {
		enum Skills : skill_id_t {
			BwFireCharge = 1211004,
			BwIceCharge = 1211006,
			BwLitCharge = 1211008,
			ChargeBlow = 1211002,
			MagicCrash = 1211009,
			SwordFireCharge = 1211003,
			SwordIceCharge = 1211005,
			SwordLitCharge = 1211007,
		};
	}
	namespace Paladin {
		enum Skills : skill_id_t {
			Achilles = 1220005,
			AdvancedCharge = 1220010,
			BwHolyCharge = 1221004,
			Guardian = 1220006,
			HeavensHammer = 1221011,
			HerosWill = 1221012,
			MapleWarrior = 1221000,
			MonsterMagnet = 1221001,
			PowerStance = 1221002,
			SwordHolyCharge = 1221003,
		};
	}
	namespace Spearman {
		enum Skills : skill_id_t {
			HyperBody = 1301007,
			IronWill = 1301006,
			PolearmBooster = 1301005,
			PolearmMastery = 1300001,
			SpearBooster = 1301004,
			SpearMastery = 1300000,
		};
	}
	namespace DragonKnight {
		enum Skills : skill_id_t {
			DragonBlood = 1311008,
			DragonRoar = 1311006,
			ElementalResistance = 1310000,
			PowerCrash = 1311007,
			Sacrifice = 1311005,
		};
	}
	namespace DarkKnight {
		enum Skills : skill_id_t {
			Achilles = 1320005,
			AuraOfBeholder = 1320008,
			Beholder = 1321007,
			Berserk = 1320006,
			HerosWill = 1321010,
			HexOfBeholder = 1320009,
			MapleWarrior = 1321000,
			MonsterMagnet = 1321001,
			PowerStance = 1321002,
		};
	}
	namespace Magician {
		enum Skills : skill_id_t {
			ImprovedMaxMpIncrease = 2000001,
			MagicArmor = 2001003,
			MagicGuard = 2001002,
		};
	}
	namespace FpWizard {
		enum Skills : skill_id_t {
			Meditation = 2101001,
			MpEater = 2100000,
			PoisonBreath = 2101005,
			Slow = 2101003,
		};
	}
	namespace FpMage {
		enum Skills : skill_id_t {
			ElementAmplification = 2110001,
			ElementComposition = 2111006,
			PartialResistance = 2110000,
			PoisonMist = 2111003,
			Seal = 2111004,
			SpellBooster = 2111005,
		};
	}
	namespace FpArchMage {
		enum Skills : skill_id_t {
			BigBang = 2121001,
			Elquines = 2121005,
			FireDemon = 2121003,
			HerosWill = 2121008,
			Infinity = 2121004,
			ManaReflection = 2121002,
			MapleWarrior = 2121000,
			Paralyze = 2121006,
		};
	}
	namespace IlWizard {
		enum Skills : skill_id_t {
			ColdBeam = 2201004,
			Meditation = 2201001,
			MpEater = 2200000,
			Slow = 2201003,
		};
	}
	namespace IlMage {
		enum Skills : skill_id_t {
			ElementAmplification = 2210001,
			ElementComposition = 2211006,
			IceStrike = 2211002,
			PartialResistance = 2210000,
			Seal = 2211004,
			SpellBooster = 2211005,
		};
	}
	namespace IlArchMage {
		enum Skills : skill_id_t {
			BigBang = 2221001,
			Blizzard = 2221007,
			HerosWill = 2221008,
			IceDemon = 2221003,
			Ifrit = 2221005,
			Infinity = 2221004,
			ManaReflection = 2221002,
			MapleWarrior = 2221000,
		};
	}
	namespace Cleric {
		enum Skills : skill_id_t {
			Bless = 2301004,
			Heal = 2301002,
			Invincible = 2301003,
			MpEater = 2300000,
		};
	}
	namespace Priest {
		enum Skills : skill_id_t {
			Dispel = 2311001,
			Doom = 2311005,
			ElementalResistance = 2310000,
			HolySymbol = 2311003,
			MysticDoor = 2311002,
			SummonDragon = 2311006,
		};
	}
	namespace Bishop {
		enum Skills : skill_id_t {
			Bahamut = 2321003,
			BigBang = 2321001,
			HerosWill = 2321009,
			HolyShield = 2321005,
			Infinity = 2321004,
			ManaReflection = 2321002,
			MapleWarrior = 2321000,
			Resurrection = 2321006,
		};
	}
	namespace Archer {
		enum Skills : skill_id_t {
			CriticalShot = 3000001,
			Focus = 3001003,
		};
	}
	namespace Hunter {
		enum Skills : skill_id_t {
			ArrowBomb = 3101005,
			BowBooster = 3101002,
			BowMastery = 3100000,
			SoulArrow = 3101004,
		};
	}
	namespace Ranger {
		enum Skills : skill_id_t {
			MortalBlow = 3110001,
			Puppet = 3111002,
			SilverHawk = 3111005,
		};
	}
	namespace Bowmaster {
		enum Skills : skill_id_t {
			Concentrate = 3121008,
			Hamstring = 3121007,
			HerosWill = 3121009,
			Hurricane = 3121004,
			MapleWarrior = 3121000,
			Phoenix = 3121006,
			SharpEyes = 3121002,
			BowExpert = 3120005,
		};
	}
	namespace Crossbowman {
		enum Skills : skill_id_t {
			CrossbowBooster = 3201002,
			CrossbowMastery = 3200000,
			SoulArrow = 3201004,
		};
	}
	namespace Sniper {
		enum Skills : skill_id_t {
			Blizzard = 3211003,
			GoldenEagle = 3211005,
			MortalBlow = 3210001,
			Puppet = 3211002,
		};
	}
	namespace Marksman {
		enum Skills : skill_id_t {
			Blind = 3221006,
			Frostprey = 3221005,
			HerosWill = 3221008,
			MapleWarrior = 3221000,
			PiercingArrow = 3221001,
			SharpEyes = 3221002,
			Snipe = 3221007,
		};
	}
	namespace Rogue {
		enum Skills : skill_id_t {
			DarkSight = 4001003,
			Disorder = 4001002,
			DoubleStab = 4001334,
			LuckySeven = 4001344,
		};
	}
	namespace Assassin {
		enum Skills : skill_id_t {
			ClawBooster = 4101003,
			ClawMastery = 4100000,
			CriticalThrow = 4100001,
			Drain = 4101005,
			Haste = 4101004,
		};
	}
	namespace Hermit {
		enum Skills : skill_id_t {
			Alchemist = 4110000,
			Avenger = 4111005,
			MesoUp = 4111001,
			ShadowMeso = 4111004,
			ShadowPartner = 4111002,
			ShadowWeb = 4111003,
		};
	}
	namespace NightLord {
		enum Skills : skill_id_t {
			HerosWill = 4121009,
			MapleWarrior = 4121000,
			NinjaAmbush = 4121004,
			NinjaStorm = 4121008,
			ShadowShifter = 4120002,
			ShadowStars = 4121006,
			Taunt = 4121003,
			TripleThrow = 4121007,
			VenomousStar = 4120005,
		};
	}
	namespace Bandit {
		enum Skills : skill_id_t {
			DaggerBooster = 4201002,
			DaggerMastery = 4200000,
			Haste = 4201003,
			SavageBlow = 4201005,
			Steal = 4201004,
		};
	}
	namespace ChiefBandit {
		enum Skills : skill_id_t {
			Assaulter = 4211002,
			BandOfThieves = 4211004,
			Chakra = 4211001,
			MesoExplosion = 4211006,
			MesoGuard = 4211005,
			Pickpocket = 4211003,
		};
	}
	namespace Shadower {
		enum Skills : skill_id_t {
			Assassinate = 4221001,
			BoomerangStep = 4221007,
			HerosWill = 4221008,
			MapleWarrior = 4221000,
			NinjaAmbush = 4221004,
			ShadowShifter = 4220002,
			Smokescreen = 4221006,
			Taunt = 4221003,
			VenomousStab = 4220005,
		};
	}
	namespace Pirate {
		enum Skills : skill_id_t {
			Dash = 5001005,
		};
	}
	namespace Brawler {
		enum Skills : skill_id_t {
			BackspinBlow = 5101002,
			CorkscrewBlow = 5101004,
			DoubleUppercut = 5101003,
			ImproveMaxHp = 5100000,
			KnucklerBooster = 5101006,
			KnucklerMastery = 5100001,
			MpRecovery = 5101005,
			OakBarrel = 5101007,
		};
	}
	namespace Marauder {
		enum Skills : skill_id_t {
			EnergyCharge = 5110001,
			EnergyDrain = 5111004,
			StunMastery = 5110000,
			Transformation = 5111005,
		};
	}
	namespace Buccaneer {
		enum Skills : skill_id_t {
			Demolition = 5121004,
			MapleWarrior = 5121000,
			HerosWill = 5121008,
			Snatch = 5121005,
			SpeedInfusion = 5121009,
			SuperTransformation = 5121003,
			TimeLeap = 5121010,
		};
	}
	namespace Gunslinger {
		enum Skills : skill_id_t {
			BlankShot = 5201004,
			Grenade = 5201002,
			GunBooster = 5201003,
			GunMastery = 5200000,
		};
	}
	namespace Outlaw {
		enum Skills : skill_id_t {
			Flamethrower = 5211004,
			Gaviota = 5211002,
			HomingBeacon = 5211006,
			IceSplitter = 5211005,
			Octopus = 5211001,
		};
	}
	namespace Corsair {
		enum Skills : skill_id_t {
			AerialStrike = 5221003,
			Battleship = 5221006,
			Bullseye = 5220011,
			ElementalBoost = 5220001,
			Hypnotize = 5221009,
			MapleWarrior = 5221000,
			RapidFire = 5221004,
			HerosWill = 5221010,
			WrathOfTheOctopi = 5220002,
		};
	}
	namespace Gm {
		enum Skills : skill_id_t {
			Haste = 9001000,
			SuperDragonRoar = 9001001,
			Teleport = 9001007,
		};
	}
	namespace SuperGm {
		enum Skills : skill_id_t {
			Bless = 9101003,
			Haste = 9101001,
			HealPlusDispel = 9101000,
			Hide = 9101004,
			HolySymbol = 9101002,
			HyperBody = 9101008,
			Resurrection = 9101005,
			SuperDragonRoar = 9101006,
			Teleport = 9101007,
		};
	}
	namespace Noblesse {
		enum Skills : skill_id_t {
			BlessingOfTheFairy = 10000012,
			EchoOfHero = 10001005,
			Maker = 10001007,
			MonsterRider = 10001004,
			NimbleFeet = 10001002,
			Recovery = 10001001,
			ThreeSnails = 10001000,
			FollowTheLead = 10000018,
			LegendarySpirit = 10001003,
		};
	}
	namespace DawnWarrior {
		enum Skills : skill_id_t {
			AdvancedCombo = 11110005,
			Coma = 11111003,
			ComboAttack = 11111001,
			FinalAttack = 11101002,
			IronBody = 11001001,
			MaxHpEnhancement = 11000000,
			Panic = 11111002,
			Rage = 11101003,
			Soul = 11001004,
			SoulBlade = 11101004,
			SoulCharge = 11111007,
			SwordBooster = 11101001,
			SwordMastery = 11100000,
		};
	}
	namespace BlazeWizard {
		enum Skills : skill_id_t {
			ElementalReset = 12101005,
			ElementAmplification = 12110001,
			FireStrike = 12111006,
			Flame = 12001004,
			FlameGear = 12111005,
			Ifrit = 12111004,
			IncreasingMaxMp = 12000000,
			MagicArmor = 12001002,
			MagicGuard = 12001001,
			Meditation = 12101000,
			Seal = 12111002,
			Slow = 12101001,
			SpellBooster = 12101004,
		};
	}
	namespace WindArcher {
		enum Skills : skill_id_t {
			EagleEye = 13111005,
			BowBooster = 13101001,
			BowMastery = 13100000,
			CriticalShot = 13000000,
			FinalAttack = 13101002,
			Focus = 13001002,
			Hurricane = 13111002,
			Puppet = 13111004,
			SoulArrow = 13101003,
			Storm = 13001004,
			WindPiercing = 13111006,
			WindShot = 13111007,
			WindWalk = 13101006,
			BowExpert = 13110003,
		};
	}
	namespace NightWalker {
		enum Skills : skill_id_t {
			Alchemist = 14110003,
			Disorder = 14001002,
			DarkSight = 14001003,
			Darkness = 14001005,
			ClawBooster = 14101002,
			ClawMastery = 14100000,
			CriticalThrow = 14100001,
			Haste = 14101003,
			PoisonBomb = 14111006,
			ShadowPartner = 14111000,
			ShadowWeb = 14111001,
			Vanish = 14100005,
			Vampire = 14101006,
			Venom = 14110004,
		};
	}
	namespace ThunderBreaker {
		enum Skills : skill_id_t {
			CorkscrewBlow = 15101003,
			Dash = 15001003,
			EnergyCharge = 15100004,
			EnergyDrain = 15111001,
			ImproveMaxHp = 15100000,
			KnucklerBooster = 15101002,
			KnucklerMastery = 15100001,
			Lightning = 15001004,
			LightningCharge = 15101006,
			Spark = 15111006,
			SharkWave = 15111007,
			SpeedInfusion = 15111005,
			Transformation = 15111002,
		};
	}
}