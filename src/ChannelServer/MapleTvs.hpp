/*
Copyright (C) 2008-2015 Vana Development Team

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

#include "Common/PacketBuilder.hpp"
#include "Common/TimerContainerHolder.hpp"
#include "Common/Types.hpp"
#include <memory>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

namespace Vana {
	namespace ChannelServer {
		class Map;
		class Player;

		struct MapleTvMessage {
			bool hasReceiver = false;
			int32_t time = 0;
			item_id_t megaphoneId = 0;
			player_id_t senderId = 0;
			uint32_t counter = 0;
			string_t msg1;
			string_t msg2;
			string_t msg3;
			string_t msg4;
			string_t msg5;
			string_t sendName;
			string_t recvName;
			PacketBuilder recvDisplay;
			PacketBuilder sendDisplay;
		};

		class MapleTvs : public TimerContainerHolder {
		public:
			auto addMap(Map *map) -> void;

			auto addMessage(ref_ptr_t<Player> sender, ref_ptr_t<Player> receiver, const string_t &msg, const string_t &msg2, const string_t &msg3, const string_t &msg4, const string_t &msg5, item_id_t megaphoneId, int32_t time) -> void;
			auto isMapleTvMap(map_id_t id) const -> bool;
			auto hasMessage() const -> bool;
			auto getCounter() -> uint32_t;
			auto getCurrentMessage() const -> const MapleTvMessage &;
			auto getMessageTime() const -> seconds_t;
		private:
			auto parseBuffer() -> void;
			auto send(const PacketBuilder &builder) -> void;

			bool m_hasMessage = false;
			uint32_t m_counter = 0;
			MapleTvMessage m_currentMessage;
			queue_t<MapleTvMessage> m_buffer;
			hash_map_t<map_id_t, Map *> m_maps;
		};
	}
}