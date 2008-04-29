#ifndef PLAYERPACKETHELPER_H
#define PLAYERPACKETHELPER_H

#include "Inventory.h"
#include "PacketCreator.h"

namespace PlayerPacketHelper {
	void addEquip(Packet &packet, Equip* equip, bool pos_is_short = false);
}

#endif