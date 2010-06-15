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
#pragma once

#include "Types.h"
#include <string>

using std::string;

class Player;

namespace EffectPacket {
	void playMusic(int32_t mapId, const string &music);
	void playMusic(Player *player, const string &music);
	void sendEvent(int32_t mapId, const string &id);
	void sendEffect(int32_t mapId, const string &effect);
	void playPortalSoundEffect(Player *player);
	void sendFieldSound(int32_t mapId, const string &sound);
	void sendFieldSound(Player *player, const string &sound);
	void sendMinigameSound(int32_t mapId, const string &sound);
	void sendMinigameSound(Player *player, const string &sound);
	void sendMobItemBuffEffect(Player *player, int32_t itemid);
}