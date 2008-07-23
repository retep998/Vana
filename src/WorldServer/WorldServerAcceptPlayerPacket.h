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

#include <string>

using std::string;

class WorldServerAcceptPlayer;

namespace WorldServerAcceptPlayerPacket {
	void connect(WorldServerAcceptPlayer *player, int channel, short port);
	void playerChangeChannel(WorldServerAcceptPlayer *player, int playerid, const string &ip, short port);
	void sendToChannels(unsigned char *data, int len);
	void findPlayer(WorldServerAcceptPlayer *player, int finder, int channel, const string &findee, unsigned char is = 0);
	void whisperPlayer(WorldServerAcceptPlayer *player, int whisperee, const string &whisperer, int channel, const string &message);
	void scrollingHeader(const string &message);
	void newConnectable(int channel, int playerid);
	void groupChat(WorldServerAcceptPlayer *player, int playerid, char type, const string &message, const string &sender);
	void sendRates(WorldServerAcceptPlayer *player, int setBit);
};

#endif
