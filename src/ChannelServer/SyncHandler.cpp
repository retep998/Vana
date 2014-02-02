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
#include "SyncHandler.hpp"
#include "BuddyListPacket.hpp"
#include "ChannelServer.hpp"
#include "Configuration.hpp"
#include "Connectable.hpp"
#include "GameObjects.hpp"
#include "InterHeader.hpp"
#include "InterHelper.hpp"
#include "PacketReader.hpp"
#include "Party.hpp"
#include "Player.hpp"
#include "PlayerBuddyList.hpp"
#include "PlayerDataProvider.hpp"
#include "PlayerPacket.hpp"
#include "SmsgHeader.hpp"
#include "SyncPacket.hpp"

auto SyncHandler::handle(PacketReader &reader) -> void {
	switch (reader.get<sync_t>()) {
		case Sync::SyncTypes::Config: handleConfigSync(reader); break;
		case Sync::SyncTypes::ChannelStart: PlayerDataProvider::getInstance().parseChannelConnectPacket(reader); break;
		case Sync::SyncTypes::Player: PlayerDataProvider::getInstance().handlePlayerSync(reader); break;
		case Sync::SyncTypes::Party: PlayerDataProvider::getInstance().handlePartySync(reader); break;
		case Sync::SyncTypes::Buddy: PlayerDataProvider::getInstance().handleBuddySync(reader); break;
	}
}

auto SyncHandler::handleConfigSync(PacketReader &reader) -> void {
	switch (reader.get<sync_t>()) {
		case Sync::Config::RateSet: ChannelServer::getInstance().setRates(reader.get<Rates>()); break;
		case Sync::Config::ScrollingHeader: ChannelServer::getInstance().setScrollingHeader(reader.get<string_t>()); break;
	}
}