/*
Copyright (C) 2008-2011 Vana Development Team

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
#include "PetsPacket.h"
#include "GameConstants.h"
#include "MapleSession.h"
#include "PacketCreator.h"
#include "PacketReader.h"
#include "Pet.h"
#include "Player.h"
#include "SendHeader.h"

void PetsPacket::addInfo(PacketCreator &packet, Pet *pet) {
	packet.add<int8_t>(3);
	packet.add<int32_t>(pet->getItemId());
	packet.add<int8_t>(1);
	packet.add<int64_t>(pet->getCashId() == 0 ? pet->getId() : pet->getCashId());
	packet.add<int64_t>(Items::NoExpiration);
	packet.addString(pet->getName(), 13);
	packet.add<int8_t>(pet->getLevel());
	packet.add<int16_t>(pet->getCloseness());
	packet.add<int8_t>(pet->getFullness());
	packet.add<int64_t>(0LL);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0); // Time to expire (for trial pet)
}
