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
#pragma once

#include "Types.h"
#include "Guild.h"
#include <boost/tr1/unordered_map.hpp>
#include <map>
#include <string>

using std::tr1::unordered_map;
using std::map;
using std::string;

class Guild;

class BbsReply {
public:
	BbsReply(int32_t id, int32_t user, time_t time, string content) : id(id),
	user(user),
	time(time),
	content(content) { };

	int32_t getId() const { return id; }
	int32_t getUserId() const { return user; }
	time_t getTime() const { return time; }
	string getContent() const { return content; }
private:
	int32_t id;
	int32_t user;
	time_t time;
	string content;
};

class BbsThread {
public:
	BbsThread(int32_t id, int32_t listid, int32_t user, time_t time, int16_t icon, const string &title, const string &content) : id(id),
	user(user),
	time(time),
	icon(icon),
	title(title),
	content(content),
	listid(listid) { loadReplies(); }
	~BbsThread() { m_replies.clear(); }

	void addReply(BbsReply *reply);
	void loadReplies();
	void saveReplies();
	void setTitle(const string &title) { this->title = title; }
	void setIcon(int16_t iconid) { icon = iconid; }
	void setContent(const string &text) { content = text; }
	void removeReply(int32_t id);

	int32_t getId() const { return id; }
	int32_t getListId() const { return listid; }
	int32_t getUserId() const { return user; }
	time_t getTime() const { return time; }
	int32_t getSize() const { return m_replies.size(); }
	int16_t getIcon() const { return icon; }
	string getTitle() const { return title; }
	string getContent() const { return content; }
	BbsReply * getReply(int32_t replyid) { return (m_replies.find(replyid) == m_replies.end() ? nullptr : m_replies[replyid]); }
	map<int32_t, BbsReply *> m_replies;

private:
	int32_t id;
	int32_t listid;
	int32_t user;
	time_t time;
	int16_t icon;
	string title;
	string content;
};

class GuildBbs {
public:
	GuildBbs(Guild *guild) : guild(guild), lastid(-1), notice(nullptr) { load(); }
	~GuildBbs();
	void load();
	void save();
	void addThread(BbsThread *thread);
	void removeThread(int32_t id);
	void setLastThreadId(int32_t id) { lastid = id; }
	void setNotice(BbsThread *notice) { this->notice = notice; }

	BbsThread *getThread(int32_t threadid);
	int32_t getLastThreadId() const { return lastid; }
	int32_t getThreadSize() const { return m_threads.size(); }
	BbsThread *getNotice() const { return notice; }
	map<int32_t, BbsThread *> m_threads;
private:
	Guild *guild;
	int32_t lastid;
	BbsThread *notice;
};
