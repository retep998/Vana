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
#include <limits>

namespace Inventories {
	const uint8_t InventoryCount = 5;
	const uint8_t EquipInventory = 1;
	const uint8_t UseInventory = 2;
	const uint8_t SetupInventory = 3;
	const uint8_t EtcInventory = 4;
	const uint8_t CashInventory = 5;

	const uint8_t EquippedSlots = 51;
	const int8_t MaxPetCount = 3;
	const uint8_t VipRockMax = 10;
	const uint8_t TeleportRockMax = 5;
}

namespace EquipSlots {
	enum {
		Helm = 1,
		Face = 2,
		Eye = 3,
		Earring = 4,
		Top = 5,
		Bottom = 6,
		Shoe = 7,
		Glove = 8,
		Cape = 9,
		Shield = 10,
		Weapon = 11,
		Ring1 = 12,
		Ring2 = 13,
		PetEquip1 = 14,
		Ring3 = 15,
		Ring4 = 16,
		Pendant = 17,
		Mount = 18,
		Saddle = 19,
		PetCollar = 20,
		PetLabelRing1 = 21,
		PetItemPouch1 = 22,
		PetMesoMagnet1 = 23,
		PetAutoHp = 24,
		PetAutoMp = 25,
		PetWingBoots1 = 26,
		PetBinoculars1 = 27,
		PetMagicScales1 = 28,
		PetQuoteRing1 = 29,
		PetEquip2 = 30,
		PetLabelRing2 = 31,
		PetQuoteRing2 = 32,
		PetItemPouch2 = 33,
		PetMesoMagnet2 = 34,
		PetWingBoots2 = 35,
		PetBinoculars2 = 36,
		PetMagicScales2 = 37,
		PetEquip3 = 38,
		PetLabelRing3 = 39,
		PetQuoteRing3 = 40,
		PetItemPouch3 = 41,
		PetMesoMagnet3 = 42,
		PetWingBoots3 = 43,
		PetBinoculars3 = 44,
		PetMagicScales3 = 45,
		PetItemIgnore1 = 46,
		PetItemIgnore2 = 47,
		PetItemIgnore3 = 48,
		Medal = 49,
		Belt = 50
	};
}

namespace Items {
	const int64_t NoExpiration = 150842304000000000LL;
	const int32_t MaxHammers = 2;
	const int32_t WeatherTime = 30;

	namespace Types {
		enum ItemTypes {
			ArmorHelm = 100,
			ArmorFace = 101,
			ArmorEye = 102,
			ArmorEarring = 103,
			ArmorTop = 104,
			ArmorOverall = 105,
			ArmorBottom = 106,
			ArmorShoe = 107,
			ArmorGlove = 108,
			ArmorShield = 109,
			ArmorCape = 110,
			ArmorRing = 111,
			ArmorPendant = 112,
			Medal = 114,
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
			WeaponGun = 149,
			Mount = 190,
			ItemArrow = 206,
			ItemStar = 207,
			ItemBullet = 233,
			ItemMonsterCard = 238,
			WeatherCash = 512,
			CashPetFood = 524
		};
	}

	namespace ScrollTypes {
		enum ScrollTypes {
			Helm = 0,
			Face = 100,
			Eye = 200,
			Earring = 300,
			Topwear = 400,
			Overall = 500,
			Bottomwear = 600,
			Shoes = 700,
			Gloves = 800,
			Shield = 900,
			Cape = 1000,
			Ring = 1100,
			Pendant = 1200,
			OneHandedSword = 3000,
			OneHandedAxe = 3100,
			OneHandedMace = 3200,
			Dagger = 3300,
			Wand = 3700,
			Staff = 3800,
			TwoHandedSword = 4000,
			TwoHandedAxe = 4100,
			TwoHandedMace = 4200,
			Spear = 4300,
			Polearm = 4400,
			Bow = 4500,
			Crossbow = 4600,
			Claw = 4700,
			Knuckle = 4800,
			Gun = 4900,
			PetEquip = 8000,
			CleanSlate = 9000,
			Chaos = 9100
		};
	}

	namespace Flags {
		enum ItemFlags {
			Lock = 0x01,
			Spikes = 0x02,
			ColdProtection = 0x04,
			TradeUnavailable = 0x08,
			KarmaScissors = 0x10
		};
	}

