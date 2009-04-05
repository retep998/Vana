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
	Weapon_1h_Sword = 130,
	Weapon_1h_Axe = 131,
	Weapon_1h_Mace = 132,
	Weapon_Dagger = 133,
	Weapon_Wand = 137,
	Weapon_Staff = 138,
	Weapon_2h_Sword = 140,
	Weapon_2h_Axe = 141,
	Weapon_2h_Mace = 142,
	Weapon_Spear = 143,
	Weapon_Polearm = 144,
	Weapon_Bow = 145,
	Weapon_Crossbow = 146,
	Weapon_Claw = 147,
	Weapon_Knuckle = 148,
	Weapon_Gun = 149
};

enum ItemFlags {
	Flag_Lock = 0x01,
	Flag_Spikes = 0x02,
	Flag_Cold = 0x04
};

enum Act {
	Act_Heal,
	Act_Hurt
};

enum SkillValues {
	Skill_X,
	Skill_Y,
	Skill_Speed,
	Skill_Jump,
	Skill_Watk,
	Skill_Wdef,
	Skill_Matk,
	Skill_Mdef,
	Skill_Acc,
	Skill_Avo,
	Skill_Prop,
	Skill_Morph,
	Skill_Lv
};

enum ByteTypes {
	Type_5,
	Type_6,
	Type_7,
	Type_8,
	Type_1,
	Type_2,
	Type_3,
	Type_4
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
	Weapon_Attack_Up = 0x1000,
	Weapon_Defense_Up = 0x2000,
	Magic_Attack_Up = 0x4000,
	Magic_Defense_Up = 0x8000,
	Doom = 0x10000,
	Shadow_Web = 0x20000,
	Weapon_Immunity = 0x40000,
	Magic_Immunity = 0x80000
};

// Skills so there are fewer magic numbers
namespace Jobs {
	namespace Beginner {
		enum {
			Echo_Of_Hero = 1005,
			Follow_The_Lead = 8,
			Monster_Rider = 1004,
			Nimble_Feet = 1002,
			Recovery = 1001
		};
	};

	namespace Swordsman {
		enum {
			Improved_Max_Hp_Increase = 1000001,
			Iron_Body = 1000003,
		};
	};

	namespace Fighter {
		enum {
			Axe_Booster = 1101005,
			Axe_Mastery = 1100001,
			Power_Guard = 1101007,
			Rage = 1101006,
			Sword_Booster = 1101004,
			Sword_Mastery = 1100000
		};
	};

	namespace Crusader {
		enum {
			Armor_Crash = 1111007,
			Axe_Coma = 1111006,
			Axe_Panic = 1111004,
			Combo_Attack = 1111002,
			Shout = 1111008,
			Sword_Coma = 1111005,
			Sword_Panic = 1111003
		};
	};

	namespace Hero {
		enum {
			Achilles = 1120004,
			Advanced_Combo_Attack = 1120003,
			Enrage = 1121010,
			Guardian = 1120005,
			Heros_Will = 1121011,
			Maple_Warrior = 1121000,
			Monster_Magnet = 1121001,
			Power_Stance = 1121002
		};
	};

	namespace Page {
		enum {
			Bw_Booster = 1201005,
			Bw_Mastery = 1200001,
			Power_Guard = 1201007,
			Sword_Booster = 1201004,
			Sword_Mastery = 1200000,
			Threaten = 1201006
		};
	};

	namespace WhiteKnight {
		enum {
			Bw_Fire_Charge = 1211004,
			Bw_Ice_Charge = 1211006,
			Bw_Lit_Charge = 1211008,
			Charge_Blow = 1211002,
			Magic_Crash = 1211009,
			Sword_Fire_Charge = 1211003,
			Sword_Ice_Charge = 1211005,
			Sword_Lit_Charge = 1211007
		};
	};

	namespace Paladin {
		enum {
			Achilles = 1220005,
			Advanced_Charge = 1220010,
			Bw_Holy_Charge = 1221004,
			Guardian = 1220006,
			Heavens_Hammer = 1221011,
			Heros_Will = 1221012,
			Maple_Warrior = 1221000,
			Monster_Magnet = 1221001,
			Power_Stance = 1221002,
			Sword_Holy_Charge = 1221003
		};
	};

