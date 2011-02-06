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
#include "Parties.h"
#include "PartyHandler.h"

Parties * Parties::singleton = 0;

int32_t Parties::addParty(Party *party) {
	m_map[++pid] = party;
	return pid;
}

void Parties::removeParty(int32_t id) {
	if (m_map.find(id) != m_map.end()) {
		Party *party = m_map[id];
		delete party;
		m_map.erase(id);
	}
}

Party * Parties::getParty(int32_t id) {
	return (m_map.find(id) != m_map.end() ? m_map[id] : 0);
}

unordered_map<int32_t, Party *> Parties::getParties() {
	return m_map;
}