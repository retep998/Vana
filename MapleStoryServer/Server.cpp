#include "Server.h"
#include "ServerPacket.h"
#include "Player.h"
#include <windows.h>

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

void Server::initialize(){
	strcpy_s(msg, 100, "Welcome to MapleStory!");
	on = 1;
}

__int64 Server::getServerTime(){
	SYSTEMTIME systemTime;
	GetSystemTime( &systemTime );
	FILETIME fileTime; 
	SystemTimeToFileTime( &systemTime, &fileTime );
	ULARGE_INTEGER uli;
	uli.LowPart = fileTime.dwLowDateTime; 
	uli.HighPart = fileTime.dwHighDateTime;
	return uli.QuadPart;
}