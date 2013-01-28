/*
Copyright (C) 2008-2013 Vana Development Team

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
#include "SyncPacket.h"
#include "AbstractConnection.h"
#include "Channel.h"
#include "Channels.h"
#include "Configuration.h"
#include "InterHeader.h"
#include "InterHelper.h"
#include "MapConstants.h"
#include "PacketCreator.h"
#include "PacketReader.h"
#include "Party.h"
#include "Player.h"
#include "PlayerDataProvider.h"
#include "Session.h"
#include "SmsgHeader.h"
#include "TimeUtilities.h"
#include "WorldServerAcceptConnection.h"

void SyncPacket::sendSyncData(WorldServerAcceptConnection *connection) {
	PacketCreator packet;
	packet.add<header_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::ChannelStart);

	PlayerDataProvider::Instance()->getChannelConnectPacket(packet);

	connection->getSession()->send(packet);
}

void SyncPacket::ConfigPacket::scrollingHeader(const string &message) {
	PacketCreator packet;
	packet.add<header_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Config);
	packet.add<int8_t>(Sync::Config::ScrollingHeader);
	packet.addString(message);
	Channels::Instance()->sendToAll(packet);
}

void SyncPacket::ConfigPacket::setRates(const Rates &rates) {
	PacketCreator packet;
	packet.add<header_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Config);
	packet.add<int8_t>(Sync::Config::RateSet);
	packet.addClass<Rates>(rates);
	Channels::Instance()->sendToAll(packet);
}

void SyncPacket::PlayerPacket::newConnectable(uint16_t channel, int32_t playerId, const Ip &ip, PacketReader &buffer) {
	PacketCreator packet;
	packet.add<header_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Player);
	packet.add<int8_t>(Sync::Player::NewConnectable);
	packet.add<int32_t>(playerId);
	packet.addClass<Ip>(ip);
	packet.add<uint16_t>(buffer.getBufferLength());
	packet.addBuffer(buffer);
	Channels::Instance()->sendToChannel(channel, packet);
}

void SyncPacket::PlayerPacket::deleteConnectable(uint16_t channel, int32_t playerId) {
	PacketCreator packet;
	packet.add<header_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Player);
	packet.add<int8_t>(Sync::Player::DeleteConnectable);
	packet.add<int32_t>(playerId);
	Channels::Instance()->sendToChannel(channel, packet);
}

void SyncPacket::PlayerPacket::playerChangeChannel(AbstractConnection *connection, int32_t playerId, const Ip &ip, port_t port) {
	PacketCreator packet;
	packet.add<header_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Player);
	packet.add<int8_t>(Sync::Player::ChangeChannelGo);
	packet.add<int32_t>(playerId);
	packet.addClass<Ip>(ip);
	packet.add<port_t>(port);
	connection->getSession()->send(packet);
}

void SyncPacket::PlayerPacket::updatePlayerJob(int32_t playerId, int16_t job) {
	PacketCreator packet;
	packet.add<header_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Player);
	packet.add<int8_t>(Sync::Player::UpdatePlayer);
	packet.add<int8_t>(Sync::Player::UpdateBits::Job);
	packet.add<int16_t>(job);
	Channels::Instance()->sendToAll(packet);
}

void SyncPacket::PlayerPacket::updatePlayerLevel(int32_t playerId, int16_t level) {
	PacketCreator packet;
	packet.add<header_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Player);
	packet.add<int8_t>(Sync::Player::UpdatePlayer);
	packet.add<int8_t>(Sync::Player::UpdateBits::Level);
	packet.add<int16_t>(level);
	Channels::Instance()->sendToAll(packet);
}

void SyncPacket::PlayerPacket::updatePlayerMap(int32_t playerId, int32_t map) {
	PacketCreator packet;
	packet.add<header_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Player);
	packet.add<int8_t>(Sync::Player::UpdatePlayer);
	packet.add<int8_t>(Sync::Player::UpdateBits::Map);
	packet.add<int32_t>(map);
	Channels::Instance()->sendToAll(packet);
}

void SyncPacket::PlayerPacket::characterCreated(int32_t playerId) {
	PacketCreator packet;
	packet.add<header_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Player);
	packet.add<int8_t>(Sync::Player::CharacterCreated);
	PlayerDataProvider::Instance()->getPlayerDataPacket(packet, playerId);
	Channels::Instance()->sendToAll(packet);
}

void SyncPacket::PartyPacket::removePartyMember(int32_t partyId, int32_t playerId, bool kicked) {
	PacketCreator packet;
	packet.add<header_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Party);
	packet.add<int8_t>(Sync::Party::RemoveMember);
	packet.add<int32_t>(partyId);
	packet.add<int32_t>(playerId);
	packet.add<bool>(kicked);
	Channels::Instance()->sendToAll(packet);
}

void SyncPacket::PartyPacket::addPartyMember(int32_t partyId, int32_t playerId) {
	PacketCreator packet;
	packet.add<header_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Party);
	packet.add<int8_t>(Sync::Party::AddMember);
	packet.add<int32_t>(partyId);
	packet.add<int32_t>(playerId);
	Channels::Instance()->sendToAll(packet);
}

void SyncPacket::PartyPacket::newPartyLeader(int32_t partyId, int32_t playerId) {
	PacketCreator packet;
	packet.add<header_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Party);
	packet.add<int8_t>(Sync::Party::SwitchLeader);
	packet.add<int32_t>(partyId);
	packet.add<int32_t>(playerId);
	Channels::Instance()->sendToAll(packet);
}

void SyncPacket::PartyPacket::createParty(int32_t partyId, int32_t playerId) {
	PacketCreator packet;
	packet.add<header_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Party);
	packet.add<int8_t>(Sync::Party::Create);
	packet.add<int32_t>(partyId);
	packet.add<int32_t>(playerId);
	Channels::Instance()->sendToAll(packet);
}

void SyncPacket::PartyPacket::disbandParty(int32_t partyId) {
	PacketCreator packet;
	packet.add<header_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Party);
	packet.add<int8_t>(Sync::Party::Disband);
	packet.add<int32_t>(partyId);
	Channels::Instance()->sendToAll(packet);
}

void SyncPacket::BuddyPacket::sendBuddyInvite(Channel *channel, int32_t inviteeId, int32_t inviterId, const string &name) {
	PacketCreator packet;
	packet.add<header_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Buddy);
	packet.add<int8_t>(Sync::Buddy::Invite);
	packet.add<int32_t>(inviterId);
	packet.add<int32_t>(inviteeId);
	packet.addString(name);
	channel->send(packet);
}

void SyncPacket::BuddyPacket::sendBuddyOnlineOffline(Channel *channel, const vector<int32_t> &players, int32_t playerId, int32_t channelId) {
	PacketCreator packet;
	packet.add<header_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Buddy);
	packet.add<int8_t>(Sync::Buddy::OnlineOffline);
	packet.add<int32_t>(playerId);
	packet.add<int32_t>(channelId); // I need to get FF FF FF FF, not FF FF 00 00
	packet.addVector(players);
	channel->send(packet);
}