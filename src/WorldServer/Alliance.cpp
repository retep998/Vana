/*
Copyright (C) 2009 Vana Development Team

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
#include "Alliance.h"
#include "Database.h"
#include "Guild.h"

void Alliance::addGuild(Guild *guild) { 
	m_guilds[guild->getId()] = guild; 
}

void Alliance::save() {
	mysqlpp::Query query = Database::getCharDB().query();
	query << "UPDATE alliances SET " <<
		"notice = " << mysqlpp::quote << notice << ", " <<
		"rank1title = " << mysqlpp::quote << title[0] << ", " <<
		"rank2title = " << mysqlpp::quote << title[1] << ", " <<
		"rank3title = " << mysqlpp::quote << title[2] << ", " <<
		"rank4title = " << mysqlpp::quote << title[3] << ", " <<
		"rank5title = " << mysqlpp::quote << title[4] << ", " <<
		"capacity = " << capacity << ", " <<
		"leader = " << leader << " " <<
		"WHERE id = " << id;
	query.exec();
}

uint8_t Alliance::getLowestRank() {
	if (!title[3].empty()) {
		if (!title[4].empty()) {
			return 5;
		}
		else {
			return 4;
		}
	}
	else {
		return 3;
	}
}
