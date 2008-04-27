#ifndef MYSQL_H
#define MYSQL_H

#include <stdio.h>
#include <Winsock2.h>
#include <vector>
#include "mysql.h"

class MySQL {
private:
	static MYSQL maple_db;
public:
	static int connectToMySQL();
	static int getUserID(char* username);
	static int getInt(char* table, int id, char* value);
	static void setInt(char* table, char* wht, int id, int value);
	static void setString(char* table, char* wht, int id, char* value);
	static int isString(char* table, char* whr, char* wht);
	static void getString(char* table, char* whr, char* wht, char* value, char* string);
	static void getStringI(char* table, char* whr, int wht, char* value, char* string);
	static int getCharactersIDs(int id, int IDs[3]);
	static int setChar(int userid);
	static void charInfo(char* wht, int id);
	static void deleteRow(char* table, int id);
	static void insert(char* query);
	static int showEquips(int id, int equips[30][2]);
	static int showEquipsIn(int id, int equips[130][21]);
	static int getItems(int id, int equips[400][4]);
	static void getKeys(int id, int keys[90]);
	static int getSkills(int id, int skills[200][2]);
	static MYSQL_RES* getRes(char *query);
};

#endif
