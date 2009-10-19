/*
Copyright (C) 2008-2009 Vana Development Team

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
#include "BbsHandler.h"
#include "BbsPacket.h"
#include "Channels.h"
#include "Database.h"
#include "Guild.h"
#include "GuildBbs.h"
#include "PacketReader.h"
#include "Player.h"
#include "PlayerDataProvider.h"
#include "WorldServerAcceptConnection.h"

void BbsHandler::handlePacket(WorldServerAcceptConnection *player, PacketReader &packet) {
	switch(packet.get<int8_t>()) {
		case 0x01: handleNewThread(packet); break;
		case 0x02: handleDeleteThread(packet); break;
		case 0x03: handleNewReply(packet); break;
		case 0x04: handleDeleteReply(packet); break;
		case 0x05: handleShowThreadList(packet); break;
		case 0x06: handleShowThread(packet); break;
	}
}

void BbsHandler::handleNewThread(PacketReader &packet) {
	int32_t guildid = packet.get<int32_t>();
	int32_t playerid = packet.get<int32_t>();
	Guild *guild = PlayerDataProvider::Instance()->getGuild(guildid);
	Player *player = PlayerDataProvider::Instance()->getPlayer(playerid);
	if (guild == 0 || player == 0)
		return;

	GuildBbs *bbs = guild->getBbs();

	if (guildid != player->getGuild()->getId()) {
		std::cout << player->getName() << " wants to post a new thread without being in the guild!" << std::endl;
		return;
	}

	int32_t threadid = -1;
	bool isEdit = packet.getBool();
	if (isEdit) 
		threadid = packet.get<int32_t>();
	bool isNotice = packet.getBool();
	if (isNotice && !isEdit)
		threadid = 0;
	else if (!isEdit)
		threadid = bbs->getLastThreadId() + 1;

	if (isNotice && bbs->getNotice() != 0)
		return; // We dont want 2 notices...

	string title = packet.getString();
	string text = packet.getString();
	int16_t icon = (int16_t) packet.get<int32_t>();
	
	if (!isEdit) {
		mysqlpp::Query query = Database::getCharDB().query();
		query << "INSERT INTO guild_bbs_threads (user, guild, time, icon, title, content, listid) VALUES ("
			<< playerid << ", "
			<< guildid << ", "
			<< "NOW()" << ", "
			<< icon << ", "
			<< mysqlpp::quote << title << ", "
			<< mysqlpp::quote << text << ", "
			<< threadid << ");";
		query.exec();

		bbs->load(); // Load all the threads again...

		handleShowThread(playerid, threadid);
		if (!isNotice)
			bbs->setLastThreadId(threadid);
	}
	else {
		BbsThread *thread = bbs->getThread(threadid);
		if (thread == 0)
			return;

		thread->setIcon(icon);
		thread->setTitle(title);
		thread->setContent(text);
		
		bbs->save();
		bbs->load();

		handleShowThread(playerid, threadid);
	}
}

void BbsHandler::handleShowThread(int32_t playerid, int32_t threadid) {
	Player *player = PlayerDataProvider::Instance()->getPlayer(playerid);
	Guild *guild = player->getGuild();
	if (player == 0 || guild == 0) 
		return;

	BbsThread *thread = guild->getBbs()->getThread(threadid);
	if (thread == 0)
		return;

	BbsPacket::sendThreadData(player->getChannel(), thread, playerid);
}

void BbsHandler::handleShowThread(PacketReader &pack) {
	int32_t playerid = pack.get<int32_t>();
	int32_t guildid = pack.get<int32_t>();
	int32_t threadid = pack.get<int32_t>();

	Player *player = PlayerDataProvider::Instance()->getPlayer(playerid);
	Guild *guild = PlayerDataProvider::Instance()->getGuild(guildid);
	if (player == 0 || player->getGuild() == 0 || guild == 0) 
		return;

	BbsThread *thread = guild->getBbs()->getThread(threadid);
	if (thread == 0)
		return;

	BbsPacket::sendThreadData(player->getChannel(), thread, playerid);
}

void BbsHandler::handleDeleteThread(PacketReader &packet) {
	int32_t playerid = packet.get<int32_t>();
	int32_t guildid = packet.get<int32_t>();
	int32_t threadid = packet.get<int32_t>();

	Player *player = PlayerDataProvider::Instance()->getPlayer(playerid);
	Guild *guild = player->getGuild();
	if (player == 0 || guild == 0 || player->getGuild()->getId() != guildid) 
		return;

	GuildBbs *bbs = guild->getBbs();

	BbsThread * thread = bbs->getThread(threadid);
	if (thread == 0 || (thread->getUserId() != playerid && player->getGuildRank() < 2)) 
		return;

	bbs->removeThread(threadid);
	bbs->save();
}

void BbsHandler::handleShowThreadList(PacketReader &pack) {
	int32_t playerid = pack.get<int32_t>();
	int32_t guildid = pack.get<int32_t>();
	int16_t page = pack.get<int16_t>();

	Player *player = PlayerDataProvider::Instance()->getPlayer(playerid);
	Guild * guild = PlayerDataProvider::Instance()->getGuild(guildid);
	if (player == 0 || player->getGuild() == 0 || guild == 0) 
		return;

	BbsPacket::sendThreadList(player->getChannel(), guild, playerid, page);
}

void BbsHandler::handleNewReply(PacketReader &packet) {
	int32_t playerid = packet.get<int32_t>();
	int32_t guildid = packet.get<int32_t>();
	int32_t threadid = packet.get<int32_t>();
	string replytext = packet.getString();

	Player *player = PlayerDataProvider::Instance()->getPlayer(playerid);
	Guild *guild = PlayerDataProvider::Instance()->getGuild(guildid);
	if (player == 0 || player->getGuild() == 0 || guild == 0) 
		return;

	BbsThread * thread = guild->getBbs()->getThread(threadid);
	if (thread == 0) 
		return;

	thread->saveReplies();
	mysqlpp::Query query = Database::getCharDB().query();
	query << "INSERT INTO guild_bbs_replies (user, thread, content, time) VALUES (" << playerid
		<< ", " << thread->getId()
		<< ", " << mysqlpp::quote << replytext
		<< ", " << "NOW()"
		<< ")";
	query.exec();

	thread->loadReplies();

	handleShowThread(playerid, thread->getListId());
}

void BbsHandler::handleDeleteReply(PacketReader &packet) {
	int32_t playerid = packet.get<int32_t>();
	int32_t guildid = packet.get<int32_t>();
	int32_t threadid = packet.get<int32_t>();
	int32_t replyid = packet.get<int32_t>();

	Player *player = PlayerDataProvider::Instance()->getPlayer(playerid);
	Guild *guild = PlayerDataProvider::Instance()->getGuild(guildid);
	if (player == 0 || player->getGuild() == 0 || guild == 0) 
		return;

	BbsThread *thread = guild->getBbs()->getThread(threadid);
	if (thread == 0) 
		return;

	BbsReply *reply = thread->getReply(replyid);
	if (reply == 0 || (reply->getUserId() != playerid && player->getGuildRank() < 2))
		return;

	thread->removeReply(replyid);
	thread->saveReplies();
	thread->loadReplies();

	handleShowThread(playerid, thread->getListId());
}
