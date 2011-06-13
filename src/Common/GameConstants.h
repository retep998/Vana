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
#pragma once

#include "Types.h"

namespace Parties {
	const int8_t MaxMembers = 6;
}

namespace Characters {
	const int8_t MaxNameSize = 12;
	const int8_t MinNameSize = 4;
	const int8_t MaxPasswordSize = 15;
	const int8_t MinPasswordSize = 5;
	const int32_t DefaultCharacterSlots = 3;
}

namespace Gender {
	enum : int8_t {
		Male = 0,
		Female = 1,
		Both = 2
	};
}

namespace GuildsAndAlliances {
	const int32_t RankQuantity = 5;
}

namespace Stats {
	const uint8_t PlayerLevels = 200;
	const uint8_t CygnusLevels = 120;
	const uint8_t PetLevels = 30;
	const uint8_t MountLevels = 30;
	const int16_t MaxMaxHp = 30000;
	const int16_t MinMaxHp = 1;
	const int16_t MaxMaxMp = 30000;
	const int16_t MinMaxMp = 1;
	const int16_t MaxFame = 30000;
	const int16_t MinFame = -30000;
	const int16_t MaxCloseness = 30000;
	const int16_t ApPerLevel = 5;
	const int16_t ApPerCygnusLevel = 6;
	const uint8_t CygnusApCutoff = 70;
	const int16_t SpPerLevel = 3;
	const int8_t MaxFullness = 100;
	const int8_t MinFullness = 0;
	const int8_t PetFeedFullness = 30;
	const int32_t MaxDamage = 199999;
	const int16_t EnergyChargeDecay = 200;
	const int16_t MaxEnergyChargeLevel = 10000;

	const int16_t PetExp[PetLevels - 1] = {
		1, 3, 6, 14, 31, 60, 108, 181, 287, 434,
		632, 891, 1224, 1642, 2161, 2793, 3557, 4467, 5542, 6801,
		8263, 9950, 11882, 14084, 16578, 19391, 22548, 26074, 30000
	};

	const int16_t MountExp[MountLevels - 1] = {
		6, 25, 50, 105, 134, 196, 254, 263, 315, 367,
		430, 543, 587, 679, 725, 897, 1146, 1394, 1701, 2247,
		2543, 2898, 3156, 3313, 3584, 3923, 4150, 4305, 4550
	};

	const uint32_t PlayerExp[PlayerLevels - 1] = {
		// 20 rows of 10 levels each (with 9 in the last row)
#if MAPLE_VERSION >= 70
		15, 34, 57, 92, 135, 372, 560, 840, 1242, 1144,
		1573, 2144, 2800, 3640, 4700, 5893, 7360, 9144, 11120, 13478,
		16268, 19320, 22881, 27009, 31478, 36601, 42446, 48722, 55816, 76560,
		86784, 98208, 110932, 124432, 139372, 155865, 173280, 192400, 213345, 235372,
		259392, 285532, 312928, 342624, 374760, 408336, 444544, 483532, 524160, 567772,
		598886, 631704, 666321, 702836, 741351, 781976, 824828, 870028, 917705, 967995,
		1021040, 1076993, 1136012, 1198265, 1263930, 1333193, 1406252, 1483314, 1564600, 1650340,
		1740778, 1836172, 1936794, 2042930, 2154882, 2272969, 2397528, 2528912, 2667496, 2813674,
		2967863, 3130501, 3302052, 3483004, 3673872, 3875200, 4087561, 4311559, 4547832, 4797052,
		5059931, 5337215, 5629694, 5938201, 6263614, 6606860, 6968915, 7350811, 7753635, 8178534,
		8626717, 9099461, 9598112, 10124088, 10678888, 11264090, 11881362, 12532460, 13219239, 13943652,
		14707764, 15513749, 16363902, 17260644, 18206527, 19204244, 20256636, 21366700, 22537594, 23772654,
		25075395, 26449526, 27898960, 29427822, 31040466, 32741483, 34535716, 36428272, 38424541, 40530206,
		42751261, 45094030, 47565183, 50171755, 52921167, 55821246, 58880250, 62106888, 65510344, 69100311,
		72887008, 76881216, 81094306, 85538273, 90225770, 95170142, 100385465, 105886588, 111689173, 117809740,
		124265713, 131075474, 138258409, 145834970, 153826726, 162256430, 171148082, 180526996, 190419876, 200854884,
		211861732, 223471754, 235718006, 248635352, 262260569, 276632448, 291791906, 307782102, 324648561, 342439302,
		361204976, 380999008, 401877753, 423900654, 447130409, 471633156, 497478652, 524740482, 553496260, 583827855,
		615821621, 649568646, 685165008, 722712050, 762316670, 804091623, 848155844, 894634784, 943660769, 995373379,
		1049919840, 1107455447, 1168144005, 1232158296, 1299680571, 1370903066, 1446028554, 1525270918, 1608855764
#else
		15, 34, 57, 92, 135, 372, 560, 840, 1242, 1716,
		2360, 3216, 4200, 5460, 7050, 8840, 11040, 13716, 16680, 20216,
		24402, 28980, 34320, 40512, 47216, 54900, 63666, 73080, 83720, 95700,
		108480, 122760, 138666, 155540, 174216, 194832, 216600, 240500, 266682, 294216,
		324240, 356916, 391160, 428280, 468450, 510420, 555680, 604416, 655200, 709716,
		748608, 789631, 832902, 878545, 926689, 977471, 1031036, 1087536, 1147132, 1209994,
		1276301, 1346242, 1420016, 1497832, 1579913, 1666492, 1757815, 1854143, 1955750, 2062925,
		2175973, 2295216, 2420993, 2553663, 2693603, 2841212, 2996910, 3161140, 3334370, 3517093,
		3709829, 3913127, 4127566, 4353756, 4592341, 4844001, 5109452, 5389449, 5684790, 5996316,
		6324914, 6671519, 7037118, 7422752, 7829518, 8258575, 8711144, 9188514, 9692044, 10223168,
		10783397, 11374327, 11997640, 12655110, 13348610, 14080113, 14851703, 15665576, 16524049, 17429566,
		18384706, 19392187, 20454878, 21575805, 22758159, 24005306, 25320796, 26708375, 28171993, 29715818,
		31344244, 33061908, 34873700, 36784778, 38800583, 40926854, 43169645, 45535341, 48030677, 50662758,
		53439077, 56367538, 59456479, 62714694, 66151459, 69776558, 73600313, 77633610, 81887931, 86375389,
		91108760, 96101520, 101367883, 106992842, 112782213, 118962678, 125481832, 132358236, 139611467, 147262175,
		155332142, 163844343, 172823012, 182293713, 192283408, 202820538, 213935103, 225658746, 238024845, 251068606,
		264827165, 279339639, 294647508, 310794191, 327825712, 345790561, 364739883, 384727628, 405810702, 428049128,
		451506220, 476248760, 502347192, 529875818, 558913012, 589541445, 621848316, 655925603, 691870326, 729784819,
		769777027, 811960808, 856456260, 903390063, 952895838, 1005114529, 1060194805, 1118293480, 1179575962, 1244216724,
		1312399800, 1384319309, 1460180007, 1540197871, 1624600714, 1713628833, 1807535693, 1906558648, 2011069705
#endif
	};

