#include "PlayerPacketHelper.h"

void PlayerPacketHelper::addEquip(Packet &packet, Equip* equip, bool pos_is_short) {
	if (pos_is_short)
		packet.addShort(equip->pos);
	else if (equip->pos < 0)
		packet.addByte(equip->type);
	else
		packet.addByte((char) equip->pos);
	packet.addByte(1);
	packet.addInt(equip->id);
	packet.addShort(0);
	packet.addBytes("8005BB46E61702");
	packet.addByte(equip->slots); // slots
	packet.addByte((char) equip->scrolls); // scrolls
	packet.addShort(equip->istr); // STR
	packet.addShort(equip->idex); // DEX
	packet.addShort(equip->iint); // INT
	packet.addShort(equip->iluk); // LUK
	packet.addShort(equip->ihp); // HP
	packet.addShort(equip->imp); // MP
	packet.addShort(equip->iwatk); // W.Atk
	packet.addShort(equip->imatk); // M.Atk
	packet.addShort(equip->iwdef); // W.def
	packet.addShort(equip->imdef); // M.Def
	packet.addShort(equip->iacc); // Acc		
	packet.addShort(equip->iavo); // Avo		
	packet.addShort(equip->ihand); // Hands		
	packet.addShort(equip->ispeed); // Speed		
	packet.addShort(equip->ijump); // Jump		
	packet.addShort(0);		
	packet.addShort(0);		
	packet.addShort(0);		
	packet.addShort(0);		
	packet.addShort(0);		
	packet.addShort(0);
}