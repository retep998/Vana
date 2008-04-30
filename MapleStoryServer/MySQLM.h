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
