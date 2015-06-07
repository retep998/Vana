/*
Copyright (C) 2008-2015 Vana Development Team

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
#include "PacketReader.hpp"
#include "PlayerDataProvider.hpp"
#include "Session.hpp"
#include "SyncPacket.hpp"
#include "WorldServer.hpp"
#include "WorldServerAcceptConnection.hpp"
#include "WorldServerAcceptPacket.hpp"

auto SyncHandler::handle(AbstractConnection *connection, PacketReader &reader) -> void {
	sync_t type = reader.get<sync_t>();
	switch (type) {
		case Sync::SyncTypes::Config: handleConfigSync(reader); break;
		default: WorldServer::getInstance().getPlayerDataProvider().handleSync(connection, type, reader); break;
	}
}

auto SyncHandler::handleConfigSync(PacketReader &reader) -> void {
	switch (reader.get<sync_t>()) {
		case Sync::Config::RateSet: WorldServer::getInstance().setRates(reader.get<Rates>()); break;
		case Sync::Config::RateReset: WorldServer::getInstance().resetRates(); break;
		case Sync::Config::ScrollingHeader: WorldServer::getInstance().setScrollingHeader(reader.get<string_t>()); break;
	}
}