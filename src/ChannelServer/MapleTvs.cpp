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
#include "MapleTvs.hpp"
#include "Map.hpp"
#include "Player.hpp"
#include "PlayerPacketHelper.hpp"
#include "SmsgHeader.hpp"
#include "TimeUtilities.hpp"
#include "Timer.hpp"
#include <functional>

auto MapleTvs::addMap(Map *map) -> void {
	m_maps[map->getId()] = map;
}

auto MapleTvs::addMessage(Player *sender, Player *receiver, const string_t &msg, const string_t &msg2, const string_t &msg3, const string_t &msg4, const string_t &msg5, int32_t megaphoneId, int32_t time) -> void {
	MapleTvMessage message;
	message.hasReceiver = (receiver != nullptr);
	message.megaphoneId = megaphoneId;
	message.senderId = sender->getId();
	message.time = time;
	message.counter = getCounter();
	message.msg1 = msg;
	message.msg2 = msg2;
	message.msg3 = msg3;
	message.msg4 = msg4;
	message.msg5 = msg5;
	PlayerPacketHelper::addPlayerDisplay(message.sendDisplay, sender); // We need to save the packet since it gets buffered and there's no guarantee the player will exist later
	message.sendName = sender->getName();
	if (receiver != nullptr) {
		PlayerPacketHelper::addPlayerDisplay(message.recvDisplay, receiver);
		message.recvName = receiver->getName();
	}

	m_buffer.push_back(message);

	if (!m_hasMessage) {
		// First element pushed
		parseBuffer();
		m_hasMessage = true;
	}
}

auto MapleTvs::parseBuffer() -> void {
	PacketCreator packet;
	if (m_buffer.size() > 0) {
		MapleTvMessage message = m_buffer.front();
		m_buffer.pop_front();

		getMapleTvPacket(message, packet);
		sendPacket(packet);

		m_currentMessage = message;

		Timer::Id id(Timer::Types::MapleTvTimer, message.senderId, message.counter);
		Timer::Timer::create([this](const time_point_t &now) { this->parseBuffer(); },
			id, getTimers(), seconds_t(message.time));
	}
	else {
		m_hasMessage = false;
		endMapleTvPacket(packet);
		sendPacket(packet);
	}
}

auto MapleTvs::sendPacket(PacketCreator &packet) -> void {
	for (const auto &kvp : m_maps) {
		kvp.second->sendPacket(packet);
	}
}

auto MapleTvs::checkMessageTimer() const -> seconds_t {
	Timer::Id id(Timer::Types::MapleTvTimer, m_currentMessage.senderId, m_currentMessage.counter);
	return getTimers()->getRemainingTime<seconds_t>(id);
}

auto MapleTvs::getMapleTvEntryPacket(PacketCreator &packet) -> void {
	getMapleTvPacket(m_currentMessage, packet, checkMessageTimer());
}

auto MapleTvs::getMapleTvPacket(MapleTvMessage &message, PacketCreator &packet, const seconds_t &timeLeft) -> void {
	packet.add<header_t>(SMSG_MAPLETV_ON);
	packet.add<int8_t>(message.hasReceiver ? 3 : 1);
	packet.add<int8_t>(static_cast<int8_t>(message.megaphoneId - 5075000));
	packet.addBuffer(message.sendDisplay);
	packet.addString(message.sendName);
	packet.addString(message.hasReceiver ? message.recvName : "");
	packet.addString(message.msg1);
	packet.addString(message.msg2);
	packet.addString(message.msg3);
	packet.addString(message.msg4);
	packet.addString(message.msg5);
	packet.add<int32_t>(timeLeft.count() == 0 ? message.time : static_cast<int32_t>(timeLeft.count()));
	if (message.hasReceiver) {
		packet.addBuffer(message.recvDisplay);
	}
}

auto MapleTvs::endMapleTvPacket(PacketCreator &packet) -> void {
	packet.add<header_t>(SMSG_MAPLETV_OFF);
}