	namespace BaseHp {
		const int16_t Variation = 4; // This is the range of HP that the server will give

		const int16_t Beginner = 12; // These are base HP values rewarded on level up
		const int16_t Warrior = 24;
		const int16_t Magician = 10;
		const int16_t Bowman = 20;
		const int16_t Thief = 20;
		const int16_t Pirate = 22;
		const int16_t Gm = 150;

		const int16_t BeginnerAp = 8; // These are base HP values rewarded on AP distribution
		const int16_t WarriorAp = 20;
		const int16_t MagicianAp = 8;
		const int16_t BowmanAp = 16;
		const int16_t ThiefAp = 16;
		const int16_t PirateAp = 18;
		const int16_t GmAp = 16;
	}
	namespace BaseMp {
		const int16_t Variation = 2; // This is the range of MP that the server will give

		const int16_t Beginner = 10; // These are base MP values rewarded on level up
		const int16_t Warrior = 4;
		const int16_t Magician = 6;
		const int16_t Bowman = 14;
		const int16_t Thief = 14;
		const int16_t Pirate = 18;
		const int16_t Gm = 150;

		const int16_t BeginnerAp = 6; // These are base MP values rewarded on AP distribution
		const int16_t WarriorAp = 2;
		const int16_t MagicianAp = 18;
		const int16_t BowmanAp = 10;
		const int16_t ThiefAp = 10;
		const int16_t PirateAp = 14;
		const int16_t GmAp = 10;
	}
	enum Constants : int32_t {
		Skin = 0x01,
		Eyes = 0x02,
		Hair = 0x04,
		Pet = 0x08,
		Level = 0x10,
		Job = 0x20,
		Str = 0x40,
		Dex = 0x80,
		Int = 0x100,
		Luk = 0x200,
		Hp = 0x400,
		MaxHp = 0x800,
		Mp = 0x1000,
		MaxMp = 0x2000,
		Ap = 0x4000,
		Sp = 0x8000,
		Exp = 0x10000,
		Fame = 0x20000,
		Mesos = 0x40000
	};
}

namespace MonsterCards {
	const uint8_t MaxCardLevel = 5;
	const int32_t MaxPlayerLevel = 8;
	const int32_t PlayerLevels[MaxPlayerLevel - 1] = {10, 30, 60, 100, 150, 210, 280};
}

namespace MobElements {
	enum Modifiers : int8_t {
		Normal,
		Immune,
		Strong,
		Weak
	};
}