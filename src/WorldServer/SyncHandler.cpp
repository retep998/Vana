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
#include "AbstractConnection.hpp"
#include "Channel.hpp"
#include "Channels.hpp"
#include "Database.hpp"
#include "GameObjects.hpp"
#include "InterHeader.hpp"
#include "InterHelper.hpp"
#include "PacketCreator.hpp"
#include "PacketReader.hpp"
#include "PlayerDataProvider.hpp"
#include "Session.hpp"
#include "SmsgHeader.hpp"
#include "SyncPacket.hpp"
#include "WorldServer.hpp"
#include "WorldServerAcceptConnection.hpp"
#include "WorldServerAcceptPacket.hpp"

auto SyncHandler::handle(AbstractConnection *connection, PacketReader &packet) -> void {
	switch (packet.get<sync_t>()) {
		case Sync::SyncTypes::Config: handleConfigSync(packet); break;
		case Sync::SyncTypes::Player: PlayerDataProvider::getInstance().handlePlayerSync(connection, packet); break;
		case Sync::SyncTypes::Party: PlayerDataProvider::getInstance().handlePartySync(connection, packet); break;
		case Sync::SyncTypes::Buddy: PlayerDataProvider::getInstance().handleBuddySync(connection, packet); break;
	}
}

auto SyncHandler::handleConfigSync(PacketReader &packet) -> void {
	switch (packet.get<sync_t>()) {
		case Sync::Config::RateSet: handleSetRates(packet); break;
		case Sync::Config::RateReset: WorldServer::getInstance().resetRates(); break;
		case Sync::Config::ScrollingHeader: handleScrollingHeader(packet); break;
	}
}

auto SyncHandler::handleSetRates(PacketReader &packet) -> void {
	Rates rates = packet.getClass<Rates>();
	WorldServer::getInstance().setRates(rates);
}

auto SyncHandler::handleScrollingHeader(PacketReader &packet) -> void {
	string_t message = packet.getString();
	WorldServer::getInstance().setScrollingHeader(message);
}