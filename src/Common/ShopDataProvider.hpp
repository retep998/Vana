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

class PacketBuilder;

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

struct BuiltShopInfo {
	int32_t npc;
	vector_t<const ShopItemInfo *> items;
	ord_map_t<int32_t, double> rechargeables;
};

class ShopDataProvider {
	SINGLETON(ShopDataProvider);
public:
	auto loadData() -> void;

	auto isShop(int32_t id) const -> bool;
	auto getShop(int32_t id) const -> BuiltShopInfo;
	auto getShopItem(int32_t shopId, uint16_t shopIndex) const -> const ShopItemInfo * const;
	auto getRechargeCost(int32_t shopId, int32_t itemId, int16_t amount) const -> int32_t;
private:
	auto loadShops() -> void;
	auto loadUserShops() -> void;
	auto loadRechargeTiers() -> void;

	hash_map_t<int32_t, ShopInfo> m_shops;
	hash_map_t<int8_t, ord_map_t<int32_t, double>> m_rechargeCosts;
};