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
#include "Connectable.h"
#include "TimeUtilities.h"
#include <ctime>

using TimeUtilities::getTickCount;

Connectable * Connectable::singleton = nullptr;

void Connectable::newPlayer(int32_t id, ip_t ip) {
	ConnectingPlayer player;
	player.connectIp = ip;
	player.connectTime = getTickCount();
	m_map[id] = player;
}

bool Connectable::checkPlayer(int32_t id, ip_t ip) {
	bool correct = false;
	if (m_map.find(id) != m_map.end()) {
		ConnectingPlayer &t = m_map[id];
		if (t.connectIp == ip && (getTickCount() - t.connectTime) < MaxMilliseconds) {
			correct = true;
		}
		m_map.erase(id);
	}
	return correct;
}