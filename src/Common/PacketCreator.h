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
#ifndef PACKETCREATOR_H
#define PACKETCREATOR_H

#define MAX_LEN 10000

#include <Vector>

using namespace std;

class Packet {
public:
	Packet(){
		pos = 0;
	}

	void addHeader(short headerid);
	void addInt(int intg);
	void addInt64(__int64 int64);
	void addShort(short shrt);
	void addString(const char *str, int slen);
	void addByte(unsigned char byte);
	void addBytes(char* hex);
	void addBuffer(unsigned char* bytes, int len);

	template <class T>
	void sendTo(T* player, vector <T*> players, bool is) {
		for(unsigned int i=0; i<players.size(); i++){
			if((player != NULL && player->getPlayerid() != players[i]->getPlayerid() && !is) || is)
				this->send(players[i]);
		}
	}
	
	template <class T>
	void send(T* player) {
		unsigned char tempbuf[MAX_LEN];
		memcpy_s(tempbuf, MAX_LEN, packet, MAX_LEN); // Copying to tempbuf so the packet doesn't get emptied on send and can be sent to other players
		player->sendPacket(tempbuf, pos);
	}
private:
	int pos;
	unsigned char packet[MAX_LEN];
};

#endif