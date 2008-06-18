/*
Copyright (C) 2008 Vana Development Team

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
#include "LoginServerConnectHandler.h"
#include "LoginServerConnectPlayer.h"
#include "WorldServerAcceptPlayerPacket.h"
#include "WorldServer.h"
#include "ReadPacket.h"
#include <iostream>

void LoginServerConnectHandler::connect(LoginServerConnectPlayer *player, ReadPacket *packet) {
	char worldid = packet->getByte();
	if (worldid != 0xFF) {
		WorldServer::Instance()->setWorldId(worldid);
		WorldServer::Instance()->setInterPort(packet->getShort());
		WorldServer::Instance()->setMaxChannels(packet->getInt());
		WorldServer::Instance()->listen();
		std::cout << "Handling world " << (int) worldid << std::endl;
	}
	else {
		std::cout << "Error: No world to handle" << std::endl;
		WorldServer::Instance()->shutdown();
	}
}

void LoginServerConnectHandler::newPlayer(ReadPacket *packet) {
	int channel = packet->getInt();
	int playerid = packet->getInt();

	WorldServerAcceptPlayerPacket::newConnectable(channel, playerid);
}
