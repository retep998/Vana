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
#pragma once

#include "Types.hpp"

class PacketReader;

namespace SyncHandler {
	auto handle(PacketReader &packet) -> void;
	auto handleConfigSync(PacketReader &packet) -> void;
	auto handleChannelSync(PacketReader &packet) -> void;
	auto handlePlayerSync(PacketReader &packet) -> void;
	auto handlePartySync(PacketReader &packet) -> void;
	auto handleBuddySync(PacketReader &packet) -> void;
	auto buddyInvite(PacketReader &packet) -> void;
	auto buddyOnlineOffline(PacketReader &packet) -> void;
	auto setRates(PacketReader &packet) -> void;
}