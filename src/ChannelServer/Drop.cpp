/*
Copyright (C) 2008-2015 Vana Development Team

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
#include "Drop.hpp"
#include "ChannelServer.hpp"
#include "DropsPacket.hpp"
#include "Maps.hpp"
#include "Pet.hpp"
#include "Player.hpp"
#include "PlayerDataProvider.hpp"
#include "TimeUtilities.hpp"

namespace Vana {

Drop::Drop(map_id_t mapId, mesos_t mesos, const Point &pos, player_id_t owner, bool playerDrop) :
	m_owner{owner},
	m_mapId{mapId},
	m_mesos{mesos},
	m_playerDrop{playerDrop},
	m_pos{pos}
{
}

Drop::Drop(map_id_t mapId, const Item &item, const Point &pos, player_id_t owner, bool playerDrop) :
	m_owner{owner},
	m_mapId{mapId},
	m_playerDrop{playerDrop},
	m_pos{pos},
	m_item{item}
{
}

auto Drop::getObjectId() -> int32_t {
	return m_mesos > 0 ? m_mesos : m_item.getId();
}

auto Drop::getAmount() -> slot_qty_t {
	return m_item.getAmount();
}

auto Drop::doDrop(const Point &origin) -> void {
	setDroppedAtTime(TimeUtilities::getNow());
	Map *map = getMap();
	map->addDrop(this);

	if (!isQuest()) {
		if (!isTradeable()) {
			map->send(Packets::Drops::showDrop(this, Packets::Drops::DropTypes::DisappearDuringDrop, origin));
			this->removeDrop(false);
		}
		else {
			map->send(Packets::Drops::showDrop(this, Packets::Drops::DropTypes::DropAnimation, origin));
			map->send(Packets::Drops::showDrop(this, Packets::Drops::DropTypes::ShowDrop, origin));
		}
	}
	else if (m_owner != 0) {
		if (Player *player = ChannelServer::getInstance().getPlayerDataProvider().getPlayer(m_owner)) {
			if (player->getMapId() == m_mapId) {
				player->send(Packets::Drops::showDrop(this, Packets::Drops::DropTypes::DropAnimation, origin));
				player->send(Packets::Drops::showDrop(this, Packets::Drops::DropTypes::ShowDrop, origin));
			}
		}
	}
}

auto Drop::showDrop(Player *player) -> void {
	if (isQuest() && player->getId() != m_owner) {
		return;
	}
	player->send(Packets::Drops::showDrop(this, Packets::Drops::DropTypes::ShowExisting, Point{}));
}

auto Drop::takeDrop(Player *player, pet_id_t petId) -> void {
	Map *map = getMap();
	map->removeDrop(m_id);

	if (petId == 0) {
		auto &packet = Packets::Drops::takeDrop(player->getId(), getId());
		if (isQuest()) {
			player->send(packet);
		}
		else {
			map->send(packet);
		}
	}
	else {
		Pet *pet = player->getPets()->getPet(petId);
		if (pet == nullptr || !pet->isSummoned()) {
			// nullptr = definitely hacking. Otherwise may be lag.
			return;
		}

		auto &packet = Packets::Drops::takeDrop(player->getId(), getId(), pet->getIndex().get());
		if (isQuest()) {
			player->send(packet);
		}
		else {
			map->send(packet);
		}

	}
	delete this;
}

auto Drop::removeDrop(bool showPacket) -> void {
	Map *map = getMap();
	map->removeDrop(m_id);
	if (showPacket) {
		map->send(Packets::Drops::removeDrop(getId()));
	}
	delete this;
}

auto Drop::getMap() const -> Map * {
	return Maps::getMap(m_mapId);
}

}