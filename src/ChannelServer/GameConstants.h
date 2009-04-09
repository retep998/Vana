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
#ifndef GAMECONSTANTS_H
#define GAMECONSTANTS_H

#include "Types.h"

// Enumerations
enum WeaponTypes {
	Weapon1hSword = 130,
	Weapon1hAxe = 131,
	Weapon1hMace = 132,
	WeaponDagger = 133,
	WeaponWand = 137,
	WeaponStaff = 138,
	Weapon2hSword = 140,
	Weapon2hAxe = 141,
	Weapon2hMace = 142,
	WeaponSpear = 143,
	WeaponPolearm = 144,
	WeaponBow = 145,
	WeaponCrossbow = 146,
	WeaponClaw = 147,
	WeaponKnuckle = 148,
	WeaponGun = 149
};

enum ItemFlags {
	FlagLock = 0x01,
	FlagSpikes = 0x02,
	FlagCold = 0x04
};

enum Act {
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
	SkillLv
};

enum ByteTypes {
	Byte5,
	Byte6,
	Byte7,
	Byte8,
	Byte1,
	Byte2,
	Byte3,
	Byte4
};

enum MobStatus {
	Watk = 0x1,
	Wdef = 0x2,
	Matk = 0x4,
	Mdef = 0x8,
	Acc = 0x10,
	Avoid = 0x20,
	Speed = 0x40,
	Stun = 0x80,
	Freeze = 0x100,
	Poison = 0x200,
	Seal = 0x400,
	// 0x800?
	WeaponAttackUp = 0x1000,
	WeaponDefenseUp = 0x2000,
	MagicAttackUp = 0x4000,
	MagicDefenseUp = 0x8000,
	Doom = 0x10000,
	ShadowWeb = 0x20000,
	WeaponImmunity = 0x40000,
	MagicImmunity = 0x80000
};

// Skills so there are fewer magic numbers
namespace Jobs {
	namespace Beginner {
		enum {
			EchoOfHero = 1005,
			FollowTheLead = 8,
			MonsterRider = 1004,
			NimbleFeet = 1002,
			Recovery = 1001
		};
	};

	namespace Swordsman {
		enum {
			ImprovedMaxHpIncrease = 1000001,
			IronBody = 1000003
		};
	};

	namespace Fighter {
		enum {
			AxeBooster = 1101005,
			AxeMastery = 1100001,
			PowerGuard = 1101007,
			Rage = 1101006,
			SwordBooster = 1101004,
			SwordMastery = 1100000
		};
	};

	namespace Crusader {
		enum {
			ArmorCrash = 1111007,
			AxeComa = 1111006,
			AxePanic = 1111004,
			ComboAttack = 1111002,
			Shout = 1111008,
			SwordComa = 1111005,
			SwordPanic = 1111003
		};
	};

	namespace Hero {
		enum {
			Achilles = 1120004,
			AdvancedComboAttack = 1120003,
			Enrage = 1121010,
			Guardian = 1120005,
			HerosWill = 1121011,
			MapleWarrior = 1121000,
			MonsterMagnet = 1121001,
			PowerStance = 1121002
		};
	};

	namespace Page {
		enum {
			BwBooster = 1201005,
			BwMastery = 1200001,
			PowerGuard = 1201007,
			SwordBooster = 1201004,
			SwordMastery = 1200000,
			Threaten = 1201006
		};
	};

	namespace WhiteKnight {
		enum {
			BwFireCharge = 1211004,
			BwIceCharge = 1211006,
			BwLitCharge = 1211008,
			ChargeBlow = 1211002,
			MagicCrash = 1211009,
			SwordFireCharge = 1211003,
			SwordIceCharge = 1211005,
			SwordLitCharge = 1211007
		};
	};

	namespace Paladin {
		enum {
			Achilles = 1220005,
			AdvancedCharge = 1220010,
			BwHolyCharge = 1221004,
			Guardian = 1220006,
			HeavensHammer = 1221011,
			HerosWill = 1221012,
			MapleWarrior = 1221000,
			MonsterMagnet = 1221001,
			PowerStance = 1221002,
			SwordHolyCharge = 1221003
		};
	};