	namespace StatVariance {
		const uint16_t Str = 2;
		const uint16_t Dex = 2;
		const uint16_t Int = 2;
		const uint16_t Luk = 2;
		const uint16_t Hp = 10;
		const uint16_t Mp = 10;
		const uint16_t Watk = 10;
		const uint16_t Wdef = 10;
		const uint16_t Matk = 10;
		const uint16_t Mdef = 10;
		const uint16_t Acc = 2;
		const uint16_t Avoid = 2;
		const uint16_t Hands = 4;
		const uint16_t Speed = 4;
		const uint16_t Jump = 2;
		const uint16_t RandScroll = 5;
	}

	namespace MaxStats {
		const int16_t Str = std::numeric_limits<int16_t>::max();
		const int16_t Dex = std::numeric_limits<int16_t>::max();
		const int16_t Int = std::numeric_limits<int16_t>::max();
		const int16_t Luk = std::numeric_limits<int16_t>::max();
		const int16_t Hp = std::numeric_limits<int16_t>::max();
		const int16_t Mp = std::numeric_limits<int16_t>::max();
		const int16_t Watk = 1999;
		const int16_t Wdef = 255;
		const int16_t Matk = 1999;
		const int16_t Mdef = 255;
		const int16_t Acc = 103;
		const int16_t Avoid = 103;
		const int16_t Hands = std::numeric_limits<int16_t>::max();
		const int16_t Speed = 40;
		const int16_t Jump = 23;
	}

	enum {
		// Equip
		GmHat = 1002140,
		GmTop = 1042003,
		GmBottom = 1062007,
		GmWeapon = 1322013,
		BattleshipMount = 1932000,

		// Use
		ShoeSpikes = 2040727,
		CapeColdProtection = 2041058,
		SpecialTeleportRock = 2320000,
		WhiteScroll = 2340000,

		// Etc.
		LudibriumMedal = 4031172,
		BeginnersShoppingGuide = 4031180,
		GoldenBell = 4031191,
		RedRibbon = 4031192,
		BeginnersGuidebook = 4161001,

		// Cash
		TeleportRock = 5040000,
		TeleportCoke = 5040001,
		VipRock = 5041000,
		ApReset = 5050000,
		FirstJobSpReset = 5050001,
		SecondJobSpReset = 5050002,
		ThirdJobSpReset = 5050003,
		FourthJobSpReset = 5050004,
		ItemNameTag = 5060000,
		ItemLock = 5060001,
		Megaphone = 5071000,
		SuperMegaphone = 5072000,
		ItemMegaphone = 5076000,
		ArtMegaphone = 5077000,
		MapleTvMessenger = 5075000,
		MapleTvStarMessenger = 5075001,
		MapleTvHeartMessenger = 5075002,
		Megassenger = 5075003,
		StarMegassenger = 5075004,
		HeartMegassenger = 5075005,
		KoreanKite = 5080000,
		HeartBalloon = 5080001,
		GraduationBanner = 5080002,
		AdmissionBanner = 5080003,
		CongratulatorySong = 5100000,
		SnowySnow = 5120000,
		SafetyCharm = 5130000,
		PetNameTag = 5170000,
		BronzeSackOfMesos = 5200000,
		SilverSackOfMesos = 5200001,
		GoldSackOfMesos = 5200002,
		FungusScroll = 5300000,
		OinkerDelight = 5300001,
		ZetaNightmare = 5300002,
		Chalkboard = 5370000,
		Chalkboard2 = 5370001,
		DiabloMessenger = 5390000,
		Cloud9Messenger = 5390001,
		LoveholicMessenger = 5390002,
		WheelOfDestiny = 5510000,
		ScissorsOfKarma = 5520000,
		ViciousHammer = 5570000
	};
}
namespace CashItems {
	namespace Serials {
		enum {
			AddEquipSlots = 50200093,
			AddUseSlots = 50200094,
			AddEtcSlots = 50200095,
			GoldenBell = 80000001,
			RedRibbon = 80000002,
			BeginnersShopingGuide = 80000048
		};
	}

	namespace Flags {
		enum {
			HasDiscount = 0x4,
			RemoveAddItem = 0x400, // It has a byte/bool after it: don't show/show
			ReplaceMark = 0x800
		};
	}
}