/*
Copyright (C) 2008-2012 Vana Development Team

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

Player::Player()
{
	initialize();
}

Player::Player(int32_t id) :
	m_id(id)
{
	initialize();
}

void Player::initialize() {
	m_online = false;
	m_cashShop = false;
	m_level = 0;
	m_channel = -1;
	m_job = -1;
	m_map = Maps::NoMap;
	m_ip = 0;
	m_onlineTime = 0;
	m_inviteTime = 0;
	m_party = nullptr;
}