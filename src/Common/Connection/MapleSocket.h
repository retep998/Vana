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
#ifndef MAPLESOCKET_H
#define MAPLESOCKET_H

#define HEADER_LEN 4
#define BUFFER_LEN 10000

#include "../Decoder/Decoder.h"
#include <TcpSocket.h>
#include <ISocketHandler.h>

class AbstractPlayerFactory;
class AbstractPlayer;

class MapleSocket : public TcpSocket {
public:
	MapleSocket(ISocketHandler &h);

	Socket * Create();
	void OnAccept();
	void OnConnect();
	void OnRead();
	void OnDelete();
	void SendEncrypted(unsigned char *buf, short len);

	AbstractPlayer * GetPlayer() const { return player; }
	bool GetReady() const { return ready; }
	void SetAbstractPlayerFactory(AbstractPlayerFactory *apf) { abstractPlayerFactory = apf; }
	void SetPlayer(AbstractPlayer *player) { this->player = player; }
	void SetPlayer();
private:
	AbstractPlayerFactory *abstractPlayerFactory;
	AbstractPlayer *player;
	Decoder *decoder;
	unsigned char *buffer;
	int bytesInBuffer;
	bool ready;
};

#endif