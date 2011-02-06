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
#include "PlayerBuddyList.h"
#include "BuddyListPacket.h"
#include "ChannelServer.h"
#include "Database.h"
#include "PacketCreator.h"
#include "Player.h"
#include "SyncPacket.h"
#include <algorithm>

PlayerBuddyList::PlayerBuddyList(Player *player) : 
m_player(player),
sentRequest(false) 
{
	load();
}

void PlayerBuddyList::load() {
	mysqlpp::Query query = Database::getCharDB().query();
	query << "SELECT bl.id, bl.buddy_charid, bl.name AS name_cache, c.name, bl.groupname, u.online "
		<< "FROM buddylist bl "
		<< "LEFT JOIN characters c ON bl.buddy_charid = c.id "
		<< "LEFT JOIN users u ON c.userid = u.id "
		<< "WHERE bl.charid = " << m_player->getId();
	mysqlpp::StoreQueryResult res = query.store();

	for (size_t i = 0; i < res.num_rows(); ++i) {
		addBuddy(res[i]);
	}

	query << "SELECT pending.* FROM buddylist_pending pending "
		<< "LEFT JOIN characters c ON c.id = pending.inviter_id "
		<< "WHERE c.world_id = " << static_cast<int16_t>(ChannelServer::Instance()->getWorld()) 
		<< " AND pending.char_id = " << m_player->getId();
	res = query.store();
	
	enum TableData {
		CharID, InviterName, InviterId
	};

	BuddyInvite invite;
	for (size_t i = 0; i < res.num_rows(); ++i) {
		invite.m_id = atoi(res[i][InviterId]);
		res[i][InviterName].to_string(invite.m_name);
		m_pending_buddies.push_back(invite);
	}
}

uint8_t PlayerBuddyList::addBuddy(const string &name, const string &group, bool invite) {
	if (listSize() >= m_player->getBuddyListSize()) {
		// Buddy list full
		return BuddyListPacket::Errors::BuddyListFull;
	}

	if (name.size() < 4 || name.size() > 12 || group.size() > 13) {
		// Invalid name or groupname length
		return BuddyListPacket::Errors::UserDoesNotExist;
	}

	mysqlpp::Query query = Database::getCharDB().query();

	enum TableColumns {
		CharacterID, CharacterName, GM, BuddylistLimit, BuddylistSize
	};

	query << "SELECT "
		<< "c.id, c.name, u.gm, c.buddylist_size AS buddylist_limit, "
		<< "(SELECT COUNT(b.id) FROM buddylist b WHERE b.charid = c.id) AS buddylist_size "
		<< "FROM characters c "
		<< "INNER JOIN users u ON c.userid = u.id "
		<< "WHERE c.name = " << mysqlpp::quote << name << " AND world_id = " << static_cast<int16_t>(ChannelServer::Instance()->getWorld());

	mysqlpp::StoreQueryResult res = query.store();

	if (res.size() == 0) {
		// Name does not exist
		return BuddyListPacket::Errors::UserDoesNotExist;
	}

	mysqlpp::Row row = res[0];

	if (atoi(row[GM]) > 0 && !m_player->isGm()) {
		// GM cannot be in buddy list unless the player is a GM
		return BuddyListPacket::Errors::NoGms;
	}

	if (atoi(row[BuddylistSize]) >= atoi(row[BuddylistLimit])) {
		// Opposite-end buddy list full
		return BuddyListPacket::Errors::TargetListFull;
	}

	int32_t charid = atoi(row[CharacterID]);

	if (m_buddies.find(charid) != m_buddies.end()) {
		if (m_buddies[charid]->m_group_name == group) {
			// Already in buddy list
			return BuddyListPacket::Errors::AlreadyInList;
		}
		else {
			query << "UPDATE buddylist SET groupname = " << mysqlpp::quote << group << " WHERE buddy_charid = " << charid << " AND charid = " << m_player->getId();
			query.exec();
			m_buddies[charid]->m_group_name = group;
		}
	}
	else {
		query << "INSERT INTO buddylist (charid, buddy_charid, name, groupname) VALUES (" << m_player->getId() << ", " << charid << ", " << mysqlpp::quote << res[0][1] << ", " << mysqlpp::quote << group << ")";
		mysqlpp::SimpleResult res2 = query.execute();
	
		query << "SELECT bl.id, bl.buddy_charid, bl.name AS name_cache, c.name, bl.groupname, u.online "
			<< "FROM buddylist bl "
			<< "LEFT JOIN characters c ON bl.buddy_charid = c.id "
			<< "LEFT JOIN users u ON c.userid = u.id "
			<< "WHERE bl.id = " << res2.insert_id();

		res = query.store();

		addBuddy(res[0]);
		
		query << "SELECT id FROM buddylist WHERE charid = " << charid << " AND buddy_charid = " << m_player->getId();
		mysqlpp::StoreQueryResult res = query.store();

		if (res.num_rows() == 0) {
			if (invite) {
				SyncPacket::buddyInvite(ChannelServer::Instance()->getWorldConnection(), charid, m_player->getId());
			}
		}
		else {
			vector<int32_t> idVector;
			idVector.push_back(charid);
			SyncPacket::buddyOnline(ChannelServer::Instance()->getWorldConnection(), m_player->getId(), idVector, true);
		}
	}
	BuddyListPacket::update(m_player, BuddyListPacket::ActionTypes::Add);


	return BuddyListPacket::Errors::None;
}

