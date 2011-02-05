/*
Copyright (C) 2008-2011 Vana Development Team

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
#include "GuildBbs.h"
#include "Database.h"
#include "TimeUtilities.h"

GuildBbs::~GuildBbs() {
	if (notice)
		delete notice;
	m_threads.clear();
}

void GuildBbs::load() {
	notice = 0;
	m_threads.clear();
	mysqlpp::Query query = Database::getCharDB().query();
	query << "SELECT id, listid, user, time, icon, title, content FROM guild_bbs_threads WHERE guild = " << guild->getId() << " ORDER BY listid DESC";
	mysqlpp::StoreQueryResult res = query.store();

	for (size_t i = 0; i < res.num_rows(); ++i) {
		int32_t listid = res[i][1];
		BbsThread *thread = new BbsThread(res[i][0],
			listid, 
			res[i][2],
			mysqlpp::DateTime(res[i][3]), 
			res[i][4],
			(string) res[i][5], 
			(string) res[i][6]);
		addThread(thread);

		if (listid > lastid)
			lastid = listid;
	}
}

void GuildBbs::save() {
	mysqlpp::Query query = Database::getCharDB().query();
	query << "DELETE FROM guild_bbs_threads WHERE guild = " << guild->getId();
	query.exec();

	bool firstrun = true;
	for (map<int32_t, BbsThread *>::iterator iter = m_threads.begin(); iter != m_threads.end(); iter++) {
		if (firstrun) {
			query << "INSERT INTO guild_bbs_threads VALUES (";
			firstrun = false;
		}
		else {
			query << ",(";
		}
		query << iter->second->getId() << ","
			<< iter->second->getUserId() << ","
			<< guild->getId() << ","
			<< mysqlpp::quote << (string) mysqlpp::DateTime(iter->second->getTime()) << ","
			<< iter->second->getIcon() << ","
			<< mysqlpp::quote << iter->second->getTitle() << ","
			<< mysqlpp::quote << iter->second->getContent() << ","
			<< iter->second->getListId() << ")";
		iter->second->saveReplies();
	}
	if (notice != nullptr) {
		if (firstrun) {
			query << "INSERT INTO guild_bbs_threads VALUES (";
			firstrun = false;
		}
		else {
			query << ",(";
		}
		query << notice->getId() << ","
			<< notice->getUserId() << ","
			<< guild->getId() << ","
			<< mysqlpp::quote << (string) mysqlpp::DateTime(notice->getTime()) << ","
			<< notice->getIcon() << ","
			<< mysqlpp::quote << notice->getTitle() << ","
			<< mysqlpp::quote << notice->getContent() << ","
			<< notice->getListId() << ")";
		notice->saveReplies();

	}
	if (!firstrun)
		query.exec();
}

void GuildBbs::addThread(BbsThread *thread) {
	if (thread->getListId() == 0)
		setNotice(thread);
	else
		m_threads[thread->getListId()] = thread;
	thread->loadReplies();
}

void GuildBbs::removeThread(int32_t id) {
	if (id == 0) {
		delete notice;
		notice = nullptr; // If we don't do this it will crash the server :/
	}
	else {
		delete m_threads[id];
		m_threads.erase(id);
	}
	save();
}

BbsThread * GuildBbs::getThread(int32_t threadid) {
	if (threadid == 0)
		return notice;
	return m_threads.find(threadid) != m_threads.end() ? m_threads[threadid] : nullptr;
}

void BbsThread::addReply(BbsReply *reply) {
	if (reply != nullptr) {
		m_replies[reply->getId()] = reply;
	}
}

void BbsThread::loadReplies() {
	mysqlpp::Query query = Database::getCharDB().query();
	query << "SELECT id, user, time, content FROM guild_bbs_replies WHERE thread = " << id << " ORDER BY time ASC";
	mysqlpp::StoreQueryResult res = query.store();

	for (size_t i = 0; i < res.num_rows(); ++i) {
		BbsReply *reply = new BbsReply(res[i][0],
			res[i][1],
			mysqlpp::DateTime(res[i][2]),
			(string) res[i][3]);
		addReply(reply);
	}
}

void BbsThread::saveReplies() {
	mysqlpp::Query query = Database::getCharDB().query();
	query << "DELETE FROM guild_bbs_replies WHERE thread = " << id;
	query.exec();

	bool firstrun = true;
	for (map<int32_t, BbsReply *>::iterator iter = m_replies.begin(); iter != m_replies.end(); iter++) {
		if (firstrun) {
			query << "INSERT INTO guild_bbs_replies VALUES (";
			firstrun = false;
		}
		else {
			query << ",(";
		}
		query << iter->second->getId() << ","
			<< id << ","
			<< iter->second->getUserId() << ","
			<< mysqlpp::quote << (string) mysqlpp::DateTime(iter->second->getTime()) << ","
			<< mysqlpp::quote << iter->second->getContent() << ")";
	}
	if (!firstrun)
		query.exec();
}

void BbsThread::removeReply(int32_t id) {
	delete m_replies[id];
	m_replies.erase(id);
}
