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
	namespace SyncTypes {
		enum SyncTypes {
			ChannelStart,
			Player,
			Party,
			Buddy,
			Guild,
			GuildBbs,
			Alliance,
			Data
		};
	}

	namespace Player {
		enum SyncModes {
			Connect,
			Disconnect,
			ChangeChannelRequest,
			ChangeChannelGo,
			ChangeServerRequest,
			ChangeServerGo,
			CannotChangeServer,
			DisconnectPlayer,
			PacketTransfer,
			RemovePacketTransfer,
			NewConnectable,
			UpdateLevel,
			UpdateJob,
			UpdateMap
		};
		namespace BlockMessages {
			enum Messages {
				CannotGo = 0x01,
				NoCashShop = 0x02,
				MtsUnavailable = 0x03,
				MtsUserLimit = 0x04,
				LevelTooLow = 0x05
			};
		}
	}

	namespace Party {
		enum SyncModes {
			Disband,
			Create,
			SwitchLeader,
			RemoveMember,
			AddMember
		};
	}

	namespace Alliance {
		enum Alliance {
			Create,
			Disband,
			Invite,
			InviteDenied,
			GetInfo,
			ChangeTitles,
			ChangeNotice,
			ChangeGuild,
			ChangeLeader,
			ChangeRank,
			ChangeCapacity,
			ChangeAlliance,
			ChannelConnect
		};
	}

	namespace Guild {
		enum Guild {
			Create,
			Disband,
			Invite,
			AcceptInvite,
			DenyInvite,
			ExpelOrLeave,
			ChangeTitles,
			ChangeNotice,
			ChangeRanks,
			ChangeCapacity,
			ChangePoints,
			ChangeEmblem,
			GetRankBoard,
			AddPlayer,
			ChangePlayer,
			ChangePlayerMoney,
			ChangePlayerGuildName,
			RemovePlayer,
			RemoveEmblem,
			Load,
			Unload,
			ChannelConnect
		};
	}

	namespace Buddies {
		enum Buddies {
			Invite,
			OnlineOffline
		};
	}
}
