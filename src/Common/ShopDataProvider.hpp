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
#include <map>
#include <unordered_map>
#include <vector>

class PacketCreator;

struct ShopItemInfo {
	int32_t itemId = 0;
	int32_t price = 0;
	int16_t quantity = 0;
};

struct ShopInfo {
	int8_t rechargeTier = 0;
	int32_t npc = 0;
	vector_t<ShopItemInfo> items;
};

class ShopDataProvider {
	SINGLETON(ShopDataProvider);
public:
	auto loadData() -> void;

	auto isShop(int32_t id) -> bool { return m_shops.find(id) != std::end(m_shops); }
	auto showShop(int32_t id, int16_t rechargeableBonus, PacketCreator &packet) -> void;
	auto getPrice(int32_t shopId, uint16_t shopIndex) -> int32_t;
	auto getItemId(int32_t shopId, uint16_t shopIndex) -> int32_t;
	auto getAmount(int32_t shopId, uint16_t shopIndex) -> int16_t;
	auto getRechargeCost(int32_t shopId, int32_t itemId, int16_t amount) -> int32_t;
private:
	auto loadShops() -> void;
	auto loadUserShops() -> void;
	auto loadRechargeTiers() -> void;

	hash_map_t<int32_t, ShopInfo> m_shops;
	hash_map_t<int8_t, ord_map_t<int32_t, double>> m_rechargeCosts;
};