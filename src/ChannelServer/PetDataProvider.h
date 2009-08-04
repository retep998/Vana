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
#ifndef PETDATA_H
#define PETDATA_H

#include "Types.h"
#include <boost/tr1/unordered_map.hpp>
#include <boost/utility.hpp>
#include <string>

using std::string;
using std::tr1::unordered_map;
	
struct PetInfo {
	string name;
	int32_t hunger;
};

struct PetInteractInfo {
	uint32_t prob;
	int16_t increase;
};

class PetDataProvider : boost::noncopyable {
public:
	static PetDataProvider * Instance() {
		if (singleton == 0)
			singleton = new PetDataProvider();
		return singleton;
	}
	void loadData();

	string getName(int32_t itemid) { return (petsInfo.find(itemid) != petsInfo.end() ? petsInfo[itemid].name : ""); }
	int32_t getHunger(int32_t itemid) {return (petsInfo.find(itemid) != petsInfo.end() ? petsInfo[itemid].hunger : 0); }
	PetInteractInfo * getInteraction(int32_t itemid, int32_t action); 
private:
	PetDataProvider() {}
	static PetDataProvider *singleton;

	unordered_map<int32_t, PetInfo> petsInfo;
	unordered_map<int32_t, unordered_map<int32_t, PetInteractInfo> > petsInteractInfo;
};

#endif
