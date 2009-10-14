/*
Copyright (C) 2009 Vana Development Team

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
#include "Alliances.h"
#include "Alliance.h"
#include "Database.h"
#include "PacketCreator.h"

Alliances * Alliances::singleton = 0;

Alliance * Alliances::getAlliance(int32_t id) {
	return (m_alliances.find(id) == m_alliances.end() ? 0 : m_alliances[id]);
}

void Alliances::removeAlliance(int32_t id) {
	delete m_alliances[id];
	m_alliances.erase(id);
}

void Alliances::addAlliance(int32_t id, string name, string notice, string title1, string title2, string title3, std::string title4, std::string title5, int32_t capacity, int32_t leader) {
	Alliance * alliance = getAlliance(id);
	if (alliance == 0) {
		alliance = new Alliance(id, name, notice, title1, title2, title3, title4, title5, capacity, leader);
		m_alliances[id] = alliance;
	}
	else {
		alliance->setCapacity(capacity);
		alliance->setLeaderId(leader);
		alliance->setNotice(notice);
		alliance->setTitle(0, title1);
		alliance->setTitle(1, title2);
		alliance->setTitle(2, title3);
		alliance->setTitle(3, title4);
		alliance->setTitle(4, title5);
	}
}

void Alliances::getChannelConnectPacket(PacketCreator &packet) {
	packet.add<int32_t>(m_alliances.size());
	for (unordered_map<int32_t, Alliance *>::iterator iter = m_alliances.begin(); iter != m_alliances.end(); iter++) {
		packet.add<int32_t>(iter->second->getId());
		packet.addString(iter->second->getName());
		packet.add<int32_t>(iter->second->getCapacity());
	}
}
