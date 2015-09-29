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

#include "Common/Types.hpp"
#include "ChannelServer/Summon.hpp"
#include <vector>

namespace Vana {
	class PacketBuilder;
	class PacketReader;

	namespace ChannelServer {
		class Player;

		class PlayerSummons {
			NONCOPYABLE(PlayerSummons);
			NO_DEFAULT_CONSTRUCTOR(PlayerSummons);
		public:
			PlayerSummons(Player *player);

			auto getSummon(summon_id_t summonId) -> Summon *;
			auto addSummon(Summon *summon, seconds_t time) -> void;
			auto removeSummon(summon_id_t summonId, bool fromTimer) -> void;
			auto changedMap() -> void;
			auto forEach(function_t<void(Summon *)> func) -> void;

			auto getTransferPacket() const -> PacketBuilder;
			auto parseTransferPacket(PacketReader &reader) -> void;
		private:
			auto getSummonTimeRemaining(summon_id_t summonId) const -> seconds_t;

			Player *m_player = nullptr;
			vector_t<Summon *> m_summons;
		};
	}
}