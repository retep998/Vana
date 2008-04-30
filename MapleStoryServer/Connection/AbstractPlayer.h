#ifndef ABSTRACTPLAYER_H
#define ABSTRACTPLAYER_H

class PacketHandler;
class AbstractPlayer {
public:
	virtual void setPacketHandler (PacketHandler* packetHandler) {
		this->packetHandler = packetHandler;
	}
	virtual void handleRequest (unsigned char* buf, int len) = 0;
	virtual ~AbstractPlayer(){}
protected:
	PacketHandler* packetHandler;
};

class AbstractPlayerFactory {
public:
	virtual AbstractPlayer* createPlayer () = 0;
};
#endif