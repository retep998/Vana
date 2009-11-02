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
#ifndef INTERHELPER_H
#define INTERHELPER_H

// Specific packet constants that are shared between servers
namespace PartyActions {
	enum PartyActions {
		Create = 0x01,
		Leave = 0x02,
		Join = 0x03,
		Invite = 0x04,
		Expel = 0x05,
		SetLeader = 0x06,
		SilentUpdate,
		LogInOrOut
	};
}

namespace Sync {
	enum Sync {
		ChannelStart = 0x7F
	};
	namespace Party {
		enum PartySync {
			Disband,
			Create,
			SwitchLeader,
			RemoveMember,
			AddMember
		};
	}
}

#endif