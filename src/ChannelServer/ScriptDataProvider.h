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
#ifndef SCRIPTDATA_H
#define SCRIPTDATA_H

#include "Types.h"
#include <boost/tr1/unordered_map.hpp>
#include <boost/utility.hpp>
#include <string>

using std::string;
using std::tr1::unordered_map;

class ScriptDataProvider : boost::noncopyable {
public:
	static ScriptDataProvider * Instance() {
		if (singleton == 0)
			singleton = new ScriptDataProvider();
		return singleton;
	}
	void loadData();

	string getNpcScript(int32_t npcid);
	string getReactorScript(int32_t reactorid);
	string getQuestScript(int16_t questid, int8_t state);
private:
	ScriptDataProvider() {}
	static ScriptDataProvider *singleton;

	unordered_map<int32_t, string> npcscripts;
	unordered_map<int32_t, string> reactorscripts;
	unordered_map<int16_t, unordered_map<int8_t, string> > questscripts;
};

#endif
