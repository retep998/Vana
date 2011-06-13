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

#include "ItemDataObjects.h"
#include "Types.h"
#include <boost/tr1/unordered_map.hpp>
#include <boost/utility.hpp>

using std::tr1::unordered_map;

class DropDataProvider : boost::noncopyable {
public:
	static DropDataProvider * Instance() {
		if (singleton == nullptr)
			singleton = new DropDataProvider();
		return singleton;
	}
	void loadData();

	bool hasDrops(int32_t objectId) { return (m_dropInfo.find(objectId) != m_dropInfo.end()); }
	DropsInfo getDrops(int32_t objectId) { return m_dropInfo[objectId]; }
	GlobalDrops * getGlobalDrops() { return (m_globalDrops.size() > 0 ? &m_globalDrops : nullptr); }
private:
	DropDataProvider() {}
	static DropDataProvider *singleton;

	void loadDrops();
	void loadGlobalDrops();

	unordered_map<int32_t, DropsInfo> m_dropInfo;
	GlobalDrops m_globalDrops;
};