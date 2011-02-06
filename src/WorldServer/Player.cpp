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
#include "Player.h"
#include "MapConstants.h"

Player::Player() {
	initialize();
}

Player::Player(int32_t id) : id(id) {
	initialize();
}

void Player::initialize() {
	online = false;
	cashShop = false;
	guildRank = 0;
	allianceRank = 0;
	level = 0;
	channel = -1;
	job = -1;
	map = Maps::NoMap;
	inviteGuild = 0;
	ip = 0;
	onlineTime = 0;
	inviteTime = 0;
	party = 0;
	guild = 0;
	alliance = 0;
}