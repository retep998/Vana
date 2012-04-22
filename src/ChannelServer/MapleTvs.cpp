/*
Copyright (C) 2008-2012 Vana Development Team

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
#include "MapleTvs.h"
#include "Map.h"
#include "Player.h"
#include "PlayerPacketHelper.h"
#include "SmsgHeader.h"
#include "TimeUtilities.h"
#include "Timer.h"
#include <functional>

using std::bind;

MapleTvs * MapleTvs::singleton = nullptr;

MapleTvs::MapleTvs() :
	m_timers(new Timer::Container),
	m_counter(0),
	m_hasMessage(false)
{
}

void MapleTvs::addMap(Map *map) {
	m_maps[map->getId()] = map;
}

void MapleTvs::addMessage(Player *sender, Player *receiver, const string &msg, const string &msg2, const string &msg3, const string &msg4, const string &msg5, int32_t megaphoneId, int32_t time) {
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

void MapleTvs::parseBuffer() {
	PacketCreator packet;
	if (m_buffer.size() > 0) {
		MapleTvMessage message = m_buffer.front();
		m_buffer.pop_front();

		getMapleTvPacket(message, packet);
		sendPacket(packet);

		m_currentMessage = message;

		Timer::Id id(Timer::Types::MapleTvTimer, message.senderId, message.counter);
		new Timer::Timer(bind(&MapleTvs::parseBuffer, this),
			id, getTimers(), TimeUtilities::fromNow(message.time * 1000));
	}
	else {
		m_hasMessage = false;
		endMapleTvPacket(packet);
		sendPacket(packet);
	}
}

void MapleTvs::sendPacket(PacketCreator &packet) {
	for (unordered_map<int32_t, Map *>::iterator iter = m_maps.begin(); iter != m_maps.end(); ++iter) {
		iter->second->sendPacket(packet);
	}
}

int32_t MapleTvs::checkMessageTimer() const {
	Timer::Id id(Timer::Types::MapleTvTimer, m_currentMessage.senderId, m_currentMessage.counter);
	return getTimers()->checkTimer(id);
}

void MapleTvs::getMapleTvEntryPacket(PacketCreator &packet) {
	 getMapleTvPacket(m_currentMessage, packet, checkMessageTimer());
}

void MapleTvs::getMapleTvPacket(MapleTvMessage &message, PacketCreator &packet, int32_t timeLeft) {
	packet.add<header_t>(SMSG_MAPLETV_ON);
	packet.add<int8_t>(message.hasReceiver ? 3 : 1);
	packet.add<int8_t>(static_cast<int8_t>(message.megaphoneId - 5075000)); // Positively will be within -128 to 127
	packet.addBuffer(message.sendDisplay);
	packet.addString(message.sendName);
	packet.addString(message.hasReceiver ? message.recvName : "");
	packet.addString(message.msg1);
	packet.addString(message.msg2);
	packet.addString(message.msg3);
	packet.addString(message.msg4);
	packet.addString(message.msg5);
	packet.add<int32_t>(timeLeft == 0 ? message.time : timeLeft);
	if (message.hasReceiver) {
		packet.addBuffer(message.recvDisplay);
	}
}

void MapleTvs::endMapleTvPacket(PacketCreator &packet) {
	packet.add<header_t>(SMSG_MAPLETV_OFF);
}