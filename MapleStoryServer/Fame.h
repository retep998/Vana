#ifndef FAME_H
#define FAME_H

#include <vector>
using namespace std;

class Player;
	
namespace Fame{
	void handleFame(Player* player, unsigned char* packet);
	int canFame(Player* player, int to);
	void addFameLog(int from, int to);
	bool getLastFameLog(int from);
	bool getLastFameSPLog(int from, int to);
};

#endif