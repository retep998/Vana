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
#include "Channels.h"
#include "ReadPacket.h"
#include "Rates.h"
#include <iostream>

void LoginServerConnectHandler::connect(LoginServerConnectPlayer *player, ReadPacket *packet) {
	char worldid = packet->getByte();
	if (worldid != 0xFF) {
		WorldServer::Instance()->setWorldId(worldid);
		WorldServer::Instance()->setInterPort(packet->getShort());
		WorldServer::Instance()->setMaxChannels(packet->getInt());

		int ratesSetBit = packet->getInt();
		if (ratesSetBit & Rates::SetBits::exp) {
			WorldServer::Instance()->setExprate(packet->getInt());
		}
		if (ratesSetBit & Rates::SetBits::questExp) {
			WorldServer::Instance()->setQuestExprate(packet->getInt());
		}
		if (ratesSetBit & Rates::SetBits::meso) {
			WorldServer::Instance()->setMesorate(packet->getInt());
		}
		if (ratesSetBit & Rates::SetBits::drop) {
			WorldServer::Instance()->setDroprate(packet->getInt());
		}

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

	if (Channels::Instance()->getChannel(channel)) {
		WorldServerAcceptPlayerPacket::newConnectable(channel, playerid);
	}
}
