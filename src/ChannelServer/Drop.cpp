/*
Copyright (C) 2008-2013 Vana Development Team

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
#include "Drop.h"
#include "DropsPacket.h"
#include "Maps.h"
#include "Pet.h"
#include "Player.h"
#include "PlayerDataProvider.h"
#include "TimeUtilities.h"
#include <limits>

Drop::Drop(int32_t mapId, int32_t mesos, const Pos &pos, int32_t owner, bool playerDrop) :
	m_questId(0),
	m_owner(owner),
	m_mapId(mapId),
	m_mesos(mesos),
	// Initializing dropped time to max-value to prevent timers from
	// deleting the drop in case doDrop did not get called right away
	m_dropped(std::numeric_limits<int32_t>::max()),
	m_playerId(0),
	m_playerDrop(playerDrop),
	m_type(Drop::Normal),
	m_tradeable(true),
	m_pos(pos)
{
	Maps::getMap(mapId)->addDrop(this);
}

Drop::Drop(int32_t mapId, const Item &item, const Pos &pos, int32_t owner, bool playerDrop) :
	m_questId(0),
	m_owner(owner),
	m_mapId(mapId),
	m_mesos(0),
	m_dropped(std::numeric_limits<int32_t>::max()),
	m_playerId(0),
	m_playerDrop(playerDrop),
	m_type(Drop::Normal),
	m_tradeable(true),
	m_pos(pos),
	m_item(item)
{
	Maps::getMap(mapId)->addDrop(this);
}

int32_t Drop::getObjectId() {
	return (m_mesos > 0 ? m_mesos : m_item.getId());
}

int16_t Drop::getAmount() {
	return m_item.getAmount();
}

void Drop::doDrop(const Pos &origin) {
	setDropped(TimeUtilities::getTickCount());
	if (!isQuest()) {
		if (!isTradeable()) {
			DropsPacket::showDrop(nullptr, this, DropsPacket::DropTypes::DisappearDuringDrop, false, origin);
			this->removeDrop(false);
		}
		else {
			DropsPacket::showDrop(nullptr, this, DropsPacket::DropTypes::DropAnimation, true, origin);
		}
	}
	else if (Player *player = PlayerDataProvider::Instance()->getPlayer(m_playerId)) {
		if (player->getMapId() == m_mapId) {
			DropsPacket::showDrop(player, this, DropsPacket::DropTypes::DropAnimation, true, origin);
		}
	}
}

void Drop::showDrop(Player *player) {
	if (isQuest() && player->getId() != m_playerId) {
		return;
	}
	DropsPacket::showDrop(player, this, DropsPacket::DropTypes::ShowExisting, false, Pos());
}

void Drop::takeDrop(Player *player, int64_t petId) {
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

void Drop::removeDrop(bool showPacket) {
	Maps::getMap(m_mapId)->removeDrop(m_id);
	if (showPacket) {
		DropsPacket::removeDrop(this);
	}
	delete this;
}