void PlayerBuddyList::removeBuddy(int32_t charid) {
	mysqlpp::Query query = Database::getCharDB().query();

	if (m_pending_buddies.size() != 0 && sentRequest) {
		BuddyInvite invite = m_pending_buddies.front();
		if (invite.m_id == charid) {
			removePendingBuddy(charid, false);
		}
		return;
	}

	if (m_buddies.find(charid) == m_buddies.end()) {
		// Hacking
		return;
	}
	if (m_buddies[charid]->m_channel != -1) {
		vector<int32_t> idVector;
		idVector.push_back(charid);
		SyncPacket::buddyOnline(ChannelServer::Instance()->getWorldConnection(), m_player->getId(), idVector, false);
	}
	
	m_buddies.erase(charid);

	query << "DELETE FROM buddylist WHERE charid = " << m_player->getId() << " AND buddy_charid = " << charid;
	query.exec();

	BuddyListPacket::update(m_player, BuddyListPacket::ActionTypes::Remove);
}

void PlayerBuddyList::addBuddy(const mysqlpp::Row &row) {
	mysqlpp::Query query = Database::getCharDB().query();
	int32_t charid = atoi(row["buddy_charid"]);

	if (!row["name"].is_null() && row["name"] != row["name_cache"]) {
		// Outdated name cache, i.e. character renamed
		mysqlpp::Query query = Database::getCharDB().query();
		query << "UPDATE buddylist SET name = " << mysqlpp::quote << row["name"] << " WHERE id = " << row["id"];
		query.exec();
	}

	BuddyPtr buddy(new Buddy);
	buddy->m_charid = charid;

	// Note that the cache is for displaying the character name when the
	// character in question is deleted.
	if (row["name"].is_null()) {
		row["name_cache"].to_string(buddy->m_name); // Buddy's character deleted
	}
	else {
		row["name"].to_string(buddy->m_name);
	}

	int32_t channelid = -1;
	if (atoi(row["online"]) >= 20000) {
		int32_t onlineId = atoi(row["online"]) - 20000;

		channelid = onlineId % 100;
	}

	buddy->m_channel = channelid;
	if (row["groupname"].is_null()) {
		buddy->m_group_name = "Default Group";
		query << "UPDATE buddylist SET groupname = 'Default Group' WHERE buddy_charid = " << charid << " AND charid = " << m_player->getId();
		query.exec();
	}
	else {
		row["groupname"].to_string(buddy->m_group_name);
	}
	
	query << "SELECT id FROM buddylist WHERE charid = " << charid << " AND buddy_charid = " << m_player->getId();
	mysqlpp::StoreQueryResult res = query.store();

	if (res.num_rows() != 0) {
		buddy->m_oppositeStatus = BuddyListPacket::OppositeStatus::Registered;
	}
	else {
		buddy->m_oppositeStatus = BuddyListPacket::OppositeStatus::Unregistered;
	}

	m_buddies[charid] = buddy;
}

void PlayerBuddyList::addBuddies(PacketCreator &packet) {
	for (unordered_map<int32_t, BuddyPtr>::iterator iter = m_buddies.begin(); iter != m_buddies.end(); iter++) {
		PlayerBuddyList::BuddyPtr buddy = iter->second;
		packet.add<int32_t>(buddy->m_charid);
		packet.addString(buddy->m_name, 13);
		packet.add<uint8_t>(buddy->m_oppositeStatus);
		if (buddy->m_oppositeStatus == BuddyListPacket::OppositeStatus::Unregistered) {
			packet.add<uint16_t>(0x00);
			packet.add<uint8_t>(0xF0);
			packet.add<uint8_t>(0xB2);
		}
		else {
			packet.add<int32_t>(buddy->m_channel);
		}
		packet.addString(buddy->m_group_name, 13);
		packet.add<int8_t>(0x00);
		packet.add<int8_t>(20); // Seems to be the amount of buddy slots for the character...
		packet.add<uint8_t>(0xFD);
		packet.add<uint8_t>(0xBA);
	}
}

void PlayerBuddyList::checkForPendingBuddy() {
	if (m_pending_buddies.size() == 0 || sentRequest) {
		// No buddies pending or request sent (didn't receive answer yet)
		return;
	}

	BuddyListPacket::invitation(m_player, m_pending_buddies.front());
	sentRequest = true;
}

void PlayerBuddyList::removePendingBuddy(int32_t id, bool accepted) {
	if (m_pending_buddies.size() == 0 || !sentRequest) {
		// Hacking or something.
		return;
	}

	BuddyInvite invite = m_pending_buddies.front();
	if (invite.m_id != id) {
		// Hacking or something stupid.
		ChannelServer::Instance()->log(LogTypes::Warning, "Player tried to accept a player with playerid " + boost::lexical_cast<string>(id) + " but the sent playerid was " + boost::lexical_cast<string>(invite.m_id) + ". PlayerID: " + m_player->getName());
		return;
	}

	if (accepted) {
		int8_t error = addBuddy(invite.m_name, "Default Group", false);
		if (error != BuddyListPacket::Errors::None) {
			BuddyListPacket::error(m_player, error);
		}
		else {
			vector<int32_t> idVector;
			idVector.push_back(id);
			SyncPacket::buddyOnline(ChannelServer::Instance()->getWorldConnection(), m_player->getId(), idVector, true);
		}
		
		mysqlpp::Query query = Database::getCharDB().query();
		query << "DELETE FROM buddylist_pending WHERE char_id = " << m_player->getId() << " AND inviter_id = " << id;
		query.exec();
	}
	
	BuddyListPacket::update(m_player, BuddyListPacket::ActionTypes::First);

	m_pending_buddies.pop_front();
	sentRequest = false;
	checkForPendingBuddy();
}

vector<int32_t> PlayerBuddyList::getBuddyIds() {
	vector<int32_t> ids;
	for (unordered_map<int32_t, BuddyPtr>::iterator iter = m_buddies.begin(); iter != m_buddies.end(); iter++) {
		ids.push_back(iter->second->m_charid);
	}

	return ids;
}
