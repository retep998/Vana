/*
Copyright (C) 2008-2016 Vana Development Team

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
#include "Common/GameLogicUtilities.hpp"
#include "Common/ItemDataProvider.hpp"
#include "Common/MapleVersion.hpp"
#include "Common/PacketReader.hpp"
#include "Common/TimeUtilities.hpp"
#include "Common/Timer.hpp"
#include "Common/TimerThread.hpp"
#include "ChannelServer/ChannelServer.hpp"
#include "ChannelServer/Player.hpp"
#include "ChannelServer/PlayerDataProvider.hpp"
#include "ChannelServer/Trade.hpp"
#include "ChannelServer/Trades.hpp"
#include "ChannelServer/TradesPacket.hpp"
#include <functional>
#include <string>

namespace Vana {
namespace ChannelServer {

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

auto TradeHandler::tradeHandler(ref_ptr_t<Player> player, PacketReader &reader) -> void {
	int8_t subOpcode = reader.get<int8_t>();
	switch (subOpcode) {
		case TradeOpcodes::OpenTrade: // Open trade - this usually comes with 03 00 - I think that implies the type of thing getting opened (trade, minigame, etc.)
			player->send(Packets::Trades::sendOpenTrade(player, nullptr));
			break;
		case TradeOpcodes::SendTradeRequest: {
			if (player->isTrading()) {
				// Most likely hacking
				return;
			}
			player_id_t recvId = reader.get<player_id_t>();
			auto receiver = ChannelServer::getInstance().getPlayerDataProvider().getPlayer(recvId);
			if (receiver != nullptr) {
				if (!receiver->isTrading()) {
					receiver->send(Packets::Trades::sendTradeRequest(player->getName(), ChannelServer::getInstance().getTrades().newTrade(player, receiver)));
				}
				else {
					player->send(Packets::Trades::sendTradeMessage(receiver->getName(), Packets::Trades::MessageTypes::DenyTrade, Packets::Trades::Messages::DoingSomethingElse));
				}
			}
			break;
		}
		case TradeOpcodes::DenyRequest: {
			trade_id_t tradeId = reader.get<trade_id_t>();
			ActiveTrade *trade = ChannelServer::getInstance().getTrades().getTrade(tradeId);
			if (trade != nullptr) {
				auto one = trade->getSender();
				auto two = trade->getReceiver();
				TradeHandler::removeTrade(tradeId);
				one->send(Packets::Trades::sendTradeMessage(two->getName(),Packets::Trades::MessageTypes::DenyTrade, reader.get<int8_t>()));
			}
			break;
		}
		case TradeOpcodes::AcceptRequest: {
			trade_id_t tradeId = reader.get<trade_id_t>();
			ActiveTrade *trade = ChannelServer::getInstance().getTrades().getTrade(tradeId);
			if (trade != nullptr) {
				auto one = trade->getSender();
				auto two = trade->getReceiver();
				two->setTrading(true);
				one->send(Packets::Trades::sendAddUser(two, TradeSlots::Two));
				player->send(Packets::Trades::sendOpenTrade(two, one));
				ChannelServer::getInstance().getTrades().stopTimeout(tradeId);
			}
			else {
				player->send(Packets::Trades::sendTradeEntryMessage(Packets::Trades::Messages::RoomAlreadyClosed));
			}
			break;
		}
		case TradeOpcodes::Chat: {
			ActiveTrade *trade = ChannelServer::getInstance().getTrades().getTrade(player->getTradeId());
			if (trade == nullptr) {
				// Hacking
				return;
			}
			auto one = trade->getSender();
			auto two = trade->getReceiver();
			bool blue = (player == two);
			string_t chat = player->getName() + " : " + reader.get<string_t>();
			one->send(Packets::Trades::sendTradeChat(blue, chat));
			if (two != nullptr) {
				two->send(Packets::Trades::sendTradeChat(blue, chat));
			}
			break;
		}
		case TradeOpcodes::CancelTrade:
			TradeHandler::cancelTrade(player);
			break;
		case TradeOpcodes::AddItem:
		case TradeOpcodes::AddMesos:
		case TradeOpcodes::AcceptTrade: {
			trade_id_t tradeId = player->getTradeId();
			ActiveTrade *trade = ChannelServer::getInstance().getTrades().getTrade(tradeId);
			if (trade == nullptr) {
				// Most likely hacking
				return;
			}
			TradeInfo *send = trade->getSenderTrade();
			TradeInfo *recv = trade->getReceiverTrade();
			auto one = trade->getSender();
			auto two = trade->getReceiver();
			bool isReceiver = (player == two);
			TradeInfo *mod = (isReceiver ? trade->getReceiverTrade() : trade->getSenderTrade());
			switch (subOpcode) {
				case TradeOpcodes::AddItem: {
					inventory_t inventory = reader.get<inventory_t>();
					inventory_slot_t slot = reader.get<inventory_slot_t>();
					slot_qty_t amount = reader.get<slot_qty_t>();
					trade_slot_t addSlot = reader.get<trade_slot_t>();
					Item *item = player->getInventory()->getItem(inventory, slot);
					if (item == nullptr || trade->isItemInSlot(mod, addSlot) || item->hasTradeBlock() || item->hasLock()) {
						// Hacking
						return;
					}

					auto itemInfo = ChannelServer::getInstance().getItemDataProvider().getItemInfo(item->getId());
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
					Item *use = trade->addItem(player, mod, item, addSlot, slot, inventory, amount);
					one->send(Packets::Trades::sendAddItem(isReceiver ? TradeSlots::Two : TradeSlots::One, addSlot, use));
					two->send(Packets::Trades::sendAddItem(isReceiver ? TradeSlots::One : TradeSlots::Two, addSlot, use));
					break;
				}
				case TradeOpcodes::AddMesos: {
					mesos_t amount = reader.get<mesos_t>();
					if (player->getInventory()->getMesos() < amount || amount < 0) {
						// Hacking
						return;
					}
					mesos_t mesos = trade->addMesos(player, mod, amount);
					one->send(Packets::Trades::sendAddMesos(isReceiver ? TradeSlots::Two : TradeSlots::One, mesos));
					two->send(Packets::Trades::sendAddMesos(isReceiver ? TradeSlots::One : TradeSlots::Two, mesos));
					break;
				}
				case TradeOpcodes::AcceptTrade: {
					trade->accept(mod);
					(isReceiver ? one : two)->send(Packets::Trades::sendAccepted());
					if (trade->bothAccepted()) {
						// Do trade processing
						bool fail = !trade->bothCanTrade();
						if (fail) {
							trade->returnTrade();
						}
						else {
							trade->swapTrade();
						}

						int8_t message = (fail ? Packets::Trades::Messages::Failure : Packets::Trades::Messages::Success);
						one->send(Packets::Trades::sendEndTrade(message));
						two->send(Packets::Trades::sendEndTrade(message));
						TradeHandler::removeTrade(tradeId);
					}
					break;
				}
			}
			break;
		}
	}
}

auto TradeHandler::cancelTrade(ref_ptr_t<Player> player) -> void {
	trade_id_t tradeId = player->getTradeId();
	ActiveTrade *trade = ChannelServer::getInstance().getTrades().getTrade(tradeId);
	if (trade != nullptr) {
		auto one = trade->getSender();
		auto two = trade->getReceiver();
		bool isReceiver = (player == two);
		if (isReceiver || (!isReceiver && two != nullptr && two->getTradeId() == one->getTradeId())) {
			// Left while in trade, give items back
			(isReceiver ? one : two)->send(Packets::Trades::sendLeaveTrade());
			trade->returnTrade();
		}
		TradeHandler::removeTrade(tradeId);
	}
}

auto TradeHandler::removeTrade(trade_id_t id) -> void {
	ChannelServer::getInstance().getTrades().removeTrade(id);
}

auto TradeHandler::getTaxLevel(mesos_t mesos) -> int32_t {
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

}
}