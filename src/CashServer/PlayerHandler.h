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
#include <string>
#include <vector>

using std::string;
using std::vector;

class CashItem;
class PacketReader;
class Player;
struct CashItemInfo;

namespace PlayerHandler {
	namespace OperationTypes {
		enum {
			RedeemCoupon = 0x00,
			BuyItem = 0x03,
			Gift = 0x04,
			WishList = 0x05,
			InventoryExpand = 0x06,
			StorageExpand = 0x07,
			BuyItemBestItems = 0x08, // Used by double-clicking the cashitem in the 'best items' list
			MoveItemToInventory = 0x0c,
			MoveItemToStorage = 0x0d,
			BuyPackage = 0x1d,
			GiftPackage = 0x1e,
			BuyItemMesos = 0x1f,
			BuyFriendRing = 0x22
		};
	}

	namespace ErrorMessages {
		enum {
			UnknownError = 0x00, // This is actually everything else...

			UnknownErrorAndGotoChannel1 = 0x93,
			RequestTimedOut = 0x94,
			UnknownErrorAndGotoChannel2 = 0x95,
			CantBuyAgeLimit = 0x97,
			ExceededAllottedLimitOfPrice = 0x98, 
			CheckCharacterName = 0x9a,
			ItemHasGenderRestrictionsConfirmRecipient = 0x9b,
			RecipientInventoryFull = 0x9c,
			CheckNumberOfItemsLimit = 0x9d,
			CheckCharacterNameOrGenderRestriction = 0x9e,
			CheckCouponNumber = 0x9f,
			CouponExpired = 0xa0,
			CouponAlreadyUsed = 0xa1,
			CouponOnlyForNexonCafes = 0xa2, // T_T
			CouponForNexonCafesAndExpired1 = 0xa3,
			CouponForNexonCafesAndExpired2 = 0xa4,
			IsNexonCashCouponNumberAndRegisterIt = 0xa5,
			CouponOnlyForRegularItems = 0xa7,
			CouponOnlyForMapleStoryAndCannotBeGifted = 0xa8, // Lolwut
			CheckInventoryFull = 0xa9,
			InvalidRecipient = 0xab,
			CheckRecipientName = 0xac,
			NotEnoughMesos = 0xb0,
			CashShopUnavailableDueBetaPhase = 0xb1,
			CheckYourBirthdayCode = 0xb2,
			StrangeCutoffString = 0xb5,
			AlreadyApplied = 0xb6,
			DailyPurchaseLimitExceeded = 0xbb,
			ExceededMaximumUsage = 0xbe,
			CouponSystemAvailableSoon = 0xc0,
			ItemOnlyUsable15DaysAfterRegging = 0xc1,
			NotEnoughGiftTokens = 0xc2, // LOL
			DueToTechnicalDifficultiesThisItemCannotBeSent = 0xc3,
			CannotGiveItemsWhenLessThen2WeeksAfterFirstCharge = 0xc4,
			UsersThatWhereBannedCantGift = 0xc5,
			DueToLimitationsYouMayNotGiftAtThisTime = 0xc6,
			ExceededAmountOfTimeToGiveItems = 0xc7,
			CannotGiveDueToTechnicalDifficulties = 0xc8,
			CannotTransferWorldUnder20 = 0xc9,
			CannotTransferWorldToSameWorld = 0xca,
			CannotTransferWorldToNewWorld = 0xcb,
			YouMayNotTransferOutOfThisWorldAtThisTime = 0xcc,
			CannotTransferWorldToWorldWithoutEmptySlots = 0xcd,
			EventEndedOrNotAvailableForTesting = 0xce
		};
	}

	namespace BuyModes {
		enum {
			NxCredit = 0x01,
			MaplePoints,
			NxPrepaid = 0x04
		};
	}

	namespace ChangeNameResults {
		enum {
			CanBeUsed = 0x01,
			InUse = 0x02
		};
	}

	void handleOperation(Player *player, PacketReader &packet);
	void handleBuyItem(Player *player, PacketReader &packet);
	void handleBuyPackage(Player *player, PacketReader &packet);
	void handleBuyItemMesos(Player *player, PacketReader &packet);
	void handleBuyFriendRing(Player *player, PacketReader &packet);
	void handleWishList(Player *player, PacketReader &packet);
	void handleGift(Player *player, PacketReader &packet);
	void handleMoveItemToInventory(Player *player, PacketReader &packet);
	void handleMoveItemToStorage(Player *player, PacketReader &packet);

	void handleChangeNameRequest(Player *player, PacketReader &packet);
	void handleChangeWorldRequest(Player *player, PacketReader &packet);

	void handleExpandInventory(Player *player, PacketReader &packet);
	void handleExpandStorage(Player *player, PacketReader &packet);
	void handleRedeemCoupon(Player *player, PacketReader &packet);
	void handleSendNote(Player *player, PacketReader &packet);

	bool buyCashItem(Player *player, CashItemInfo *info, int8_t mode, bool noCashNeeded = false, const string &giftee = "", const string &giftMessage = "");
	bool buyCashItemPackage(Player *player, CashItemInfo *info, int8_t mode, vector<CashItem *> &itemList, const string &giftee = "", const string &giftMessage = "");
	bool takeCash(Player *player, int8_t buyMode, int32_t amount);
};
