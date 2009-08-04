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
#ifndef REACTORDATA_H
#define REACTORDATA_H

#include "Pos.h"
#include "Types.h"
#include <boost/tr1/unordered_map.hpp>
#include <boost/utility.hpp>
#include <vector>

using std::tr1::unordered_map;
using std::vector;

struct ReactorEventInfo {
	bool repeat;
	int8_t nextstate;
	int16_t type;
	int32_t itemid;
	int32_t timeout;
	Pos lt;
	Pos rb;
};

class ReactorDataProvider : boost::noncopyable {
public:
	static ReactorDataProvider * Instance() {
		if (singleton == 0)
			singleton = new ReactorDataProvider();
		return singleton;
	}
	void loadData();

	int8_t getMaxState(int32_t reactorid) { return (maxstates.find(reactorid) != maxstates.end() ? maxstates[reactorid] : 0); }
	int8_t getEventCount(int32_t reactorid, int8_t state);
	ReactorEventInfo * getEvent(int32_t reactorid, int8_t state, int8_t specific = 0);
private:
	ReactorDataProvider() {}
	static ReactorDataProvider *singleton;

	unordered_map<int32_t, unordered_map<int8_t,  vector<ReactorEventInfo> > > reactorinfo;
	unordered_map<int32_t, int8_t> maxstates;
};

#endif
