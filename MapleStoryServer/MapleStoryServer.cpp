#include "../Connection/Acceptor.h"
#include "PlayerLogin.h"
#include "Player.h"
#include <stdlib.h>
#include "Initializing.h"
#include "Skills.h"
#include "Maps.h"
#include "Server.h"

void main(){
	Initializing::initializing();

	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != NO_ERROR)  printf("Error at WSAStartup()\n");

	srand((unsigned char)time(0));

	Selector* selector = new Selector();
	Acceptor::Acceptor(8484, selector, new PlayerLoginFactory());
	for(int i=0; i<5; i++){
		Acceptor::Acceptor(8888+i, selector, new PlayerFactory());
	}
	while(getchar()){}
}