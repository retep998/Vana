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
#include "Server.h"
#include "ServerPacket.h"
#include "Player.h"

char Server::msg[100];
int Server::on;

void Server::showScrollingHeader(Player* player){
	if(on)
		ServerPacket::showScrollingHeader(player, msg);

}
void Server::changeScrollingHeader(char* msg){
	strcpy_s(Server::msg, 100, msg);
	ServerPacket::changeScrollingHeader(Server::msg);
	
}
void Server::enableScrollingHeader(){
	on=1;
	ServerPacket::changeScrollingHeader(msg);

}
void Server::disableScrollingHeader(){
	on=0;
	ServerPacket::scrollingHeaderOff();
}
