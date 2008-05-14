/*
Copyright (C) 2008 Vana Development Team

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
#include <ctime>

Connectable * Connectable::singleton = 0;

void Connectable::newPlayer(int id) {
	map[id] = clock();
}

bool Connectable::checkPlayer(int id) {
	if (map[id]) {
		if (clock() - map[id] < 5000)
			return true;
		map.erase(id);
	}
	return false;
}
