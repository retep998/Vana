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
#include "SyncPacket.hpp"
#include "AbstractConnection.hpp"
#include "Channel.hpp"
#include "Channels.hpp"
#include "Configuration.hpp"
#include "InterHeader.hpp"
#include "InterHelper.hpp"
#include "MapConstants.hpp"
#include "PacketCreator.hpp"
#include "PacketReader.hpp"
#include "PlayerDataProvider.hpp"
#include "Session.hpp"
#include "SmsgHeader.hpp"
#include "TimeUtilities.hpp"
#include "WorldServerAcceptConnection.hpp"

auto SyncPacket::sendSyncData(WorldServerAcceptConnection *connection) -> void {
	PacketCreator packet;
	packet.add<header_t>(IMSG_SYNC);
	packet.add<sync_t>(Sync::SyncTypes::ChannelStart);

	PlayerDataProvider::getInstance().getChannelConnectPacket(packet);

	connection->getSession()->send(packet);
}

auto SyncPacket::ConfigPacket::scrollingHeader(const string_t &message) -> void {
	PacketCreator packet;
	packet.add<header_t>(IMSG_SYNC);
	packet.add<sync_t>(Sync::SyncTypes::Config);
	packet.add<sync_t>(Sync::Config::ScrollingHeader);
	packet.addString(message);
	Channels::getInstance().sendToAll(packet);
}

auto SyncPacket::ConfigPacket::setRates(const Rates &rates) -> void {
	PacketCreator packet;
	packet.add<header_t>(IMSG_SYNC);
	packet.add<sync_t>(Sync::SyncTypes::Config);
	packet.add<sync_t>(Sync::Config::RateSet);
	packet.addClass<Rates>(rates);
	Channels::getInstance().sendToAll(packet);
}

auto SyncPacket::PlayerPacket::newConnectable(channel_id_t channel, int32_t playerId, const Ip &ip, PacketReader &buffer) -> void {
	PacketCreator packet;
	packet.add<header_t>(IMSG_SYNC);
	packet.add<sync_t>(Sync::SyncTypes::Player);
	packet.add<sync_t>(Sync::Player::NewConnectable);
	packet.add<int32_t>(playerId);
	packet.addClass<Ip>(ip);
	packet.add<uint16_t>(buffer.getBufferLength());
	packet.addBuffer(buffer);
	Channels::getInstance().sendToChannel(channel, packet);
}

auto SyncPacket::PlayerPacket::deleteConnectable(channel_id_t channel, int32_t playerId) -> void {
	PacketCreator packet;
	packet.add<header_t>(IMSG_SYNC);
	packet.add<sync_t>(Sync::SyncTypes::Player);
	packet.add<sync_t>(Sync::Player::DeleteConnectable);
	packet.add<int32_t>(playerId);
	Channels::getInstance().sendToChannel(channel, packet);
}

auto SyncPacket::PlayerPacket::playerChangeChannel(AbstractConnection *connection, int32_t playerId, channel_id_t channelId, const Ip &ip, port_t port) -> void {
	PacketCreator packet;
	packet.add<header_t>(IMSG_SYNC);
	packet.add<sync_t>(Sync::SyncTypes::Player);
	packet.add<sync_t>(Sync::Player::ChangeChannelGo);
	packet.add<int32_t>(playerId);
	packet.add<channel_id_t>(channelId);
	packet.addClass<Ip>(ip);
	packet.add<port_t>(port);
	connection->getSession()->send(packet);
}

auto SyncPacket::PlayerPacket::updatePlayer(const PlayerData &data, update_bits_t flags) -> void {
	PacketCreator packet;
	packet.add<header_t>(IMSG_SYNC);
	packet.add<sync_t>(Sync::SyncTypes::Player);
	packet.add<sync_t>(Sync::Player::UpdatePlayer);
	packet.add<int32_t>(data.id);
	packet.add<update_bits_t>(flags);
	if (flags & Sync::Player::UpdateBits::Full) {
		packet.addClass<PlayerData>(data);
	}
	else {
		if (flags & Sync::Player::UpdateBits::Level) {
			packet.add<int16_t>(data.level);
		}
		if (flags & Sync::Player::UpdateBits::Job) {
			packet.add<int16_t>(data.job);
		}
		if (flags & Sync::Player::UpdateBits::Map) {
			packet.add<int32_t>(data.map);
		}
		if (flags & Sync::Player::UpdateBits::Channel) {
			packet.add<channel_id_t>(data.channel);
		}
	}
	Channels::getInstance().sendToAll(packet);
}