	namespace Spearman {
		enum {
			Hyper_Body = 1301007,
			Iron_Will = 1301006,
			Polearm_Booster = 1301005,
			Polearm_Mastery = 1300001,
			Spear_Booster = 1301004,
			Spear_Mastery = 1300000
		};
	};

	namespace DragonKnight {
		enum {
			Dragon_Blood = 1311008,
			Dragon_Roar = 1311006,
			Power_Crash = 1311007,
			Sacrifice = 1311005
		};
	};

	namespace DarkKnight {
		enum {
			Achilles = 1320005,
			Aura_Of_Beholder = 1320008,
			Beholder = 1321007,
			Berserk = 1320006,
			Heros_Will = 1321010,
			Hex_Of_Beholder = 1320009,
			Maple_Warrior = 1321000,
			Monster_Magnet = 1321001,
			Power_Stance = 1321002
		};
	};

	namespace Magician {
		enum {
			Improved_Max_Mp_Increase = 2000001,
			Magic_Armor = 2001003,
			Magic_Guard = 2001002
		};
	};

	namespace FPWizard {
		enum {
			Meditation = 2101001,
			Mp_Eater = 2100000,
			Poison_Breath = 2101005,
			Slow = 2101003
		};
	};

	namespace FPMage {
		enum {
			Element_Amplification = 2110001,
			Element_Composition = 2111006,
			Poison_Mist = 2111003,
			Seal = 2111004,
			Spell_Booster = 2111005
		};
	};

	namespace FPArchMage {
		enum {
			Big_Bang = 2121001,
			Elquines = 2121005,
			Fire_Demon = 2121003,
			Heros_Will = 2121008,
			Infinity = 2121004,
			Mana_Reflection = 2121002,
			Maple_Warrior = 2121000,
			Paralyze = 2121006
		};
	};

	namespace ILWizard {
		enum {
			Cold_Beam = 2201004,
			Meditation = 2201001,
			Mp_Eater = 2200000,
			Slow = 2201003
		};
	};

	namespace ILMage {
		enum {
			Element_Amplification = 2210001,
			Element_Composition = 2211006,
			Ice_Strike = 2211002,
			Seal = 2211004,
			Spell_Booster = 2211005
		};
	};

	namespace ILArchMage {
		enum {
			Big_Bang = 22210001,
			Blizzard = 2221007,
			Heros_Will = 2221008,
			Ice_Demon = 2221003,
			Ifrit = 2221005,
			Infinity = 2221004,
			Mana_Reflection = 2221002,
			Maple_Warrior = 2221000
		};
	};

	namespace Cleric {
		enum {
			Bless = 2301004,
			Heal = 2301002,
			Invincible = 2301003,
			Mp_Eater = 2300000
		};
	};

	namespace Priest {
		enum {
			Dispel = 2311001,
			Doom = 2311005,
			Holy_Symbol = 2311003,
			Mystic_Door = 2311002,
			Summon_Dragon = 2311006
		};
	};

	namespace Bishop {
		enum {
			Bahamut = 2321003,
			Big_Bang = 2321001,
			Heros_Will = 2321009,
			Holy_Shield = 2321005,
			Infinity = 2321004,
			Mana_Reflection = 2321002,
			Maple_Warrior = 2321000,
			Resurrection = 2321006
		};
	};

	namespace Archer {
		enum {
			Critical_Shot = 3000001,
			Focus = 3001003
		};
	};

	namespace Hunter {
		enum {
			Arrow_Bomb = 3101005,
			Bow_Booster = 3101002,
			Bow_Mastery = 3100000,
			Soul_Arrow = 3101004
		};
	};

	namespace Ranger {
		enum {
			Mortal_Blow = 3110001,
			Puppet = 3111002,
			Silver_Hawk = 3111005
		};
	};

	namespace Bowmaster {
		enum {
			Concentrate = 3121008,
			Hamstring = 3121007,
			Heros_Will = 3121009,
			Hurricane = 3121004,
			Maple_Warrior = 3121000,
			Phoenix = 3121006,
			Sharp_Eyes = 3121002
		};
	};

	namespace Crossbowman {
		enum {
			Crossbow_Booster = 3201002,
			Crossbow_Mastery = 3200000,
			Soul_Arrow = 3201004
		};
	};

	namespace Sniper {
		enum {
			Blizzard = 3211003,
			Golden_Eagle = 3211005,
			Mortal_Blow = 3210001,
			Puppet = 3211002
		};
	};

