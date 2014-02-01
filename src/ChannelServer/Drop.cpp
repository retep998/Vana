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
#include "Drop.hpp"
#include "DropsPacket.hpp"
#include "Maps.hpp"
#include "Pet.hpp"
#include "Player.hpp"
#include "PlayerDataProvider.hpp"
#include "TimeUtilities.hpp"

Drop::Drop(int32_t mapId, int32_t mesos, const Pos &pos, int32_t owner, bool playerDrop) :
	m_owner(owner),
	m_mapId(mapId),
	m_mesos(mesos),
	m_playerDrop(playerDrop),
	m_pos(pos)
{
}

Drop::Drop(int32_t mapId, const Item &item, const Pos &pos, int32_t owner, bool playerDrop) :
	m_owner(owner),
	m_mapId(mapId),
	m_playerDrop(playerDrop),
	m_pos(pos),
	m_item(item)
{
}

auto Drop::getObjectId() -> int32_t {
	return m_mesos > 0 ? m_mesos : m_item.getId();
}

auto Drop::getAmount() -> int16_t {
	return m_item.getAmount();
}

auto Drop::doDrop(const Pos &origin) -> void {
	setDroppedAtTime(TimeUtilities::getNow());
	Map *map = getMap();
	map->addDrop(this);

	if (!isQuest()) {
		if (!isTradeable()) {
			map->send(DropsPacket::showDrop(this, DropsPacket::DropTypes::DisappearDuringDrop, origin));
			this->removeDrop(false);
		}
		else {
			map->send(DropsPacket::showDrop(this, DropsPacket::DropTypes::DropAnimation, origin));
			map->send(DropsPacket::showDrop(this, DropsPacket::DropTypes::ShowDrop, origin));
		}
	}
	else if (Player *player = PlayerDataProvider::getInstance().getPlayer(m_playerId)) {
		if (player->getMapId() == m_mapId) {
			player->send(DropsPacket::showDrop(this, DropsPacket::DropTypes::DropAnimation, origin));
			player->send(DropsPacket::showDrop(this, DropsPacket::DropTypes::ShowDrop, origin));
		}
	}
}

auto Drop::showDrop(Player *player) -> void {
	if (isQuest() && player->getId() != m_playerId) {
		return;
	}
	player->send(DropsPacket::showDrop(this, DropsPacket::DropTypes::ShowExisting, Pos()));
}

auto Drop::takeDrop(Player *player, int64_t petId) -> void {
	Map *map = getMap();
	map->removeDrop(m_id);

	if (petId == 0) {
		auto &packet = DropsPacket::takeDrop(player->getId(), getId());
		if (isQuest()) {
			map->send(packet);
		}
		else {
			player->send(packet);
		}
	}
	else {
		Pet *pet = player->getPets()->getPet(petId);
		if (pet == nullptr || !pet->isSummoned()) {
			// nullptr = definitely hacking. Otherwise may be lag.
			return;
		}

		auto &packet = DropsPacket::takeDrop(player->getId(), getId(), pet->getIndex().get());
		if (isQuest()) {
			map->send(packet);
		}
		else {
			player->send(packet);
		}

	}
	delete this;
}

auto Drop::removeDrop(bool showPacket) -> void {
	Map *map = getMap();
	map->removeDrop(m_id);
	if (showPacket) {
		map->send(DropsPacket::removeDrop(getId()));
	}
	delete this;
}

auto Drop::getMap() const -> Map * {
	return Maps::getMap(m_mapId);
}