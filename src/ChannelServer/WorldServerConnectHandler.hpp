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

class PacketReader;
class WorldServerConnection;

namespace WorldServerConnectHandler {
	auto connectLogin(WorldServerConnection *player, PacketReader &packet) -> void;
	auto connect(WorldServerConnection *player, PacketReader &packet) -> void;
	auto findPlayer(PacketReader &packet) -> void;
	auto whisperPlayer(PacketReader &packet) -> void;
	auto forwardPacket(PacketReader &packet) -> void;
	auto sendToPlayers(PacketReader &packet) -> void;
	auto reloadMcdb(PacketReader &packet) -> void;
	auto rehashConfig(PacketReader &packet) -> void;
}