	namespace Spearman {
		enum {
			HyperBody = 1301007,
			IronWill = 1301006,
			PolearmBooster = 1301005,
			PolearmMastery = 1300001,
			SpearBooster = 1301004,
			SpearMastery = 1300000
		};
	};

	namespace DragonKnight {
		enum {
			DragonBlood = 1311008,
			DragonRoar = 1311006,
			PowerCrash = 1311007,
			Sacrifice = 1311005
		};
	};

	namespace DarkKnight {
		enum {
			Achilles = 1320005,
			AuraOfBeholder = 1320008,
			Beholder = 1321007,
			Berserk = 1320006,
			HerosWill = 1321010,
			HexOfBeholder = 1320009,
			MapleWarrior = 1321000,
			MonsterMagnet = 1321001,
			PowerStance = 1321002
		};
	};

	namespace Magician {
		enum {
			ImprovedMaxMpIncrease = 2000001,
			MagicArmor = 2001003,
			MagicGuard = 2001002
		};
	};

	namespace FPWizard {
		enum {
			Meditation = 2101001,
			MpEater = 2100000,
			PoisonBreath = 2101005,
			Slow = 2101003
		};
	};

	namespace FPMage {
		enum {
			ElementAmplification = 2110001,
			ElementComposition = 2111006,
			PoisonMist = 2111003,
			Seal = 2111004,
			SpellBooster = 2111005
		};
	};

	namespace FPArchMage {
		enum {
			BigBang = 2121001,
			Elquines = 2121005,
			FireDemon = 2121003,
			HerosWill = 2121008,
			Infinity = 2121004,
			ManaReflection = 2121002,
			MapleWarrior = 2121000,
			Paralyze = 2121006
		};
	};

	namespace ILWizard {
		enum {
			ColdBeam = 2201004,
			Meditation = 2201001,
			MpEater = 2200000,
			Slow = 2201003
		};
	};

	namespace ILMage {
		enum {
			ElementAmplification = 2210001,
			ElementComposition = 2211006,
			IceStrike = 2211002,
			Seal = 2211004,
			SpellBooster = 2211005
		};
	};

	namespace ILArchMage {
		enum {
			BigBang = 22210001,
			Blizzard = 2221007,
			HerosWill = 2221008,
			IceDemon = 2221003,
			Ifrit = 2221005,
			Infinity = 2221004,
			ManaReflection = 2221002,
			MapleWarrior = 2221000
		};
	};

	namespace Cleric {
		enum {
			Bless = 2301004,
			Heal = 2301002,
			Invincible = 2301003,
			MpEater = 2300000
		};
	};

	namespace Priest {
		enum {
			Dispel = 2311001,
			Doom = 2311005,
			HolySymbol = 2311003,
			MysticDoor = 2311002,
			SummonDragon = 2311006
		};
	};

	namespace Bishop {
		enum {
			Bahamut = 2321003,
			BigBang = 2321001,
			HerosWill = 2321009,
			HolyShield = 2321005,
			Infinity = 2321004,
			ManaReflection = 2321002,
			MapleWarrior = 2321000,
			Resurrection = 2321006
		};
	};

	namespace Archer {
		enum {
			CriticalShot = 3000001,
			Focus = 3001003
		};
	};

	namespace Hunter {
		enum {
			ArrowBomb = 3101005,
			BowBooster = 3101002,
			BowMastery = 3100000,
			SoulArrow = 3101004
		};
	};

	namespace Ranger {
		enum {
			MortalBlow = 3110001,
			Puppet = 3111002,
			SilverHawk = 3111005
		};
	};

	namespace Bowmaster {
		enum {
			Concentrate = 3121008,
			Hamstring = 3121007,
			HerosWill = 3121009,
			Hurricane = 3121004,
			MapleWarrior = 3121000,
			Phoenix = 3121006,
			SharpEyes = 3121002
		};
	};

	namespace Crossbowman {
		enum {
			CrossbowBooster = 3201002,
			CrossbowMastery = 3200000,
			SoulArrow = 3201004
		};
	};

	namespace Sniper {
		enum {
			Blizzard = 3211003,
			GoldenEagle = 3211005,
			MortalBlow = 3210001,
			Puppet = 3211002
		};
	};

