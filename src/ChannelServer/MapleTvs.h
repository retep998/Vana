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

#include "PacketCreator.h"
#include "TimerContainerHolder.h"
#include "Types.h"
#include <memory>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

class Map;
class Player;

struct MapleTvMessage {
	bool hasReceiver = false;
	int32_t time = 0;
	int32_t megaphoneId = 0;
	int32_t senderId = 0;
	uint32_t counter = 0;
	string_t msg1;
	string_t msg2;
	string_t msg3;
	string_t msg4;
	string_t msg5;
	string_t sendName;
	string_t recvName;
	PacketCreator recvDisplay;
	PacketCreator sendDisplay;
};

class MapleTvs : public TimerContainerHolder {
	SINGLETON(MapleTvs);
public:
	auto addMap(Map *map) -> void;

	auto addMessage(Player *sender, Player *receiver, const string_t &msg, const string_t &msg2, const string_t &msg3, const string_t &msg4, const string_t &msg5, int32_t megaphoneId, int32_t time) -> void;
	auto getMapleTvEntryPacket(PacketCreator &packet) -> void;
	auto isMapleTvMap(int32_t id) const -> bool { return m_maps.find(id) != std::end(m_maps); }
	auto hasMessage() const -> bool { return m_hasMessage; }
	auto getCounter() -> uint32_t { return ++m_counter; }
private:
	auto parseBuffer() -> void;
	auto getMapleTvPacket(MapleTvMessage &message, PacketCreator &packet, const seconds_t &timeLeft = seconds_t(0)) -> void;
	auto endMapleTvPacket(PacketCreator &packet) -> void;
	auto sendPacket(PacketCreator &packet) -> void;
	auto checkMessageTimer() const -> seconds_t;

	bool m_hasMessage = false;
	uint32_t m_counter = 0;
	MapleTvMessage m_currentMessage;
	queue_t<MapleTvMessage> m_buffer;
	hash_map_t<int32_t, Map *> m_maps;
};