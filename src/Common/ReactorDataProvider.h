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
#pragma once

#include "Pos.h"
#include "Types.h"
#include <boost/tr1/unordered_map.hpp>
#include <boost/utility.hpp>
#include <vector>

using std::tr1::unordered_map;
using std::vector;

struct ReactorStateInfo {
	ReactorStateInfo() : type(0) { }
	int8_t nextState;
	int16_t type;
	int16_t itemQuantity;
	int32_t itemId;
	int32_t timeout;
	Pos lt;
	Pos rb;
	vector<int32_t> triggerSkills;
};

struct ReactorData {
	ReactorData() : removeInFieldSet(false), activateByTouch(false) { }
	bool removeInFieldSet;
	bool activateByTouch;
	int8_t maxStates;
	int32_t link;
	unordered_map<int8_t, vector<ReactorStateInfo> > states;
};

class ReactorDataProvider : boost::noncopyable {
public:
	static ReactorDataProvider * Instance() {
		if (singleton == nullptr)
			singleton = new ReactorDataProvider();
		return singleton;
	}
	void loadData();

	ReactorData * getReactorData(int32_t reactorId, bool respectLink = false);
private:
	ReactorDataProvider() {}
	static ReactorDataProvider *singleton;

	void loadReactors();
	void loadStates();
	void loadTriggerSkills();

	unordered_map<int32_t, ReactorData> m_reactorInfo;
};
