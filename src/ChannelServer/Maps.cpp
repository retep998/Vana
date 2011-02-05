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
#include "Maps.h"
#include "FileUtilities.h"
#include "Instance.h"
#include "Inventory.h"
#include "LuaPortal.h"
#include "MapDataProvider.h"
#include "MapObjects.h"
#include "MapPacket.h"
#include "PacketReader.h"
#include "PetHandler.h"
#include "Player.h"
#include "PlayerDataProvider.h"
#include "PlayerPacket.h"
#include "Summons.h"
#include <string>

using std::string;

Map * Maps::getMap(int32_t mapid) {
	return MapDataProvider::Instance()->getMap(mapid);
}

void Maps::usePortal(Player *player, PortalInfo *portal) {
	if (portal->script.size() != 0) { // Scripted portal
		// Check for "onlyOnce" portal
		if (portal->onlyOnce) {
			if (player->usedPortal(portal->id)) {
				MapPacket::portalBlocked(player);
				return;
			}
			else {
				player->addUsedPortal(portal->id);
			}
		}

		string filename = "scripts/portals/" + portal->script + ".lua";

		if (FileUtilities::fileExists(filename)) { // Lua Portal script exists
			int32_t map = player->getMap();
			LuaPortal(filename, player->getId(), portal);

			if (map == player->getMap()) {
				// The portal didn't change the map
				MapPacket::portalBlocked(player);
			}
		}
		else {
			string message;
			if (player->isGm()) {
				message = "This portal '" + portal->script + "' is currently unavailable.";
			}
			else {
				message = "This portal is currently unavailable.";
			}
			PlayerPacket::showMessage(player, message, PlayerPacket::NoticeTypes::Red);
			MapPacket::portalBlocked(player);
		}
	}
	else {
		// Normal portal
		Map *tomap = getMap(portal->toMap);
		if (tomap == nullptr) {
			string message = "Bzzt. The map you're attempting to travel to doesn't exist.";
			PlayerPacket::showMessage(player, message, PlayerPacket::NoticeTypes::Red);
			MapPacket::portalBlocked(player);
			return;
		}
		PortalInfo *nextportal = tomap->getPortal(portal->toName);
		player->setMap(portal->toMap, nextportal);
	}
}

void Maps::usePortal(Player *player, PacketReader &packet) {
	packet.skipBytes(1);

	int32_t opcode = packet.get<int32_t>();
	switch (opcode) {
		case 0: // Dead
			if (player->getStats()->getHp() == 0) {
				packet.getString(); // Useless
				packet.skipBytes(1); // Useless
				bool wheel = packet.getBool();
				if (wheel && player->getInventory()->getItemAmount(Items::WheelOfDestiny) <= 0) {
					player->acceptDeath(false);
					return;
				}
				Inventory::takeItem(player, Items::WheelOfDestiny, 1);
				player->acceptDeath(wheel);
			}
			break;
		case -1: {
			string portalname = packet.getString();

			Map *tomap = getMap(player->getMap());
			if (tomap == nullptr)
				return;
			PortalInfo *portal = tomap->getPortal(portalname);
			if (portal == nullptr) // Exit the function if portal is not found
				return;

			usePortal(player, portal);
			break;
		}
		default: { // GM Map change (command "/m")
			if (player->isGm() && getMap(opcode)) {
				player->setMap(opcode);
			}
		}
	}
}

void Maps::useScriptedPortal(Player *player, PacketReader &packet) {
	packet.skipBytes(1);
	string portalname = packet.getString();

	PortalInfo *portal = getMap(player->getMap())->getPortal(portalname);
	if (portal == nullptr) // Exit the function if portal is not found
		return;

	usePortal(player, portal);
}

void Maps::addPlayer(Player *player, int32_t mapid) {
	PlayerDataProvider::Instance()->addPlayer(player);
	getMap(mapid)->addPlayer(player);
	getMap(mapid)->showObjects(player);
	PetHandler::showPets(player);
	Summons::showSummon(player);
	// Bug in global - would be fixed here:
	// Berserk doesn't display properly when switching maps with it activated - client displays, but no message is sent to any client
	// player->getActiveBuffs()->checkBerserk(true) would override the default of only displaying changes
}