	namespace Marksman {
		enum {
			Blind = 3221006,
			Frostprey = 3221005,
			HerosWill = 3221008,
			MapleWarrior = 3221000,
			PiercingArrow = 3221001,
			SharpEyes = 3221002,
			Snipe = 3221007
		};
	};

	namespace Rogue {
		enum {
			DarkSight = 4001003,
			Disorder = 4001002
		};
	};

	namespace Assassin {
		enum {
			ClawBooster = 4101003,
			ClawMastery = 4100000,
			CriticalThrow = 4100001,
			Drain = 4101005,
			Haste = 4101004
		};
	};

	namespace Hermit {
		enum {
			Alchemist = 4110000,
			MesoUp = 4111001,
			ShadowMeso = 4111004,
			ShadowPartner = 4111002,
			ShadowWeb = 4111003
		};
	};

	namespace NightLord {
		enum {
			HerosWill = 4121009,
			MapleWarrior = 4121000,
			NinjaAmbush = 4121004,
			NinjaStorm = 4121008,
			ShadowClaw = 4121006,
			ShadowShifter = 4120002,
			Taunt = 4121003,
			VenomousStar = 4120005,
		};
	};

	namespace Bandit {
		enum {
			DaggerBooster = 4201002,
			DaggerMastery = 4200000,
			Haste = 4201003,
			Steal = 4201004
		};
	};

	namespace ChiefBandit {
		enum {
			Assaulter = 4211002,
			Chakra = 4211001,
			MesoExplosion = 4211006,
			MesoGuard = 4211005,
			Pickpocket = 4211003
		};
	};

	namespace Shadower {
		enum {
			BoomerangStep = 4221007,
			HerosWill = 4221008,
			MapleWarrior = 4221000,
			NinjaAmbush = 4221004,
			ShadowShifter = 4220002,
			Smokescreen = 4221006,
			Taunt = 4221003,
			VenomousStab = 4220005
		};
	};

	namespace Pirate {
		enum {
			Dash = 5001005
		};
	};

	namespace Infighter {
		enum {
			BackspinBlow = 5101002,
			CorkscrewBlow = 5101004,
			DoubleUppercut = 5101003,
			ImproveMaxHp = 5100000,
			KnucklerBooster = 5101006,
			KnucklerMastery = 5100001,
			MpRecovery = 5101005,
			OakBarrel = 5101007
		};
	};

	namespace Marauder {
		enum {
			EnergyCharge = 5110001,
			StunMastery = 5110000,
			Transformation = 5111005
		};
	};

	namespace Buccaneer {
		enum {
			Demolition = 5121004,
			MapleWarrior = 5121000,
			PiratesRage = 5121008, // Hero's Will
			Snatch = 5121005,
			SpeedInfusion = 5121009,
			SuperTransformation = 5121003,
			TimeLeap = 5121010
		};
	};

	namespace Gunslinger {
		enum {
			BlankShot = 5201004,
			Grenade = 5201002,
			GunBooster = 5201003,
			GunMastery = 5200000
		};
	};

	namespace Outlaw {
		enum {
			Flamethrower = 5211004,
			Gaviota = 5211002,
			HomingBeacon = 5211006,
			IceSplitter = 5211005,
			Octopus = 5211001
		};
	};

	namespace Corsair {
		enum {
			AerialStrike = 5221003,
			Battleship = 5221006,
			Bullseye = 5220011,
			ElementalBoost = 5220001,
			Hypnotize = 5221009,
			MapleWarrior = 5221000,
			RapidFire = 5221004,
			SpeedInfusion = 5221010 // Technically Hero's Will
		};
	};

	namespace GM {
		enum {
			Haste = 9001000,
			SuperDragonRoar = 9001001,
			Teleport = 9001002,
		};
	};

	namespace SuperGM {
		enum {
			HealPlusDispel = 9101000,
			Haste = 9101001,
			HolySymbol = 9101002,
			Bless = 9101003,
			Hide = 9101004,
			Resurrection = 9101005,
			SuperDragonRoar = 9101006,
			Teleport = 9101007,
			HyperBody = 9101008
		};
	};
};

namespace GMSuit {
	enum {
		Hat = 1002140,
		Top = 1042003,
		Bottom = 1062007,
		Weapon = 1322013
	};
};

#endif
