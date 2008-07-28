/*
Copyright (C) 2008 Vana Development Team

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
#include "Shops.h"
#include "Player.h"
#include "PacketCreator.h"
#include "Drops.h"
#include "Inventory.h"
#include "SendHeader.h"

hash_map <int, ShopInfo> Shops::shops;
vector <int> Shops::rechargables;

void Shops::showShop(Player *player, int id) {
	player->setShop(id);
	if (shops.find(id) == shops.end())
		return;
	PacketCreator packet;
	packet.addShort(SEND_SHOP_OPEN);
	packet.addInt(shops[id].npc);
	packet.addShort(shops[id].items.size() + rechargables.size());
	for (size_t i = 0; i < shops[id].items.size(); i++) {
		packet.addInt(shops[id].items[i].id);
		packet.addInt(shops[id].items[i].price);
		if (ISRECHARGEABLE(shops[id].items[i].id)) {
			packet.addShort(0);
			packet.addInt(0);
			packet.addShort(shops[id].items[i].price);
		}
		else {
			packet.addShort(1);
		}
		if (ISSTAR(shops[id].items[i].id))
			packet.addShort(Inventory::items[shops[id].items[i].id].maxslot + player->skills->getSkillLevel(4100000)*10);
		else if (ISBULLET(shops[id].items[i].id))
			packet.addShort(Inventory::items[shops[id].items[i].id].maxslot + player->skills->getSkillLevel(5200000)*10);
		else if (Inventory::items.find(shops[id].items[i].id) != Inventory::items.end())
			packet.addShort(Inventory::items[shops[id].items[i].id].maxslot); 
		else
			packet.addShort(1000);
	}

	for (size_t i = 0; i < rechargables.size(); i++) {
		packet.addInt(rechargables[i]);
		packet.addInt(0);
		packet.addShort(0);
		packet.addInt(0);
		packet.addShort(1);
		packet.addShort(Inventory::items[rechargables[i]].maxslot + player->skills->getSkillLevel(4100000)*10);
	}

	//packet.addBytes("333333333333d33ff401f1951f00000000009a9999999999d93ff401f2951f0000000000000000000000e03fbc02f3951f00000000000333333333333e33ff401f4951f0000000000333333333333e33fe803f5951f0000000000666666666666e63fe803f6951f000000000009a9999999999e93f2003f7951f000000000000000000");
	//Struct: unitPrice [6] unk [2] price [2] id [2] 0 [4]
	packet.send(player);
}

int Shops::getPrice(Player *player, int itemid) {
	for (unsigned int i=0; i<shops[player->getShop()].items.size(); i++) {
		if (shops[player->getShop()].items[i].id == itemid)
			return shops[player->getShop()].items[i].price;
	}
	return 0;
}

void Shops::addShop(int id, ShopInfo shop) {
	shops[id] = shop;
}
