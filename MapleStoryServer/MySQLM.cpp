#include "MySQLM.h"
#include <sstream>
#include <string>
using namespace std;

MYSQL MySQL::maple_db;

int MySQL::connectToMySQL(){
	if(!mysql_real_connect(&maple_db, "localhost", "root", "", "maplestory", 3306, NULL, 0)){
		printf(mysql_error(&maple_db));
		return 0;
	}
	return 1;
}

int MySQL::getInt(char* table, int id, char* value){
	MYSQL_RES *mres;
	MYSQL_ROW mrow;
	char query[255]; 
	int ret = 0; 
	sprintf_s(query, 255, "SELECT %s FROM %s WHERE ID=%d;",value, table, id);
	mysql_real_query(&maple_db, query, strlen(query));
	mres = mysql_store_result(&MySQL::maple_db);
	if (mres == 0 ){
		printf_s("\n\rError in getINT SQL returned no rows (Mysql_error: %s)",mysql_error(&maple_db)); 
		printf_s("SQL Query is : %s",query);
	}
	else {
		mrow = mysql_fetch_row(mres);

		if(mysql_num_fields(mres) > 0){
			string mr = string((char*)mrow[0]);
			istringstream buffer(mr);
			buffer >> ret;
		}
	}
	mysql_free_result(mres);
	return ret;
}
int MySQL::getCharactersIDs(int id, int IDs[3]){
	MYSQL_RES *mres;
	MYSQL_ROW mrow;
	char query[255]; 
    sprintf_s(query, 255, "SELECT ID FROM characters WHERE userid = %d;", id);
	mysql_real_query(&maple_db, query, strlen(query));
	mres = mysql_store_result(&MySQL::maple_db);
	if (mres == 0 ){
		printf_s("\n\rError in getCharactersIDs SQL returned no rows (Mysql_error: %s)",mysql_error(&maple_db)); 
		printf_s("SQL Query is : %s",query);
	}
	else {
		mrow = mysql_fetch_row(mres);
		int ret = 0;
		for(int i=0; i<mysql_num_rows(mres); i++){
			string mr = string((char*)mrow[0]);
			istringstream buffer(mr);
			buffer >> IDs[i];
			mrow = mysql_fetch_row(mres);
		}
	}
	int ret = (int)mysql_num_rows(mres);
	mysql_free_result(mres);
	return ret;
}
int MySQL::isString(char* table, char* whr, char* wht){
	MYSQL_RES *mres;
	MYSQL_ROW mrow;
	char query[255]; 
    sprintf_s(query, 255, "select * from %s where %s='%s';",table, whr, wht);
	mysql_real_query(&maple_db, query, strlen(query));
	mres = mysql_store_result(&MySQL::maple_db);
	mrow = mysql_fetch_row(mres);
	if(mrow){
		return 1;
	}
	return 0;
}

void MySQL::getString(char* table, char* whr, char* wht, char* value, char* string){
	MYSQL_RES *mres;
	MYSQL_ROW mrow;
	char query[255]; 
    sprintf_s(query, 255, "SELECT %s FROM %s WHERE %s='%s';",value, table, whr, wht);
	mysql_real_query(&maple_db, query, strlen(query));
	mres = mysql_store_result(&MySQL::maple_db);
	if (mres == 0 ){
		printf_s("\n\rError in getSTRING SQL returned no rows (Mysql_error: %s)",mysql_error(&maple_db)); 
		printf_s("SQL Query is : %s",query);
	}
	else {
		mrow = mysql_fetch_row(mres);
		if(mysql_num_rows(mres) > 0){
			strcpy_s(string, 13, (char*)mrow[0]);
		}
	}
	mysql_free_result(mres);
}
void MySQL::getStringI(char* table, char* whr, int wht, char* value, char* string){
	MYSQL_RES *mres;
	MYSQL_ROW mrow;
	char query[255]; 
    sprintf_s(query, 255, "SELECT %s FROM %s WHERE %s = %d;",value, table, whr, wht);
	mysql_real_query(&maple_db, query, strlen(query));
	mres = mysql_store_result(&MySQL::maple_db);
	if (mres == 0 ){
		printf_s("\n\rError in getStringI SQL returned no rows (Mysql_error: %s)",mysql_error(&maple_db)); 
		printf_s("SQL Query is : %s",query);
	}
	else {
		mrow = mysql_fetch_row(mres);
		if(mysql_num_rows(mres) > 0){
			strcpy_s(string, 13, (char*)mrow[0]);
		}
	}
	mysql_free_result(mres);
}

