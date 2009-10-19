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
#ifndef BBSPACKET_H
#define BBSPACKET_H

#include "Types.h"
#include <string>

using std::string;

class BbsThread;
class Guild;

namespace BbsPacket {
	void sendThreadList(uint16_t channel, Guild *guild, int32_t playerid, int16_t page = 0);
	void sendThreadData(uint16_t channel, BbsThread *thread, int32_t playerid);
};

#endif
