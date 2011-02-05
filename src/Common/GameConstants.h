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
	const int32_t DefaultCharacterSlots = 3;
}

namespace Gender {
	enum {
		Male = 0,
		Female = 1,
		Both = 2
	};
}

namespace GuildsAndAlliances {
	const int32_t RankQuantity = 5;
	const int32_t InvitationAliveTime = 300; // Amount of seconds the invitation is 'alive'
	const int32_t GuildMaxCapacacity = 100;

	// Alliance costs
	const int32_t AllianceCreationCost = 5000000;
	const int32_t AllianceCapacityIncreasementCost = 1000000;

	// Guild costs
	const int32_t GuildCreationCost = 1500000;
	const int32_t GuildDisbandCost = 200000;
	const int32_t GuildChangeEmblemCost = 5000000;
	const int32_t GuildRemoveEmblemCost = 1000000;
	const int32_t GuildCapacityIncreasementCost = 500000;

	namespace GuildIncreaseCapacityCostMultiplier {
		enum {
			FifteenSlots = 3,
			TwentySlots = 5,
			TwentyFiveOrMoreSlots = 7
		};
	}
}

namespace Stats {
	const uint8_t PlayerLevels = 200;
	const uint8_t CygnusLevels = 120;
	const uint8_t PetLevels = 30;
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

	const int16_t PetExp[PetLevels - 1] = {
		1, 3, 6, 14, 31, 60, 108, 181, 287, 434,
		632, 891, 1224, 1642, 2161, 2793, 3557, 4467, 5542, 6801,
		8263, 9950, 11882, 14084, 16578, 19391, 22548, 26074, 30000
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
	enum Constants {
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
	enum Modifiers {
		Normal,
		Immune,
		Strong,
		Weak
	};
}