void MySQL::setInt(char* table, char* wht, int id, int value){
	char query[255]; 
    sprintf_s(query, 255, "update %s set %s='%d' where ID='%d';", table, wht, value, id);
	mysql_real_query(&maple_db, query, strlen(query));
}

void MySQL::setString(char* table, char* wht, int id, char* value){
	char query[255]; 
    sprintf_s(query, 255, "update %s set %s='%s' where ID='%d';", table, wht, value, id);
	mysql_real_query(&maple_db, query, strlen(query));
}

int MySQL::getUserID(char *username){
	MYSQL_RES *mres;
	MYSQL_ROW mrow;
	char query[255]; 
	int ret = 0;
	sprintf_s(query, 255, "SELECT id FROM users WHERE username='%s';",username);
	mysql_real_query(&maple_db, query, strlen(query));
	mres = mysql_store_result(&maple_db);
	if (mres == 0 ){
		printf_s("\n\rError in getUserID SQL returned no rows (Mysql_error: %s)",mysql_error(&maple_db)); 
		printf_s("SQL Query is : %s",query);
	}
	else {
		if(mysql_num_rows(mres) > 0){
			mrow = mysql_fetch_row(mres);
			string mr = string((char*)mrow[0]);
			istringstream buffer(mr);
			buffer >> ret;
		}
	}
	mysql_free_result(mres);
	return ret;
}
int MySQL::setChar(int userid){
	char query[255]; 
    sprintf_s(query, 255, "insert into characters(userid) values(%d);", userid);
	mysql_real_query(&maple_db, query, strlen(query));
	int IDs[3];
	int num = getCharactersIDs(userid, IDs);
	return IDs[num-1];
}

void MySQL::charInfo(char* wht, int id){
	char query[255]; 
    sprintf_s(query, 255, "update characters set %s where ID='%d';", wht, id);
	mysql_real_query(&maple_db, query, strlen(query));
}

void MySQL::deleteRow(char* table, int id){
	char query[255]; 
    sprintf_s(query, 255, "delete from %s where ID='%d';", table, id);
	mysql_real_query(&maple_db, query, strlen(query));

}

void MySQL::insert(char* query){
	mysql_real_query(&maple_db, query, strlen(query));
}

int MySQL::showEquips(int id, int equips[30][2]){
    MYSQL_RES *mres;
    MYSQL_ROW mrow;
    char query[255]; 
    int ret = 0;
    sprintf_s(query, 255, "SELECT equipid, type FROM equip WHERE (charid = %d AND pos<0);", id);
    mysql_real_query(&maple_db, query, strlen(query));
    mres = mysql_store_result(&MySQL::maple_db);
    if (mres == 0 ){
        printf_s("\n\rError in showEquips SQL returned no rows (Mysql_error: %s)",mysql_error(&maple_db)); 
        printf_s("SQL Query is : %s",query);
    }
    else {
        mrow = mysql_fetch_row(mres);
        for(int i=0; i<mysql_num_rows(mres); i++){
            string mr = string((char*)mrow[0]);
            istringstream buffer(mr);
            buffer >> equips[i][0];
            string mr2 = string((char*)mrow[1]);
            istringstream buffer2(mr2);
            buffer2 >> equips[i][1];
            mrow = mysql_fetch_row(mres);
        }
    }
    ret = (int)mysql_num_rows(mres);  
    mysql_free_result(mres);
    return ret;
}
int MySQL::showEquipsIn(int id, int equips[130][21]){
	MYSQL_RES *mres;
	MYSQL_ROW mrow;
	char query[255]; 
    sprintf_s(query, 255, "SELECT * FROM equip WHERE charid = %d;", id);
	mysql_real_query(&maple_db, query, strlen(query));
	mres = mysql_store_result(&MySQL::maple_db);
	if (mres == 0 ){
		printf_s("\n\rError in getEquipsIn SQL returned no rows (Mysql_error: %s)",mysql_error(&maple_db)); 
		printf_s("SQL Query is : %s",query);
	}
	else {
		mrow = mysql_fetch_row(mres);
		for(int i=0; i<mysql_num_rows(mres); i++){
			for(int j=0; j<21; j++){
				string mr = string((char*)mrow[j]);
				istringstream buffer(mr);
				buffer >> equips[i][j];
			}
			mrow = mysql_fetch_row(mres);
		}
	}
	int ret = (int)mysql_num_rows(mres);
	mysql_free_result(mres);
	return ret;
}

