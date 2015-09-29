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
#include "Common/AbstractConnection.hpp"
#include "Common/Database.hpp"
#include "Common/InterHeader.hpp"
#include "Common/InterHelper.hpp"
#include "Common/PacketReader.hpp"
#include "Common/RatesConfig.hpp"
#include "Common/Session.hpp"
#include "WorldServer/Channel.hpp"
#include "WorldServer/Channels.hpp"
#include "WorldServer/PlayerDataProvider.hpp"
#include "WorldServer/SyncPacket.hpp"
#include "WorldServer/WorldServer.hpp"
#include "WorldServer/WorldServerAcceptConnection.hpp"
#include "WorldServer/WorldServerAcceptPacket.hpp"

namespace Vana {
namespace WorldServer {

auto SyncHandler::handle(AbstractConnection *connection, PacketReader &reader) -> void {
	sync_t type = reader.get<sync_t>();
	switch (type) {
		case Sync::SyncTypes::Config: handleConfigSync(reader); break;
		default: WorldServer::getInstance().getPlayerDataProvider().handleSync(connection, type, reader); break;
	}
}

auto SyncHandler::handleConfigSync(PacketReader &reader) -> void {
	switch (reader.get<sync_t>()) {
		case Sync::Config::RateSet: WorldServer::getInstance().setRates(reader.get<RatesConfig>()); break;
		case Sync::Config::RateReset: WorldServer::getInstance().resetRates(reader.get<int32_t>()); break;
		case Sync::Config::ScrollingHeader: WorldServer::getInstance().setScrollingHeader(reader.get<string_t>()); break;
		default: throw NotImplementedException{"ConfigSync type"};
	}
}

}
}