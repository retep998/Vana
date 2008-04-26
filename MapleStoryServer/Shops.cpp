#include "Shops.h"
#include "Player.h"
#include "PacketCreator.h"
#include "Drops.h"
#include "Inventory.h"

hash_map <int, ShopInfo> Shops::shops;

void Shops::showShop(Player* player, int id){
	player->setShop(id);
	if(shops.find(id) == shops.end())
		return;
	Packet packet = Packet();
	packet.addHeader(0xD6);
	packet.addInt(shops[id].npc);
	packet.addShort(shops[id].items.size());
	for(unsigned int i=0; i<shops[id].items.size(); i++){
		packet.addInt(shops[id].items[i].id);
		packet.addInt(shops[id].items[i].price);
		if(ISSTAR(shops[id].items[i].id)) continue;
		packet.addShort(1);
		if(Drops::items.find(shops[id].items[i].id) != Drops::items.end())
			packet.addShort(Drops::items[shops[id].items[i].id].maxslot); 
		else
			packet.addShort(1000);
	}

	//packet.addBytes("333333333333d33ff401f1951f00000000009a9999999999d93ff401f2951f0000000000000000000000e03fbc02f3951f00000000000333333333333e33ff401f4951f0000000000333333333333e33fe803f5951f0000000000666666666666e63fe803f6951f000000000009a9999999999e93f2003f7951f000000000000000000");
	//Struct: unitPrice [6] unk [2] price [2] id [2] 0 [4]
	packet.packetSend(player);
}

int Shops::getPrice(Player* player, int itemid){
	for(unsigned int i=0; i<shops[player->getShop()].items.size(); i++){
		if(shops[player->getShop()].items[i].id == itemid)
			return shops[player->getShop()].items[i].price;
	}
	return 0;
}