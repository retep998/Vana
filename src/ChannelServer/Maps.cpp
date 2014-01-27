/*
Copyright (C) 2008-2014 Vana Development Team

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
#include "Maps.hpp"
#include "FileUtilities.hpp"
#include "Instance.hpp"
#include "Inventory.hpp"
#include "LuaPortal.hpp"
#include "MapDataProvider.hpp"
#include "MapObjects.hpp"
#include "MapPacket.hpp"
#include "PetHandler.hpp"
#include "Player.hpp"
#include "PlayerPacket.hpp"
#include "PlayerDataProvider.hpp"
#include "PacketReader.hpp"
#include "SummonHandler.hpp"
#include <string>

auto Maps::getMap(int32_t mapId) -> Map * {
	return MapDataProvider::getInstance().getMap(mapId);
}

auto Maps::unloadMap(int32_t mapId) -> void {
	MapDataProvider::getInstance().unloadMap(mapId);
}

auto Maps::usePortal(Player *player, PortalInfo *portal) -> void {
	if (portal->script.size() != 0) {
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

		string_t filename = "scripts/portals/" + portal->script + ".lua";

		if (FileUtilities::fileExists(filename)) {
			int32_t map = player->getMapId();
			LuaPortal(filename, player->getId(), portal);

			if (map == player->getMapId()) {
				MapPacket::portalBlocked(player);
			}
		}
		else {
			string_t message;
			if (player->isGm()) {
				message = "Portal '" + portal->script + "' is currently unavailable.";
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
		Map *toMap = getMap(portal->toMap);
		if (toMap == nullptr) {
			PlayerPacket::showMessage(player, "Bzzt. The map you're attempting to travel to doesn't exist.", PlayerPacket::NoticeTypes::Red);
			MapPacket::portalBlocked(player);
			return;
		}
		PortalInfo *nextPortal = toMap->getPortal(portal->toName);
		player->setMap(portal->toMap, nextPortal);
	}
}

auto Maps::usePortal(Player *player, PacketReader &packet) -> void {
	packet.skipBytes(1);

	int32_t opcode = packet.get<int32_t>();
	switch (opcode) {
		case 0: // Dead
			if (player->getStats()->isDead()) {
				const string_t &unk = packet.getString(); // Useless
				packet.skipBytes(1); // Useless
				bool wheel = packet.get<bool>();
				if (wheel && player->getInventory()->getItemAmount(Items::WheelOfDestiny) <= 0) {
					player->acceptDeath(false);
					return;
				}
				Inventory::takeItem(player, Items::WheelOfDestiny, 1);
				player->acceptDeath(wheel);
			}
			break;
		case -1: {
			const string_t &portalName = packet.getString();

			Map *toMap = player->getMap();
			if (toMap == nullptr) {
				return;
			}
			PortalInfo *portal = toMap->getPortal(portalName);
			if (portal == nullptr) {
				return;
			}
			usePortal(player, portal);
			break;
		}
		default: {
			// GM Map change (command "/m")
			if (player->isGm() && getMap(opcode)) {
				player->setMap(opcode);
			}
		}
	}
}

auto Maps::useScriptedPortal(Player *player, PacketReader &packet) -> void {
	packet.skipBytes(1);
	const string_t &portalName = packet.getString();

	PortalInfo *portal = player->getMap()->getPortal(portalName);
	if (portal == nullptr) {
		return;
	}
	usePortal(player, portal);
}

auto Maps::addPlayer(Player *player, int32_t mapId) -> void {
	PlayerDataProvider::getInstance().addPlayer(player);
	getMap(mapId)->addPlayer(player);
	getMap(mapId)->showObjects(player);
	PetHandler::showPets(player);
	SummonHandler::showSummon(player);
	// Bug in global - would be fixed here:
	// Berserk doesn't display properly when switching maps with it activated - client displays, but no message is sent to any client
	// player->getActiveBuffs()->checkBerserk(true) would override the default of only displaying changes
}