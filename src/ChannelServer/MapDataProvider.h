/*
Copyright (C) 2008-2009 Vana Development Team

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
#include <boost/bimap.hpp>
#include <boost/bimap/unordered_multiset_of.hpp>
#include <boost/tr1/unordered_map.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/utility.hpp>

using std::tr1::unordered_map;

class Map;

class MapDataProvider : boost::noncopyable {
public:
	static MapDataProvider * Instance() {
		if (singleton == 0)
			singleton = new MapDataProvider();
		return singleton;
	}
	Map * getMap(int32_t mapid);

private:
	MapDataProvider();
	static MapDataProvider *singleton;
	typedef boost::bimap<int8_t, boost::bimaps::unordered_multiset_of<int8_t> > continent_map;
	typedef continent_map::value_type continent_info;

	unordered_map<int32_t, Map *> maps;
	continent_map continents;
	boost::mutex loadmap_mutex;
	
	void loadMap(int32_t mapid, Map *&map);
	int8_t getContinent(int32_t mapid);
};

#endif
