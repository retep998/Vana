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
	Maps::getMap(m_mapId)->addDrop(this);

	if (!isQuest()) {
		if (!isTradeable()) {
			DropsPacket::showDrop(nullptr, this, DropsPacket::DropTypes::DisappearDuringDrop, false, origin);
			this->removeDrop(false);
		}
		else {
			DropsPacket::showDrop(nullptr, this, DropsPacket::DropTypes::DropAnimation, true, origin);
		}
	}
	else if (Player *player = PlayerDataProvider::getInstance().getPlayer(m_playerId)) {
		if (player->getMapId() == m_mapId) {
			DropsPacket::showDrop(player, this, DropsPacket::DropTypes::DropAnimation, true, origin);
		}
	}
}

auto Drop::showDrop(Player *player) -> void {
	if (isQuest() && player->getId() != m_playerId) {
		return;
	}
	DropsPacket::showDrop(player, this, DropsPacket::DropTypes::ShowExisting, false, Pos());
}

auto Drop::takeDrop(Player *player, int64_t petId) -> void {
	Maps::getMap(m_mapId)->removeDrop(m_id);
	if (petId == 0) {
		DropsPacket::takeDrop(player, this);
	}
	else {
		Pet *pet = player->getPets()->getPet(petId);
		if (pet == nullptr || !pet->isSummoned()) {
			// nullptr = definitely hacking. Otherwise may be lag.
			return;
		}
		DropsPacket::takeDrop(player, this, pet->getIndex().get());
	}
	delete this;
}

auto Drop::removeDrop(bool showPacket) -> void {
	Maps::getMap(m_mapId)->removeDrop(m_id);
	if (showPacket) {
		DropsPacket::removeDrop(this);
	}
	delete this;
}