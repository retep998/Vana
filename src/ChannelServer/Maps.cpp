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
#include "ChannelServer.hpp"
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

auto Maps::getMap(map_id_t mapId) -> Map * {
	return ChannelServer::getInstance().getMap(mapId);
}

auto Maps::unloadMap(map_id_t mapId) -> void {
	ChannelServer::getInstance().unloadMap(mapId);
}

auto Maps::usePortal(Player *player, const PortalInfo * const portal) -> void {
	if (portal->disabled) {
		player->send(MapPacket::portalBlocked());
		player->send(PlayerPacket::showMessage("The portal is closed for now.", PlayerPacket::NoticeTypes::Red));
		return;
	}

	if (portal->script.size() != 0) {
		// Check for "onlyOnce" portal
		if (portal->onlyOnce && player->usedPortal(portal->id)) {
			player->send(MapPacket::portalBlocked());
			return;
		}

		string_t filename = ChannelServer::getInstance().getScriptDataProvider().buildScriptPath(ScriptTypes::Portal, portal->script);

		if (FileUtilities::fileExists(filename)) {
			LuaPortal luaEnv = {filename, player->getId(), player->getMapId(), portal};

			if (!luaEnv.playerMapChanged()) {
				player->send(MapPacket::portalBlocked());
			}
			if (portal->onlyOnce && !luaEnv.portalFailed()) {
				player->addUsedPortal(portal->id);
			}
		}
		else {
			string_t message = player->isGm() ?
				"Portal '" + portal->script + "' is currently unavailable." :
				"This portal is currently unavailable.";

			player->send(PlayerPacket::showMessage(message, PlayerPacket::NoticeTypes::Red));
			player->send(MapPacket::portalBlocked());
		}
	}
	else {
		// Normal portal
		Map *toMap = getMap(portal->toMap);
		if (toMap == nullptr) {
			player->send(PlayerPacket::showMessage("Bzzt. The map you're attempting to travel to doesn't exist.", PlayerPacket::NoticeTypes::Red));
			player->send(MapPacket::portalBlocked());
			return;
		}
		const PortalInfo * const nextPortal = toMap->getPortal(portal->toName);
		player->setMap(portal->toMap, nextPortal);
	}
}

auto Maps::usePortal(Player *player, PacketReader &reader) -> void {
	reader.skip<portal_count_t>();

	int32_t opcode = reader.get<int32_t>();
	switch (opcode) {
		case 0: // Dead
			if (player->getStats()->isDead()) {
				reader.unk<string_t>(); // Seemingly useless
				reader.unk<int8_t>(); // Seemingly useless
				bool wheel = reader.get<bool>();
				if (wheel) {
					if (player->getInventory()->getItemAmount(Items::WheelOfDestiny) <= 0) {
						player->acceptDeath(false);
						return;
					}
					Inventory::takeItem(player, Items::WheelOfDestiny, 1);
				}
				player->acceptDeath(wheel);
			}
			break;
		case -1: {
			string_t portalName = reader.get<string_t>();

			Map *toMap = player->getMap();
			if (toMap == nullptr) {
				return;
			}
			const PortalInfo * const portal = toMap->getPortal(portalName);
			if (portal == nullptr) {
				return;
			}
			usePortal(player, portal);
			break;
		}
		default: {
			// GM Map change (command "/m")
			if (player->isGm() && getMap(opcode) != nullptr) {
				player->setMap(opcode);
			}
		}
	}
}

auto Maps::useScriptedPortal(Player *player, PacketReader &reader) -> void {
	reader.skip<portal_count_t>();
	string_t portalName = reader.get<string_t>();

	const PortalInfo * const portal = player->getMap()->getPortal(portalName);
	if (portal == nullptr) {
		return;
	}
	usePortal(player, portal);
}

auto Maps::addPlayer(Player *player, map_id_t mapId) -> void {
	getMap(mapId)->addPlayer(player);
	getMap(mapId)->showObjects(player);
	PetHandler::showPets(player);
	SummonHandler::showSummon(player);
	// Bug in global - would be fixed here:
	// Berserk doesn't display properly when switching maps with it activated - client displays, but no message is sent to any client
	// player->getActiveBuffs()->checkBerserk(true) would override the default of only displaying changes
}