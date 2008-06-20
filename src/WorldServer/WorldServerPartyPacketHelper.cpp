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

#include "WorldServerPartyPacketHelper.h"
#include "Players.h"
#include "PacketCreator.h"
#include "WorldServerPartyHandler.h"

void PartyPacketHelper::addParty(Packet &packet, Party *party) {

	for(unsigned int i=0; i<6; i++) {
		if(i<party->members.size())
			packet.addInt(party->members[i]->id);
		else
			packet.addInt(0);
	}

	for(unsigned int i=0; i<6; i++) {
		if(i<party->members.size())
			packet.addString(party->members[i]->name, 13);
		else
			packet.addString("", 13);
	}

	for(unsigned int i=0; i<6; i++) {
		if(i<party->members.size())
			packet.addInt(party->members[i]->job);
		else
			packet.addInt(0);
	}
	
	for(unsigned int i=0; i<6; i++) {
		if(i<party->members.size())
			packet.addInt(party->members[i]->level);
		else
			packet.addInt(0);
	}

	for(unsigned int i=0; i<6; i++) {
		if(i < party->members.size())
			packet.addInt(party->members[i]->channel);
		else
			packet.addInt(4294967295);
	}

	packet.addInt(party->getLeader());
	
	for(unsigned int i=0; i<6; i++) {
		if(i < party->members.size())
			packet.addInt(party->members[i]->map);
		else
			packet.addInt(0);
	}

	for(unsigned int i=0; i<6; i++) {
		if(i < party->members.size()) {
			packet.addInt(999999999); //some portal shit
			packet.addInt(999999999); //some portal shit
			packet.addInt(4294967295); //some portal shit
			packet.addInt(4294967295); //some portal shit
		}
		else {
			packet.addInt(999999999); //some portal shit
			packet.addInt(999999999); //some portal shit
			packet.addInt(4294967295); //some portal shit
			packet.addInt(4294967295); //some portal shit
		}
	}
}
