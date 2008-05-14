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

class WorldServerAcceptPlayer;

namespace WorldServerAcceptPlayerPacket {
	void connect(WorldServerAcceptPlayer *player, int channel, short port);
	void playerChangeChannel(WorldServerAcceptPlayer *player, int playerid, char *ip, short port);
	void sendToChannels(unsigned char *data, int len);
	void findPlayer(WorldServerAcceptPlayer *player, int finder, int channel, char *findee, unsigned char is = 0);
	void whisperPlayer(WorldServerAcceptPlayer *player, int whisperee, char *whisperer, int channel, char *message);
	void scrollingHeader(char *message);
};

#endif