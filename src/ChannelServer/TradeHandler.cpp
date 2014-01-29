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
#include "TradeHandler.hpp"
#include "GameLogicUtilities.hpp"
#include "ItemDataProvider.hpp"
#include "MapleVersion.hpp"
#include "Player.hpp"
#include "PlayerDataProvider.hpp"
#include "PacketReader.hpp"
#include "TimeUtilities.hpp"
#include "Timer.hpp"
#include "TimerThread.hpp"
#include "Trade.hpp"
#include "Trades.hpp"
#include "TradesPacket.hpp"
#include <functional>
#include <string>

namespace TradeOpcodes {
	enum Opcodes : int8_t {
		OpenTrade = 0x00,
		SendTradeRequest = 0x02,
		DenyRequest = 0x03,
		AcceptRequest = 0x04,
		Chat = 0x06,
		CancelTrade = 0x0A,
		AddItem = 0x0E,
		AddMesos = 0x0F,
		AcceptTrade = 0x10
	};
}

namespace TradeSlots {
	enum Slots : int8_t {
		One = 0x00,
		Two = 0x01
	};
}

auto TradeHandler::tradeHandler(Player *player, PacketReader &packet) -> void {
	int8_t subOpcode = packet.get<int8_t>();
	switch (subOpcode) {
		case TradeOpcodes::OpenTrade: // Open trade - this usually comes with 03 00 - I think that implies the type of thing getting opened (trade, minigame, etc.)
			TradesPacket::sendOpenTrade(player, player, nullptr);
			break;
		case TradeOpcodes::SendTradeRequest: {
			if (player->isTrading()) {
				// Most likely hacking
				return;
			}
			int32_t recvId = packet.get<int32_t>();
			Player *receiver = PlayerDataProvider::getInstance().getPlayer(recvId);
			if (receiver != nullptr) {
				if (!receiver->isTrading()) {
					TradesPacket::sendTradeRequest(player, receiver, Trades::getInstance().newTrade(player, receiver));
				}
				else {
					TradesPacket::sendTradeMessage(receiver, player, TradesPacket::MessageTypes::DenyTrade, TradesPacket::Messages::DoingSomethingElse);
				}
			}
			break;
		}
		case TradeOpcodes::DenyRequest: {
			int32_t tradeId = packet.get<int32_t>();
			ActiveTrade *trade = Trades::getInstance().getTrade(tradeId);
			if (trade != nullptr) {
				Player *one = trade->getSender();
				Player *two = trade->getReceiver();
				TradeHandler::removeTrade(tradeId);
				TradesPacket::sendTradeMessage(two, one, TradesPacket::MessageTypes::DenyTrade, packet.get<int8_t>());
			}
			break;
		}
		case TradeOpcodes::AcceptRequest: {
			int32_t tradeId = packet.get<int32_t>();
			ActiveTrade *trade = Trades::getInstance().getTrade(tradeId);
			if (trade != nullptr) {
				Player *one = trade->getSender();
				Player *two = trade->getReceiver();
				two->setTrading(true);
				TradesPacket::sendAddUser(one, two, TradeSlots::Two);
				TradesPacket::sendOpenTrade(player, two, one);
				Trades::getInstance().stopTimeout(tradeId);
			}
			else {
				TradesPacket::sendTradeMessage(player, TradesPacket::MessageTypes::ShopEntryMessages, TradesPacket::Messages::RoomAlreadyClosed);
			}
			break;
		}
		case TradeOpcodes::Chat: {
			ActiveTrade *trade = Trades::getInstance().getTrade(player->getTradeId());
			if (trade == nullptr) {
				// Hacking
				return;
			}
			Player *one = trade->getSender();
			Player *two = trade->getReceiver();
			bool blue = (player == two);
			string_t chat = player->getName() + " : " + packet.getString();
			TradesPacket::sendTradeChat(one, blue, chat);
			if (two != nullptr) {
				TradesPacket::sendTradeChat(two, blue, chat);
			}
			break;
		}
		case TradeOpcodes::CancelTrade:
			TradeHandler::cancelTrade(player);
			break;
		case TradeOpcodes::AddItem:
		case TradeOpcodes::AddMesos:
		case TradeOpcodes::AcceptTrade: {
			int32_t tradeId = player->getTradeId();
			ActiveTrade *trade = Trades::getInstance().getTrade(tradeId);
			if (trade == nullptr) {
				// Most likely hacking
				return;
			}
			TradeInfo *send = trade->getSenderTrade();
			TradeInfo *recv = trade->getReceiverTrade();
			Player *one = trade->getSender();
			Player *two = trade->getReceiver();
			bool isReceiver = (player == two);
			TradeInfo *mod = (isReceiver ? trade->getReceiverTrade() : trade->getSenderTrade());
			switch (subOpcode) {
				case TradeOpcodes::AddItem: {
					int8_t inventory = packet.get<int8_t>();
					int16_t slot = packet.get<int16_t>();
					int16_t amount = packet.get<int16_t>();
					uint8_t addslot = packet.get<uint8_t>();
					Item *item = player->getInventory()->getItem(inventory, slot);
					if (item == nullptr || trade->isItemInSlot(mod, addslot) || item->hasTradeBlock() || item->hasLock()) {
						// Hacking
						return;
					}

					auto itemInfo = ItemDataProvider::getInstance().getItemInfo(item->getId());
					if ((itemInfo->quest || itemInfo->noTrade) && !(itemInfo->karmaScissors || item->hasKarma())) {
						// Hacking
						return;
					}
					if (GameLogicUtilities::isGmEquip(item->getId())) {
						// We don't allow these to be dropped or traded
						return;
					}

					if (GameLogicUtilities::isRechargeable(item->getId())) {
						amount = item->getAmount();
					}
					else if (amount > item->getAmount() || amount < 0) {
						// Hacking
						amount = item->getAmount();
					}
					Item *use = trade->addItem(player, mod, item, addslot, slot, inventory, amount);
					TradesPacket::sendAddItem(one, (isReceiver ? TradeSlots::Two : TradeSlots::One), addslot, use);
					TradesPacket::sendAddItem(two, (isReceiver ? TradeSlots::One : TradeSlots::Two), addslot, use);
					break;
				}
				case TradeOpcodes::AddMesos: {
					int32_t amount = packet.get<int32_t>();
					if (player->getInventory()->getMesos() < amount || amount < 0) {
						// Hacking
						return;
					}
					int32_t mesos = trade->addMesos(player, mod, amount);
					TradesPacket::sendAddMesos(one, (isReceiver ? TradeSlots::Two : TradeSlots::One), mesos);
					TradesPacket::sendAddMesos(two, (isReceiver ? TradeSlots::One : TradeSlots::Two), mesos);
					break;
				}
				case TradeOpcodes::AcceptTrade: {
					trade->accept(mod);
					TradesPacket::sendAccepted(isReceiver ? one : two);
					if (trade->bothAccepted()) {
						// Do trade processing
						bool fail = !trade->bothCanTrade();
						if (fail) {
							trade->returnTrade();
						}
						else {
							trade->swapTrade();
						}
						int8_t type = TradesPacket::MessageTypes::EndTrade;
						int8_t message = (fail ? TradesPacket::Messages::Failure : TradesPacket::Messages::Success);
						TradesPacket::sendTradeMessage(one, type, message);
						TradesPacket::sendTradeMessage(two, type, message);
						TradeHandler::removeTrade(tradeId);
					}
					break;
				}
			}
			break;
		}
	}
}