	namespace Marksman {
		enum {
			Blind = 3221006,
			Frostprey = 3221005,
			Heros_Will = 3221008,
			Maple_Warrior = 3221000,
			Piercing_Arrow = 3221001,
			Sharp_Eyes = 3221002,
			Snipe = 3221007
		};
	};

	namespace Rogue {
		enum {
			Dark_Sight = 4001003,
			Disorder = 4001002
		};
	};

	namespace Assassin {
		enum {
			Claw_Booster = 4101003,
			Claw_Mastery = 4100000,
			Critical_Throw = 4100001,
			Drain = 4101005,
			Haste = 4101004
		};
	};

	namespace Hermit {
		enum {
			Alchemist = 4110000,
			Meso_Up = 4111001,
			Shadow_Meso = 4111004,
			Shadow_Partner = 4111002,
			Shadow_Web = 4111003
		};
	};

	namespace NightLord {
		enum {
			Heros_Will = 4121009,
			Maple_Warrior = 4121000,
			Ninja_Ambush = 4121004,
			Ninja_Storm = 4121008,
			Shadow_Claw = 4121006,
			Shadow_Shifter = 4120002,
			Taunt = 4121003,
			Venomous_Star = 4120005,
		};
	};

	namespace Bandit {
		enum {
			Dagger_Booster = 4201002,
			Dagger_Mastery = 4200000,
			Haste = 4201003,
			Steal = 4201004
		};
	};

	namespace ChiefBandit {
		enum {
			Assaulter = 4211002,
			Chakra = 4211001,
			Meso_Explosion = 4211006,
			Meso_Guard = 4211005,
			Pickpocket = 4211003
		};
	};

	namespace Shadower {
		enum {
			Boomerang_Step = 4221007,
			Heros_Will = 4221008,
			Maple_Warrior = 4221000,
			Ninja_Ambush = 4221004,
			Shadow_Shifter = 4220002,
			Smokescreen = 4221006,
			Taunt = 4221003,
			Venomous_Stab = 4220005
		};
	};

	namespace Pirate {
		enum {
			Dash = 5001005
		};
	};

	namespace Infighter {
		enum {
			Backspin_Blow = 5101002,
			Corkscrew_Blow = 5101004,
			Double_Uppercut = 5101003,
			Improve_Max_Hp = 5100000,
			Knuckler_Booster = 5101006,
			Knuckler_Mastery = 5100001,
			Mp_Recovery = 5101005,
			Oak_Barrel = 5101007
		};
	};

	namespace Marauder {
		enum {
			Energy_Charge = 5110001,
			Stun_Mastery = 5110000,
			Transformation = 5111005
		};
	};

	namespace Buccaneer {
		enum {
			Demolition = 5121004,
			Maple_Warrior = 5121000,
			Pirates_Rage = 5121008, // Hero's Will
			Snatch = 5121005,
			Speed_Infusion = 5121009,
			Super_Transformation = 5121003,
			Time_Leap = 5121010
		};
	};

	namespace Gunslinger {
		enum {
			Blank_Shot = 5201004,
			Grenade = 5201002,
			Gun_Booster = 5201003,
			Gun_Mastery = 5200000
		};
	};

	namespace Outlaw {
		enum {
			Flamethrower = 5211004,
			Gaviota = 5211002,
			Homing_Beacon = 5211006,
			Ice_Splitter = 5211005,
			Octopus = 5211001
		};
	};

	namespace Corsair {
		enum {
			Aerial_Strike = 5221003,
			Battleship = 5221006,
			Bullseye = 5220011,
			Elemental_Boost = 5220001,
			Hypnotize = 5221009,
			Maple_Warrior = 5221000,
			Rapid_Fire = 5221004,
			Speed_Infusion = 5221010 // Technically Hero's Will
		};
	};

	namespace GM {
		enum {
			Haste = 9001000,
			Super_Dragon_Roar = 9001001,
			Teleport = 9001002,
		};
	};

	namespace SuperGM {
		enum {
			Heal_Plus_Dispel = 9101000,
			Haste = 9101001,
			Holy_Symbol = 9101002,
			Bless = 9101003,
			Hide = 9101004,
			Resurrection = 9101005,
			Super_Dragon_Roar = 9101006,
			Teleport = 9101007,
			Hyper_Body = 9101008
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