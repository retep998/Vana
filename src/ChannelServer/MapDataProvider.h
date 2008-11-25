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
#ifndef MAPDATAPROVIDER_H
#define MAPDATAPROVIDER_H

#include "Types.h"
#include <tr1/unordered_map>
#include <boost/thread/mutex.hpp>

using std::tr1::unordered_map;

class Map;

class MapDataProvider {
public:
	static MapDataProvider * Instance() {
		if (singleton == 0)
			singleton = new MapDataProvider();
		return singleton;
	}
	Map * getMap(int32_t mapid);

private:
	MapDataProvider() {}
	MapDataProvider(const MapDataProvider&);
	MapDataProvider& operator=(const MapDataProvider&);
	static MapDataProvider *singleton;
	unordered_map<int32_t, Map *> maps;
	boost::mutex loadmap_mutex;

	void loadMap(int32_t mapid, Map *&map);
};

#endif
