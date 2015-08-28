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

#include "Types.hpp"
#include <map>
#include <unordered_map>
#include <vector>

namespace Vana {
	class PacketBuilder;

	struct ShopItemInfo {
		item_id_t itemId = 0;
		mesos_t price = 0;
		slot_qty_t quantity = 0;
	};

	struct ShopInfo {
		int8_t rechargeTier = 0;
		npc_id_t npc = 0;
		vector_t<ShopItemInfo> items;
	};

	struct BuiltShopInfo {
		npc_id_t npc;
		vector_t<const ShopItemInfo *> items;
		ord_map_t<item_id_t, double> rechargeables;
	};

	class ShopDataProvider {
	public:
		auto loadData() -> void;

		auto isShop(shop_id_t id) const -> bool;
		auto getShop(shop_id_t id) const -> BuiltShopInfo;
		auto getShopItem(shop_id_t shopId, uint16_t shopIndex) const -> const ShopItemInfo * const;
		auto getRechargeCost(shop_id_t shopId, item_id_t itemId, slot_qty_t amount) const -> mesos_t;
	private:
		auto loadShops() -> void;
		auto loadUserShops() -> void;
		auto loadRechargeTiers() -> void;

		hash_map_t<shop_id_t, ShopInfo> m_shops;
		hash_map_t<int8_t, ord_map_t<item_id_t, double>> m_rechargeCosts;
	};
}