int MySQL::getItems(int id, int equips[400][4]){
	MYSQL_RES *mres;
	MYSQL_ROW mrow;
	char query[255]; 
    sprintf_s(query, 255, "SELECT * FROM items WHERE charid = %d;", id);
	mysql_real_query(&maple_db, query, strlen(query));
	mres = mysql_store_result(&MySQL::maple_db);
	if (mres == 0 ){
		printf_s("\n\rError in getItems SQL returned no rows (Mysql_error: %s)",mysql_error(&maple_db)); 
		printf_s("SQL Query is : %s",query);
	}
	else {
		mrow = mysql_fetch_row(mres);
		//int ret = 0;
		for(int i=0; i<mysql_num_rows(mres); i++){
			for(int j=0; j<5; j++){
				string mr = string((char*)mrow[j]);
				istringstream buffer(mr);
				if(j>1)
					buffer >> equips[i][j-1];
				else if(j!=1)
					buffer >> equips[i][0];
			}
			mrow = mysql_fetch_row(mres);
		}
	}
	int ret = (int)mysql_num_rows(mres);
	mysql_free_result(mres);
	return ret;
}
int MySQL::getSkills(int id, int skills[200][2]){
	MYSQL_RES *mres;
	MYSQL_ROW mrow;
	char query[255]; 
    sprintf_s(query, 255, "SELECT * FROM skills WHERE charid = %d;", id);
	mysql_real_query(&maple_db, query, strlen(query));
	mres = mysql_store_result(&MySQL::maple_db);
	if (mres == 0 ){
		printf_s("\n\rError in getSkills SQL returned no rows (Mysql_error: %s)",mysql_error(&maple_db)); 
		printf_s("SQL Query is : %s",query);
	}
	else {
		mrow = mysql_fetch_row(mres);
		//int ret = 1;
		for(int i=0; i<mysql_num_rows(mres); i++){
			for(int j=1; j<3; j++){
				string mr = string((char*)mrow[j]);
				istringstream buffer(mr);
				buffer >> skills[i][j-1];
			}
			mrow = mysql_fetch_row(mres);
		}
	}
	int ret = (int)mysql_num_rows(mres);
	mysql_free_result(mres);
	return ret;
}
void MySQL::getKeys(int id, int keys[90]){
	MYSQL_RES *mres;
	MYSQL_ROW mrow;
	char query[255]; 
	int ret = 0;
    sprintf_s(query, 255, "SELECT * FROM keymap WHERE charid = %d;", id);
	mysql_real_query(&maple_db, query, strlen(query));
	mres = mysql_store_result(&MySQL::maple_db);
	if (mres == 0 ){
		printf_s("\n\rError in getKeys SQL returned no rows (Mysql_error: %s)",mysql_error(&maple_db)); 
		printf_s("SQL Query is : %s",query);
	}
	else {
		mrow = mysql_fetch_row(mres);
		for(int i=0; i<90; i++){
			string mr = string((char*)mrow[i+1]);
			istringstream buffer(mr);
			buffer >> keys[i];
		}
	}
	mysql_free_result(mres);
}