auto TradeHandler::cancelTrade(Player *player) -> void {
	int32_t tradeId = player->getTradeId();
	ActiveTrade *trade = Trades::getInstance().getTrade(tradeId);
	if (trade != nullptr) {
		Player *one = trade->getSender();
		Player *two = trade->getReceiver();
		bool isReceiver = (player == two);
		if (isReceiver || (!isReceiver && two != nullptr && two->getTradeId() == one->getTradeId())) {
			// Left while in trade, give items back
			TradesPacket::sendLeaveTrade(isReceiver ? one : two);
			trade->returnTrade();
		}
		TradeHandler::removeTrade(tradeId);
	}
}

auto TradeHandler::removeTrade(int32_t id) -> void {
	Trades::getInstance().removeTrade(id);
}

auto TradeHandler::getTaxLevel(int32_t mesos) -> int32_t {
#if MAPLE_LOCALE == MAPLE_LOCALE_GLOBAL
#	if MAPLE_VERSION >= 67
	if (mesos < 100000) {
		return 0;
	}
	if (mesos >= 100000000) {
		return 600;
	}
	if (mesos >= 25000000) {
		return 500;
	}
	if (mesos >= 10000000) {
		return 400;
	}
	if (mesos >= 5000000) {
		return 300;
	}
	if (mesos >= 1000000) {
		return 180;
	}
	return 80;
#	elif MAPLE_VERSION >= 17
	if (mesos < 50000) {
		return 0;
	}
	if (mesos >= 10000000) {
		return 400;
	}
	if (mesos >= 5000000) {
		return 300;
	}
	if (mesos >= 1000000) {
		return 200;
	}
	if (mesos >= 100000) {
		return 100;
	}
	return 50;
#	endif /* MAPLE_VERSION check for tax levels */
#else
#	error Please specify trade costs
#endif
	return 0; // There was no tax prior to .17
}