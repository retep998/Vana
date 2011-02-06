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

#include "Types.h"
#include <boost/tr1/unordered_map.hpp>
#include <boost/utility.hpp>

using std::tr1::unordered_map;

class ChannelChangeRequests : boost::noncopyable {
public:
	static ChannelChangeRequests * Instance() {
		if (singleton == 0)
			singleton = new ChannelChangeRequests;
		return singleton;
	}

	void addPendingPlayer(int32_t id, uint16_t channelid);
	void removePendingPlayer(int32_t id);
	void removePendingPlayerEarly(int32_t id);
	uint16_t getPendingPlayerChannel(int32_t id);
private:
	ChannelChangeRequests() {};
	static ChannelChangeRequests *singleton;

	unordered_map<int32_t, uint16_t> m_map;
};