auto SyncPacket::PlayerPacket::characterCreated(const PlayerData &data) -> void {
	PacketCreator packet;
	packet.add<header_t>(IMSG_SYNC);
	packet.add<sync_t>(Sync::SyncTypes::Player);
	packet.add<sync_t>(Sync::Player::CharacterCreated);
	packet.addClass<PlayerData>(data);
	Channels::getInstance().sendToAll(packet);
}

auto SyncPacket::PlayerPacket::characterDeleted(int32_t id) -> void {
	PacketCreator packet;
	packet.add<header_t>(IMSG_SYNC);
	packet.add<sync_t>(Sync::SyncTypes::Player);
	packet.add<sync_t>(Sync::Player::CharacterDeleted);
	packet.add<int32_t>(id);
	Channels::getInstance().sendToAll(packet);
}

auto SyncPacket::PartyPacket::removePartyMember(int32_t partyId, int32_t playerId, bool kicked) -> void {
	PacketCreator packet;
	packet.add<header_t>(IMSG_SYNC);
	packet.add<sync_t>(Sync::SyncTypes::Party);
	packet.add<sync_t>(Sync::Party::RemoveMember);
	packet.add<int32_t>(partyId);
	packet.add<int32_t>(playerId);
	packet.add<bool>(kicked);
	Channels::getInstance().sendToAll(packet);
}

auto SyncPacket::PartyPacket::addPartyMember(int32_t partyId, int32_t playerId) -> void {
	PacketCreator packet;
	packet.add<header_t>(IMSG_SYNC);
	packet.add<sync_t>(Sync::SyncTypes::Party);
	packet.add<sync_t>(Sync::Party::AddMember);
	packet.add<int32_t>(partyId);
	packet.add<int32_t>(playerId);
	Channels::getInstance().sendToAll(packet);
}

auto SyncPacket::PartyPacket::newPartyLeader(int32_t partyId, int32_t playerId) -> void {
	PacketCreator packet;
	packet.add<header_t>(IMSG_SYNC);
	packet.add<sync_t>(Sync::SyncTypes::Party);
	packet.add<sync_t>(Sync::Party::SwitchLeader);
	packet.add<int32_t>(partyId);
	packet.add<int32_t>(playerId);
	Channels::getInstance().sendToAll(packet);
}

auto SyncPacket::PartyPacket::createParty(int32_t partyId, int32_t playerId) -> void {
	PacketCreator packet;
	packet.add<header_t>(IMSG_SYNC);
	packet.add<sync_t>(Sync::SyncTypes::Party);
	packet.add<sync_t>(Sync::Party::Create);
	packet.add<int32_t>(partyId);
	packet.add<int32_t>(playerId);
	Channels::getInstance().sendToAll(packet);
}

auto SyncPacket::PartyPacket::disbandParty(int32_t partyId) -> void {
	PacketCreator packet;
	packet.add<header_t>(IMSG_SYNC);
	packet.add<sync_t>(Sync::SyncTypes::Party);
	packet.add<sync_t>(Sync::Party::Disband);
	packet.add<int32_t>(partyId);
	Channels::getInstance().sendToAll(packet);
}

auto SyncPacket::BuddyPacket::sendBuddyInvite(Channel *channel, int32_t inviteeId, int32_t inviterId, const string_t &name) -> void {
	PacketCreator packet;
	packet.add<header_t>(IMSG_SYNC);
	packet.add<sync_t>(Sync::SyncTypes::Buddy);
	packet.add<sync_t>(Sync::Buddy::Invite);
	packet.add<int32_t>(inviterId);
	packet.add<int32_t>(inviteeId);
	packet.addString(name);
	channel->send(packet);
}

auto SyncPacket::BuddyPacket::sendBuddyOnlineOffline(Channel *channel, const vector_t<int32_t> &players, int32_t playerId, channel_id_t channelId) -> void {
	PacketCreator packet;
	packet.add<header_t>(IMSG_SYNC);
	packet.add<sync_t>(Sync::SyncTypes::Buddy);
	packet.add<sync_t>(Sync::Buddy::OnlineOffline);
	packet.add<int32_t>(playerId);
	packet.add<channel_id_t>(channelId);
	packet.addVector(players);
	channel->send(packet);
}