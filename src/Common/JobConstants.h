/*
Copyright (C) 2008-2012 Vana Development Team

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

#include "Types.h"

namespace Jobs {
	namespace JobIds {
		enum Jobs : int16_t {
			Beginner = 0,

			Swordsman = 100,
			Fighter = 110,
			Crusader = 111,
			Hero = 112,
			Page = 120,
			WhiteKnight = 121,
			Paladin = 122,
			Spearman = 130,
			DragonKnight = 131,
			DarkKnight = 132,

			Magician = 200,
			FpWizard = 210,
			FpMage = 211,
			FpArchMage = 212,
			IlWizard = 220,
			IlMage = 221,
			IlArchMage = 222,
			Cleric = 230,
			Priest = 231,
			Bishop = 232,

			Archer = 300,
			Hunter = 310,
			Ranger = 311,
			Bowmaster = 312,
			Crossbowman = 320,
			Sniper = 321,
			Marksman = 322,

			Rogue = 400,
			Assassin = 410,
			Hermit = 411,
			NightLord = 412,
			Bandit = 420,
			ChiefBandit = 421,
			Shadower = 422,
			DualBlade1 = 430,
			DualBlade2 = 431,
			DualBlade3 = 432,
			DualBlade4 = 433,
			DualBlade5 = 434,

			Pirate = 500,
			CannoneerPirate = 501,
			Brawler = 510,
			Marauder = 511,
			Buccaneer = 512,
			Gunslinger = 520,
			Outlaw = 521,
			Corsair = 522,
			Cannoneer = 530,
			CannonTrooper = 531,
			CannonMaster = 532,

			JuniorGm = 800, // ??
			Gm = 900,
			SuperGm = 910,

			Noblesse = 1000,

			DawnWarrior1 = 1100,
			DawnWarrior2 = 1110,
			DawnWarrior3 = 1111,
			DawnWarrior4 = 1112,

			BlazeWizard1 = 1200,
			BlazeWizard2 = 1210,
			BlazeWizard3 = 1211,
			BlazeWizard4 = 1212,

			WindArcher1 = 1300,
			WindArcher2 = 1310,
			WindArcher3 = 1311,
			WindArcher4 = 1312,

			NightWalker1 = 1400,
			NightWalker2 = 1410,
			NightWalker3 = 1411,
			NightWalker4 = 1412,

			ThunderBreaker1 = 1500,
			ThunderBreaker2 = 1510,
			ThunderBreaker3 = 1511,
			ThunderBreaker4 = 1512,

			Legend = 2000,
			Evan = 2001,
			Mercedes = 2002,

			Aran1 = 2100,
			Aran2 = 2110,
			Aran3 = 2111,
			Aran4 = 2112,

			Evan1 = 2200,
			Evan2 = 2210,
			Evan3 = 2211,
			Evan4 = 2212,
			Evan5 = 2213,
			Evan6 = 2214,
			Evan7 = 2215,
			Evan8 = 2216,
			Evan9 = 2217,
			Evan10 = 2218,

			Mercedes1 = 2300,
			Mercedes2 = 2310,
			Mercedes3 = 2311,
			Mercedes4 = 2312,

			Citizen = 3000,
			DemonSlayer = 3001,

			DemonSlayer1 = 3100,
			DemonSlayer2 = 3110,
			DemonSlayer3 = 3111,
			DemonSlayer4 = 3112,

			BattleMage1 = 3200,
			BattleMage2 = 3210,
			BattleMage3 = 3211,
			BattleMage4 = 3212,

			WildHunter1 = 3300,
			WildHunter2 = 3310,
			WildHunter3 = 3311,
			WildHunter4 = 3312,

			Mechanic1 = 3500,
			Mechanic2 = 3510,
			Mechanic3 = 3511,
			Mechanic4 = 3512
		};
	}
	namespace JobTypes {
		enum Types : int8_t {
			Adventurer = 0,
			Cygnus = 1,
			Legend = 2,
			Resistance = 3
		};
	}
	namespace JobLines {
		enum Lines : int8_t {
			Beginner = 0,
			Warrior = 1,
			Magician = 2,
			Bowman = 3,
			Thief = 4,
			Pirate = 5
		};
	}
	namespace JobTracks {
		enum Tracks : int8_t {
			Beginner = 0,
			Warrior = 1,
			Magician = 2,
			Bowman = 3,
			Thief = 4,
			Pirate = 5,

			JuniorGm = 8, // ??
			Gm = 9,

			Noblesse = 10,
			DawnWarrior = 11,
			BlazeWizard = 12,
			WindArcher = 13,
			NightWalker = 14,
			ThunderBreaker = 15,

			Legend = 20,
			Aran = 21,
			Evan = 22,
			Mercedes = 23,

			Citizen = 30,
			DemonSlayer = 31,
			BattleMage = 32,
			WildHunter = 33,
			Mechanic = 35
		};

		const int32_t JobTrackCount = 21;
		const int8_t JobTracks[JobTrackCount] = {
			Beginner, Warrior, Magician, Bowman, Thief, Pirate,
			Noblesse, DawnWarrior, BlazeWizard, WindArcher, NightWalker, ThunderBreaker,
			Legend, Aran, Evan, Mercedes,
			Citizen, DemonSlayer, BattleMage, WildHunter, Mechanic
		};
	}
	namespace JobProgressions {
		enum Progressions  : int8_t {
			Beginner = 0,
			FirstJob = 1,
			SecondJob = 2,
			ThirdJob = 3,
			FourthJob = 4,
			// The following only apply to a couple classes, e.g. Evan/Dual Blade
			FifthJob = 5,
			SixthJob = 6,
			SeventhJob = 7,
			EighthJob = 8,
			NinthJob = 9,
			TenthJob = 10
		};
	}
	namespace Beginners {
		const int32_t JobCount = 7;
		const int16_t Jobs[JobCount] = {
			Jobs::JobIds::Beginner,
			Jobs::JobIds::Noblesse,
			Jobs::JobIds::Legend, Jobs::JobIds::Evan, Jobs::JobIds::Mercedes,
			Jobs::JobIds::Citizen, Jobs::JobIds::DemonSlayer
		};
	}
}