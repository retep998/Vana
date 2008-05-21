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
#include "Reactors.h"
#include "Player.h"
#include "ReactorPacket.h"
#include "InventoryPacket.h"
#include "PacketCreator.h"
#include "Maps.h"
#include "Inventory.h"
#include "BufferUtilities.h"
#include "SendHeader.h"

hash_map <int, ReactorsInfo> Reactors::info;

void Reactors::addReactor(int id, ReactorsInfo reactors){
	info[id] = reactors;
}

void Reactors::showReactors(Player *player){
	for(unsigned int i=0; i<info[player->getMap()].size(); i++)
		ReactorPacket::showReactor(player, info[player->getMap()][i], i);
}
