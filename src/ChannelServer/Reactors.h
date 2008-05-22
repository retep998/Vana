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
#ifndef REACTOR_H
#define REACTOR_H

#include <hash_map>
#include <vector>
#include <string>

using namespace std;
using namespace stdext;

class Player;
class Packet;

struct ReactorInfo {
	int id;
	short x;
	short y;
	short time;
	unsigned char f;
};

typedef vector<ReactorInfo> ReactorsInfo;

namespace Reactors {
	extern hash_map <int, ReactorsInfo> info;
	void addReactor(int id, ReactorsInfo reactors);
	void showReactors(Player* player);
};

#endif
