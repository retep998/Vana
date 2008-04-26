#include "PlayerInventory.h"
#include "InventoryPacket.h"

void PlayerInventory::setMesos(int mesos, bool is){
	this->mesos=mesos;
	InventoryPacket::newMesos(player, mesos, is);
}
