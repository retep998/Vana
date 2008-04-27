#include "PacketCreator.h"
#include "Player.h"
#include "Mobs.h"
#include "MobsPacket.h"
#include "BufferUtilities.h"

void MobsPacket::controlMob(Player* player, Mob* mob){
	Packet packet = Packet();
	packet.addHeader(0xa4);
	packet.addByte(1);
	packet.addInt(mob->getID());
	packet.addByte(1);
	packet.addInt(mob->getMobID());
	packet.addInt(0);
	packet.addShort(mob->getPos().x);
	packet.addShort(mob->getPos().y-1);
	packet.addByte(mob->getType());
	packet.addShort(0);
	packet.addShort(mob->getFH());
	packet.addShort(-1);
	packet.packetSend(player);
}
void MobsPacket::endControlMob(Player* player, Mob* mob){
	Packet packet = Packet();
	packet.addHeader(0xa4);
	packet.addByte(0);
	packet.addInt(mob->getID());
	packet.packetSend(player);
}

void MobsPacket::spawnMob(Player* player, Mob* mob, vector <Player*> players, bool isspawn){
	Packet packet = Packet();
	packet.addHeader(0x96);
	packet.addInt(mob->getID());
	packet.addByte(1);
	packet.addInt(mob->getMobID());
	packet.addInt(0);
	packet.addShort(mob->getPos().x);
	packet.addShort(mob->getPos().y-1);
	packet.addByte(mob->getType());
	packet.addShort(0);
	packet.addShort(mob->getFH());
	if(!isspawn)
		packet.addShort(-1);
	else
		packet.addShort(-2);
	packet.sendTo(player, players, 1);
}

void MobsPacket::showMob(Player* player, Mob* mob){
	Packet packet = Packet();
	packet.addHeader(0x96);
	packet.addInt(mob->getID());
	packet.addByte(1);
	packet.addInt(mob->getMobID());
	packet.addInt(0);
	packet.addShort(mob->getPos().x);
	packet.addShort(mob->getPos().y-1);
	packet.addByte(mob->getType());
	packet.addShort(0);
	packet.addShort(mob->getFH());
	packet.addShort(-1);
	packet.packetSend(player);
}

void MobsPacket::moveMob(Player* player, Mob* mob ,vector <Player*> players, unsigned char* pack, int pla){
	Packet packet = Packet();
	packet.addHeader(0x9C);
	packet.addInt(mob->getID());
	packet.addShort(getShort(pack+4));
	packet.addByte(0);
	packet.addInt(mob->getMP());
	packet.packetSend(player);
	packet = Packet();
	packet.addHeader(0x97);
	packet.addInt(mob->getID());
	packet.addByte(0);
	packet.addInt(0xFF);
	packet.addByte(0);
	packet.addBytesHex(pack+17, pla-17);
	packet.sendTo(player, players, 0);
}

void MobsPacket::damageMob(Player* player, vector <Player*> players, unsigned char* pack){
	int howmany = pack[1]/0x10;
	int hits = pack[1]%0x10;
	int skillid = getInt(pack+2);
	bool s4211006 = false;
	if(skillid == 4211006)
		s4211006 = true;
	Packet packet = Packet();
	packet.addHeader(0x87);
	packet.addInt(player->getPlayerid());
	packet.addByte(pack[1]);
	if(getInt(pack+2)>0){
		packet.addByte(-1);
		packet.addInt(getInt(pack+2));
	} 
	else
		packet.addByte(0);
	packet.addByte(pack[7]);
	packet.addByte(pack[9]);
	packet.addByte(10);
	packet.addInt(0);
	for(int i=0; i<howmany; i++){
		int mobid = getInt(pack+14+i*(22-s4211006+4*(hits-1)));
		packet.addInt(mobid);
		packet.addByte(-1);
		for(int j=0; j<hits; j++){
			int damage = getInt(pack+32-s4211006+i*(22-s4211006+4*(hits-1))+j*4);
			packet.addInt(damage);
		}
	}
	packet.sendTo(player, players, 0);
}

void MobsPacket::damageMobS(Player* player, vector <Player*> players, unsigned char* pack, int itemid){
	int howmany = pack[1]/0x10;
	int hits = pack[1]%0x10;
	int skillid = getInt(pack+2);
	bool s3121004 = false;
	if(skillid == 3121004 || skillid == 3221001)
		s3121004 = true;
	Packet packet = Packet();
	packet.addHeader(0x8D);
	packet.addInt(player->getPlayerid());
	packet.addByte(pack[1]);
	if(getInt(pack+2)>0){
		packet.addByte(1);
		packet.addInt(getInt(pack+2));
	} 
	else
		packet.addByte(0);
	packet.addByte(pack[7]);
	packet.addByte(pack[9]);
	packet.addByte(pack[13]);
	packet.addInt(itemid);
	for(int i=0; i<howmany; i++){
		int mobid = getInt(pack+19+4*s3121004+i*(22+4*(hits-1)));
		packet.addInt(mobid); 
		packet.addByte(-1);
		for(int j=0; j<hits; j++){
			int damage = getInt(pack+37+4*s3121004+i*(22+4*(hits-1))+j*4);
			packet.addInt(damage);
		}
	}
	packet.sendTo(player, players, 0);
}

void MobsPacket::damageMobSkill(Player* player, vector <Player*> players, unsigned char* pack){
	int howmany = pack[1]/0x10;
	int hits = pack[1]%0x10;
	Packet packet = Packet();
	packet.addHeader(0x93);
	packet.addInt(player->getPlayerid());
	packet.addByte(pack[1]);
	packet.addByte(1);
	packet.addInt(getInt(pack+2));
	packet.addByte(pack[7]);
	packet.addByte(pack[9]);
	packet.addByte(0);
	packet.addInt(0);
	for(int i=0; i<howmany; i++){
		int mobid = getInt(pack+14+i*(22+4*(hits-1)));
		packet.addInt(mobid);
		packet.addByte(-1);
		for(int j=0; j<hits; j++){
			int damage = getInt(pack+32+i*(22+4*(hits-1))+j*4);
			packet.addInt(damage);
		}
	}
	packet.sendTo(player, players, 0);
}

void MobsPacket::showHP(Player* player, int mobid, char per){
	Packet packet = Packet();
	packet.addHeader(0x98);
	packet.addInt(mobid);
	packet.addByte(per);
	packet.packetSend(player);
}

void MobsPacket::dieMob(Player* player, vector<Player*> players, Mob* mob, int mobid){
	Packet packet = Packet();
	packet.addHeader(0xa5);
	packet.addInt(mobid);
	packet.addByte(1);
	packet.sendTo(player, players, 1); 
	if(mob->getControl() != NULL && mob->getControl()->getMap() == player->getMap()){
		Packet packet = Packet();
		packet.addHeader(0xa4);
		packet.addByte(0);
		packet.addInt(mobid);
		packet.packetSend(mob->getControl());
	}
}