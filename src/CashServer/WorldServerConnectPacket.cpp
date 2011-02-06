/*
Copyright (C) 2008-2011 Vana Development Team

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
#include "WorldServerConnectPacket.h"
#include "InterHeader.h"
#include "SendHeader.h"
#include "MapleSession.h"
#include "PacketCreator.h"
#include "WorldServerConnection.h"

void WorldServerConnectPacket::toChannels(WorldServerConnection *player, PacketCreator &packet) {
	PacketCreator pack;
	pack.add<int16_t>(IMSG_TO_CHANNELS);
	pack.addBuffer(packet);
	player->getSession()->send(pack);
}

void WorldServerConnectPacket::toWorlds(WorldServerConnection *player, PacketCreator &packet) {
	PacketCreator pack;
	pack.add<int16_t>(IMSG_TO_WORLDS);
	pack.addBuffer(packet);
	player->getSession()->send(pack);
}

void WorldServerConnectPacket::updateCoupon(WorldServerConnection *player, string coupon, bool used) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_TO_LOGIN);
	packet.add<int16_t>(IMSG_TO_WORLDS);
	packet.add<int16_t>(IMSG_TO_CASH_SERVER);
	packet.add<int16_t>(IMSG_UPDATE_COUPON);
	packet.addString(coupon);
	packet.addBool(used);
	player->getSession()->send(packet);
}

void WorldServerConnectPacket::reloadBestItems(WorldServerConnection *player) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_TO_LOGIN);
	packet.add<int16_t>(IMSG_TO_WORLDS);
	packet.add<int16_t>(IMSG_TO_CASH_SERVER);
	packet.add<int16_t>(IMSG_RECALCULATE_BEST_ITEMS);
	player->getSession()->send(packet);
}
