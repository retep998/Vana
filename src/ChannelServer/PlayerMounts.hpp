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

#include "Types.hpp"
#include <unordered_map>

class PacketBuilder;
class Player;

struct MountData {
	int16_t exp = 0;
	int8_t tiredness = 0;
	int8_t level = 0;
};

class PlayerMounts {
	NONCOPYABLE(PlayerMounts);
	NO_DEFAULT_CONSTRUCTOR(PlayerMounts);
public:
	PlayerMounts(Player *p);

	auto save() -> void;
	auto load() -> void;

	auto mountInfoPacket(PacketBuilder &packet) -> void;
	auto getCurrentMount() const -> item_id_t { return m_currentMount; }
	auto getCurrentExp() -> int16_t;
	auto getCurrentLevel() -> int8_t;
	auto getCurrentTiredness() -> int8_t;
	auto setCurrentMount(item_id_t id) -> void { m_currentMount = id; }
	auto setCurrentExp(int16_t exp) -> void;
	auto setCurrentLevel(int8_t level) -> void;
	auto setCurrentTiredness(int8_t tiredness) -> void;

	auto addMount(item_id_t id) -> void;

	auto getMountExp(item_id_t id) -> int16_t;
	auto getMountLevel(item_id_t id) -> int8_t;
	auto getMountTiredness(item_id_t id) -> int8_t;
private:
	item_id_t m_currentMount = 0;
	Player *m_player = nullptr;
	hash_map_t<item_id_t, MountData> m_mounts;
};