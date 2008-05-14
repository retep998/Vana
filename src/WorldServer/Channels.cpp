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
#include "Channels.h"

Channels * Channels::singleton = 0;

void Channels::registerChannel(WorldServerAcceptPlayer *player, int channel, char *ip, short port) {
	Channel *chan = new Channel();
	chan->player = player;
	strcpy_s(chan->ip, ip);
	chan->port = port;
	channels[channel] = chan;
}

Channel * Channels::getChannel(int num) {
	return channels[num];
}

int Channels::size() {
	return channels.size();
}