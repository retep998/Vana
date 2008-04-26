#include "../Connection/Acceptor.h"
#include "PlayerLogin.h"
#include "Player.h"
#include "MySQLM.h"
#include <stdlib.h>
#include "Initializing.h"
#include "Timer.h"
#include "Skills.h"
#include "Maps.h"
#include "Server.h"

void main(){

	Initializing::initializing();
	printf("Initializing Timers... ");
	Timer::timer = new Timer();
	Skills::startTimer();
	Maps::startTimer();
	printf("DONE\n");
	printf("Initializing MySQL... ");
	if(MySQL::connectToMySQL())
		printf("DONE\n");
	else{
		printf("FAILED\n");
		exit(1);
	}
	Server::initialize();
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