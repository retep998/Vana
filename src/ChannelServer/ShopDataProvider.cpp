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
#include "GameConstants.h"
#include "GameLogicUtilities.h"
#include "InitializeCommon.h"
#include "Inventory.h"
#include "ItemDataProvider.h"
#include "MapleSession.h"
#include "PacketCreator.h"
#include "Player.h"
#include "SendHeader.h"

using Initializing::outputWidth;

ShopDataProvider * ShopDataProvider::singleton = 0;

void ShopDataProvider::loadData() {
	std::cout << std::setw(outputWidth) << std::left << "Initializing Shops... ";
	mysqlpp::Query query = Database::getDataDB().query("SELECT shopdata.shopid, shopdata.npcid, shopdata.rechargetier, shopitemdata.itemid, shopitemdata.price FROM shopdata LEFT JOIN shopitemdata ON shopdata.shopid = shopitemdata.shopid ORDER BY shopdata.shopid ASC, shopitemdata.sort DESC");
	mysqlpp::UseQueryResult res = query.use();

	MYSQL_ROW shopRow;
	while ((shopRow = res.fetch_raw_row())) {
		// Col0 : Shop ID
		//    1 : NPC ID
		//    2 : Recharge Tier
		//    3 : Item ID
		//    4 : Price
		int32_t shopid = atoi(shopRow[0]);

		if (shops.find(shopid) == shops.end()) {
			ShopInfo shop = ShopInfo();
			shop.npc = atoi(shopRow[1]);
			shop.rechargetier = atoi(shopRow[2]);
			shops[shopid] = shop;
		}

		if (shopRow[3] != 0) {
			shops[shopid].items.push_back(atoi(shopRow[3]));
			shops[shopid].prices[atoi(shopRow[3])] = atoi(shopRow[4]);
		}
		else
			std::cout << "Warning: Shop " << shopid << " does not have any shop items on record.";
	}

	query << "SELECT * FROM rechargedata";
	res = query.use();

	while ((shopRow = res.fetch_raw_row())) {
		// Col0 : Recharge Tier ID
		//    1 : Item ID
		//    2 : Price
		int8_t rechargetier = atoi(shopRow[0]);
		int32_t itemid = atoi(shopRow[1]);
		double price = atof(shopRow[2]);

		rechargecosts[rechargetier][itemid] = price;
	}

	std::cout << "DONE" << std::endl;
}

bool ShopDataProvider::showShop(Player *player, int32_t id) {
	if (shops.find(id) == shops.end())
		return false;

	player->setShop(id);

	unordered_map<int32_t, bool> idsdone;
	int8_t rechargetier = shops[id].rechargetier;
	map<int32_t, double> rechargables = rechargecosts[rechargetier];
	int16_t shopcount = shops[id].items.size() + rechargables.size();

	PacketCreator packet;
	packet.add<int16_t>(SEND_SHOP_OPEN);
	packet.add<int32_t>(shops[id].npc);
	packet.add<int16_t>(0); // To be set later

	// Items
	for (size_t i = 0; i < shops[id].items.size(); i++) {
		int32_t itemid = shops[id].items[i];
		int32_t price = shops[id].prices[itemid];
		packet.add<int32_t>(itemid);
		packet.add<int32_t>(price);
		if (GameLogicUtilities::isRechargeable(itemid)) {
			idsdone[itemid] = true;
			shopcount--;
			double cost = 0.0;
			if (rechargables.find(itemid) != rechargables.end())
				cost = rechargables[itemid];
			packet.add<double>(cost);
		}
		else {
			packet.add<int16_t>(1); // Item amount
		}
		int16_t maxslot = ItemDataProvider::Instance()->getMaxSlot(itemid);
		if (GameLogicUtilities::isStar(itemid))
			packet.add<int16_t>(maxslot + player->getSkills()->getSkillLevel(Jobs::Assassin::ClawMastery) * 10);
		else if (GameLogicUtilities::isBullet(itemid))
			packet.add<int16_t>(maxslot + player->getSkills()->getSkillLevel(Jobs::Gunslinger::GunMastery) * 10);
		else
			packet.add<int16_t>(maxslot);
	}

	// Rechargables
	for (map<int32_t, double>::iterator iter = rechargables.begin(); iter != rechargables.end(); iter++) {
		if (idsdone.find(iter->first) == idsdone.end()) {
			packet.add<int32_t>(iter->first);
			packet.add<int32_t>(0);
			packet.add<double>(iter->second);
			packet.add<int16_t>(ItemDataProvider::Instance()->getMaxSlot(iter->first) + (GameLogicUtilities::isStar(iter->first) ? player->getSkills()->getSkillLevel(4100000) * 10 : player->getSkills()->getSkillLevel(5200000) * 10));
		}
	}

	packet.set<int16_t>(shopcount, 6);

	player->getSession()->send(packet);
	return true;
}

int32_t ShopDataProvider::getPrice(int32_t shopid, int32_t itemid) {
	return shops[shopid].prices.find(itemid) != shops[shopid].prices.end() ? shops[shopid].prices[itemid] : 0;
}

int32_t ShopDataProvider::getRechargeCost(int32_t shopid, int32_t itemid, int16_t amount) {
	int32_t cost = 1;
	if (shops.find(shopid) != shops.end()) {
		int8_t tier = shops[shopid].rechargetier;
		if (rechargecosts.find(tier) != rechargecosts.end()) {
			if (rechargecosts[tier].find(itemid) != rechargecosts[tier].end()) {
				cost = -1 * static_cast<int32_t>(rechargecosts[tier][itemid] * amount);
			}
		}
	}
	return cost;
}