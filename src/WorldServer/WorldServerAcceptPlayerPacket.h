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
#ifndef WORLDSERVERACCEPTPLAYERPACKET_H
#define WORLDSERVERACCEPTPLAYERPACKET_H

#include "Types.h"
#include <string>

using std::string;

class WorldServerAcceptPlayer;

namespace WorldServerAcceptPlayerPacket {
	void connect(WorldServerAcceptPlayer *player, uint16_t channel, uint16_t port, unsigned char maxMultiLevel);
	void playerChangeChannel(WorldServerAcceptPlayer *player, int32_t playerid, const string &ip, int16_t port);
	void sendToChannels(unsigned char *data, int32_t len);
	void findPlayer(WorldServerAcceptPlayer *player, int32_t finder, uint16_t channel, const string &findee, unsigned char is = 0);
	void whisperPlayer(WorldServerAcceptPlayer *player, int32_t whisperee, const string &whisperer, uint16_t channel, const string &message);
	void scrollingHeader(const string &message);
	void newConnectable(uint16_t channel, int32_t playerid);
	void groupChat(WorldServerAcceptPlayer *player, int32_t playerid, char type, const string &message, const string &sender);
	void sendRates(WorldServerAcceptPlayer *player, int32_t setBit);
};

#endif
