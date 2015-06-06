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
#include "MapleTvPacket.hpp"
#include "Player.hpp"
#include "PlayerPacketHelper.hpp"
#include "SmsgHeader.hpp"
#include "TimeUtilities.hpp"
#include "Timer.hpp"
#include <functional>

auto MapleTvs::addMap(Map *map) -> void {
	m_maps[map->getId()] = map;
}

auto MapleTvs::addMessage(Player *sender, Player *receiver, const string_t &msg, const string_t &msg2, const string_t &msg3, const string_t &msg4, const string_t &msg5, item_id_t megaphoneId, int32_t time) -> void {
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
	message.sendDisplay.addBuffer(PlayerPacketHelper::addPlayerDisplay(sender)); // We need to save the packet since it gets buffered and there's no guarantee the player will exist later
	message.sendName = sender->getName();
	if (receiver != nullptr) {
		message.recvDisplay.addBuffer(PlayerPacketHelper::addPlayerDisplay(receiver));
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
	if (m_buffer.size() > 0) {
		MapleTvMessage message = m_buffer.front();
		m_buffer.pop_front();

		send(MapleTvPacket::showMessage(message, getMessageTime()));

		m_currentMessage = message;

		Timer::Id id{TimerType::MapleTvTimer, message.senderId, message.counter};
		Timer::Timer::create(
			[this](const time_point_t &now) { this->parseBuffer(); },
			id, getTimers(), seconds_t{message.time});
	}
	else {
		m_hasMessage = false;
		send(MapleTvPacket::endDisplay());
	}
}

auto MapleTvs::send(const PacketBuilder &builder) -> void {
	for (const auto &kvp : m_maps) {
		kvp.second->send(builder);
	}
}

auto MapleTvs::getMessageTime() const -> seconds_t {
	Timer::Id id{TimerType::MapleTvTimer, m_currentMessage.senderId, m_currentMessage.counter};
	return getTimers()->getRemainingTime<seconds_t>(id);
}

auto MapleTvs::isMapleTvMap(map_id_t id) const -> bool {
	return m_maps.find(id) != std::end(m_maps);
}

auto MapleTvs::hasMessage() const -> bool {
	return m_hasMessage;
}

auto MapleTvs::getCounter() -> uint32_t {
	return ++m_counter;
}

auto MapleTvs::getCurrentMessage() const -> const MapleTvMessage & {
	return m_currentMessage;
}
