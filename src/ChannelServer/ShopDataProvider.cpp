/*
Copyright (C) 2008-2009 Vana Development Team

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
#include "ShopDataProvider.h"
#include "Database.h"
#include "InitializeCommon.h"
#include "Inventory.h"
#include "ItemDataProvider.h"
#include "MapleSession.h"
#include "PacketCreator.h"
#include "Player.h"
#include "SendHeader.h"

using Initializing::outputWidth;

ShopDataProvider *ShopDataProvider::singleton = 0;

void ShopDataProvider::loadData() {
	std::cout << std::setw(outputWidth) << std::left << "Initializing Shops... ";
	mysqlpp::Query query = Database::getDataDB().query("SELECT shopdata.shopid, shopdata.npcid, shopitemdata.itemid, shopitemdata.price FROM shopdata LEFT JOIN shopitemdata ON shopdata.shopid=shopitemdata.shopid ORDER BY shopdata.shopid ASC, shopitemdata.sort DESC");
	mysqlpp::UseQueryResult res = query.use();

	MYSQL_ROW shopRow;
	while ((shopRow = res.fetch_raw_row())) {
		// Col0 : Shop ID
		//    1 : NPC ID
		//    2 : Item ID
		//    3 : Price
		int32_t shopid = atoi(shopRow[0]);

		if (shops.find(shopid) == shops.end()) {
			ShopInfo shop = ShopInfo();
			shop.npc = atoi(shopRow[1]);
			shops[shopid] = shop;
		}

		if (shopRow[2] != 0) {
			shops[shopid].items.push_back(atoi(shopRow[2]));
			shops[shopid].prices[atoi(shopRow[2])] = atoi(shopRow[3]);
		}
		else std::cout << "Warning: Shop " << shopid << " does not have any shop items on record.";
	}
	std::cout << "DONE" << std::endl;
}

bool ShopDataProvider::showShop(Player *player, int32_t id) {
	if (shops.find(id) == shops.end())
		return false;

	player->setShop(id);
	PacketCreator packet;
	packet.addShort(SEND_SHOP_OPEN);
	packet.addInt(shops[id].npc);
	packet.addShort(shops[id].items.size() + rechargables.size());
	for (size_t i = 0; i < shops[id].items.size(); i++) {
		packet.addInt(shops[id].items[i]);
		packet.addInt(shops[id].prices[shops[id].items[i]]);
		if (ISRECHARGEABLE(shops[id].items[i])) {
			packet.addShort(0);
			packet.addInt(0);
			packet.addShort((uint16_t) shops[id].prices[shops[id].items[i]]);
		}
		else {
			packet.addShort(1);
		}
		int16_t maxslot = ItemDataProvider::Instance()->getMaxslot(shops[id].items[i]);
		if (ISSTAR(shops[id].items[i]))
			packet.addShort(maxslot + player->getSkills()->getSkillLevel(4100000) * 10);
		else if (ISBULLET(shops[id].items[i]))
			packet.addShort(maxslot + player->getSkills()->getSkillLevel(5200000) * 10);
		else
			packet.addShort(maxslot);
	}

	for (size_t i = 0; i < rechargables.size(); i++) {
		packet.addInt(rechargables[i]);
		packet.addInt(0);
		packet.addShort(0);
		packet.addInt(0);
		packet.addShort(1);
		packet.addShort(ItemDataProvider::Instance()->getMaxslot(rechargables[i]) + (ISSTAR(rechargables[i]) ? player->getSkills()->getSkillLevel(4100000) * 10 : player->getSkills()->getSkillLevel(5200000) * 10));
	}

	player->getSession()->send(packet);
	return true;
}

int32_t ShopDataProvider::getPrice(int32_t shopid, int32_t itemid) {
	return shops[shopid].prices.find(itemid) != shops[shopid].prices.end() ? shops[shopid].prices[itemid] : 0;
}
