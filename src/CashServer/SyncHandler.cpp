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
#include "SyncHandler.h"
#include "Connectable.h"
#include "GameObjects.h"
#include "InterHeader.h"
#include "InterHelper.h"
#include "PacketCreator.h"
#include "PacketReader.h"
#include "Player.h"
#include "PlayerDataProvider.h"
#include "PlayerPacket.h"
#include "SendHeader.h"
#include "SyncPacket.h"

void SyncHandler::handle(PacketReader &packet) {
	switch (packet.get<int8_t>()) {
		case Sync::SyncTypes::Player: handlePlayerSync(packet); break;
	}
}

void SyncHandler::handlePlayerSync(PacketReader &packet) {
	switch (packet.get<int8_t>()) {
		case Sync::Player::NewConnectable: newConnectable(packet); break;
		case Sync::Player::ChangeChannelGo: playerChangeChannel(packet); break;
		case Sync::Player::PacketTransfer: PlayerDataProvider::Instance()->parseIncomingPacket(packet); break;
		case Sync::Player::RemovePacketTransfer: PlayerDataProvider::Instance()->removePacket(packet.get<int32_t>()); break;
		case Sync::Player::Disconnect: disconnectPlayer(packet); break;
	}
}

void SyncHandler::playerChangeChannel(PacketReader &packet) {
	int32_t playerid = packet.get<int32_t>();
	uint32_t ip = packet.get<uint32_t>();
	uint16_t port = packet.get<uint16_t>();

	Player *ccPlayer = PlayerDataProvider::Instance()->getPlayer(playerid);
	if (!ccPlayer) {
		return;
	}
	if (ip == 0) {
		PlayerPacket::sendBlockedMessage(ccPlayer, PlayerPacket::BlockMessages::CannotGo);
	}
	else {
		ccPlayer->setOnline(false); // Set online to 0 BEFORE CC packet is sent to player
		PlayerPacket::changeChannel(ccPlayer, ip, port);
		ccPlayer->saveAll(true);
		ccPlayer->setSaveOnDc(false);
	}
}

void SyncHandler::newConnectable(PacketReader &packet) {
	int32_t playerid = packet.get<int32_t>();
	uint32_t playerip = packet.get<uint32_t>();
	Connectable::Instance()->newPlayer(playerid, playerip);
}

void SyncHandler::disconnectPlayer(PacketReader &packet) {
	int32_t playerid = packet.get<int32_t>();
	Player *player = PlayerDataProvider::Instance()->getPlayer(playerid);
	if (!player) {
		return;
	}

	PlayerPacket::disconnectPlayer(player);
}
