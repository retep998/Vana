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
#include "Player.h"
#include "CashServer.h"
#include "Connectable.h"
#include "Database.h"
#include "GameConstants.h"
#include "GameLogicUtilities.h"
#include "IpUtilities.h"
#include "Inventory.h"
#include "ItemDataProvider.h"
#include "KeyMaps.h"
#include "MapleSession.h"
#include "PacketReader.h"
#include "Pet.h"
#include "PlayerDataProvider.h"
#include "PlayerHandler.h"
#include "PlayerPacket.h"
#include "RecvHeader.h"
#include "ServerPacket.h"
#include "StringUtilities.h"
#include "SyncPacket.h"
#include "TimeUtilities.h"
#include <boost/array.hpp>
#include <stdexcept>

Player::Player() :
fall_counter(0),
shop(0),
item_effect(0),
chair(0),
mapchair(0),
trade_id(0),
trade_state(false),
save_on_dc(true),
is_connect(false),
party(nullptr),
instance(nullptr)
{
}

Player::~Player() {
	if (is_connect) {
		//if (this->getHp() == 0)
		//	this->acceptDeath();
		// "Bug" in global, would be fixed here:
		// When disconnecting and dead, you actually go back to forced return map before the death return map
		// (that means that it's parsed while logging in, not while logging out)
		if (save_on_dc) {
			saveAll(true);
			setOnline(false);
		}
		if (CashServer::Instance()->isConnected()) { // Do not connect to worldserver if the worldserver has disconnected
			SyncPacket::removePlayer(CashServer::Instance()->getWorldConnection(), id);
		}
		PlayerDataProvider::Instance()->removePlayer(this);
	}
}

void Player::realHandleRequest(PacketReader &packet) {
	try {
		int16_t header = packet.get<int16_t>();
		if (!is_connect) {
			// Same as the channelserver, don't accept any other packets while not being logged on.
			if (header == CMSG_PLAYER_LOAD) {
				playerConnect(packet);
			}
		}
		else {
			switch (header) {
				case CMSG_MAP_CHANGE: changeChannel(0); break;
				case CMSG_CASHSHOP_CHECK_CASH: PlayerPacket::showNX(this, true); break;
				case CMSG_CASHSHOP_OPERATION: PlayerHandler::handleOperation(this, packet); break;
				case CMSG_CASHSHOP_REDEEM_COUPON: PlayerHandler::handleRedeemCoupon(this, packet); break;
				case CMSG_PONG: getInventory()->checkExpiredItems(); getStorage()->checkExpiredItems(); break;
				case CMSG_REQUEST_CHARACTER_TRANSFER: PlayerHandler::handleChangeWorldRequest(this, packet); break;
				case CMSG_REQUEST_NAME_CHANGE: PlayerHandler::handleChangeNameRequest(this, packet); break;
				case CMSG_SEND_NOTE: PlayerHandler::handleSendNote(this, packet); break;
			}
		}
	}
	catch (std::range_error) {
		// Packet data didn't match the packet length somewhere
		// This isn't always evidence of tampering with packets
		// We may not process the structure properly

		packet.reset();
		std::stringstream x;
		x << "Player ID: " << getId() << "; Packet: " << packet;
		CashServer::Instance()->log(LogTypes::MalformedPacket, x.str());
		getSession()->disconnect();
	}
}

void Player::playerConnect(PacketReader &packet) {
	int32_t id = packet.get<int32_t>();
	if (!Connectable::Instance()->checkPlayer(id, getIp())) {
		// Hacking
		getSession()->disconnect();
		return;
	}
	this->id = id;

	// Character info
	mysqlpp::Query query = Database::getCharDB().query();

	query << "SELECT "
		<< "c.*, u.gm, u.admin, u.username, u.char_delete_password FROM characters c "
		<< "LEFT JOIN users u ON c.userid = u.id "
		<< "WHERE c.id = " << id;

	mysqlpp::StoreQueryResult res = query.store();

	if (res.empty()) {
		// Hacking
		getSession()->disconnect();
		return;
	}

	res[0]["name"].to_string(name);
	res[0]["username"].to_string(userName);
	user_id		= res[0]["userid"];
	map			= res[0]["map"];
	gm_level	= res[0]["gm"];
	admin		= StringUtilities::atob(res[0]["admin"]);
	eyes		= res[0]["eyes"];
	hair		= res[0]["hair"];
	world_id	= static_cast<int8_t>(res[0]["world_id"]);
	gender		= static_cast<int8_t>(res[0]["gender"]);
	skin		= static_cast<int8_t>(res[0]["skin"]);
	map_pos		= static_cast<int8_t>(res[0]["pos"]);
	buddylist_size = static_cast<uint8_t>(res[0]["buddylist_size"]);
	guildid		= res[0]["guildid"];
	guildrank	= static_cast<uint8_t>(res[0]["guildrank"]);
	alliancerank = static_cast<uint8_t>(res[0]["alliancerank"]);
	allianceid = res[0]["allianceid"];
	birthDate = res[0]["char_delete_password"];

	// Stats
	stats.reset(new PlayerStats(this, static_cast<uint8_t>(res[0]["level"]),
		static_cast<int16_t>(res[0]["job"]),
		static_cast<int16_t>(res[0]["fame"]),
		static_cast<int16_t>(res[0]["str"]),
		static_cast<int16_t>(res[0]["dex"]),
		static_cast<int16_t>(res[0]["int"]),
		static_cast<int16_t>(res[0]["luk"]),
		static_cast<int16_t>(res[0]["ap"]),
		static_cast<uint16_t>(res[0]["hpmp_ap"]),
		static_cast<int16_t>(res[0]["sp"]),
		static_cast<int16_t>(res[0]["chp"]),
		static_cast<int16_t>(res[0]["mhp"]),
		static_cast<int16_t>(res[0]["cmp"]),
		static_cast<int16_t>(res[0]["mmp"]),
		res[0]["exp"]));

	// Inventory
	mounts.reset(new PlayerMounts(this));
	pets.reset(new PlayerPets(this));
	boost::array<uint8_t, Inventories::InventoryCount> maxslots;
	maxslots[0] = static_cast<uint8_t>(res[0]["equip_slots"]);
	maxslots[1] = static_cast<uint8_t>(res[0]["use_slots"]);
	maxslots[2] = static_cast<uint8_t>(res[0]["setup_slots"]);
	maxslots[3] = static_cast<uint8_t>(res[0]["etc_slots"]);
	maxslots[4] = static_cast<uint8_t>(res[0]["cash_slots"]);
	inv.reset(new PlayerInventory(this, maxslots, res[0]["mesos"]));
	storage.reset(new PlayerStorage(this));

	// Skills
	skills.reset(new PlayerSkills(this));

	// Buffs/summons
	activeBuffs.reset(new PlayerActiveBuffs(this));
	summons.reset(new PlayerSummons(this));

	// Packet transferring on channel switch
	bool checked = PlayerDataProvider::Instance()->checkPlayer(id);
	if (checked) {
		PacketReader pack = PlayerDataProvider::Instance()->getPacket(id);

		setConnectionTime(pack.get<int64_t>());

		getActiveBuffs()->parseBuffTransferPacket(pack);
		if (getActiveBuffs()->hasHyperBody()) {
			int32_t skillid = getActiveBuffs()->getHyperBody();
			uint8_t hblevel = getActiveBuffs()->getActiveSkillLevel(skillid);
			SkillLevelInfo *hb = SkillDataProvider::Instance()->getSkill(skillid, hblevel);
			stats->setHyperBody(hb->x, hb->y);
		}

		getSummons()->parseSummonTransferPacket(pack);

		PlayerDataProvider::Instance()->removePacket(id);
	}
	else {
		// No packet, that means that they're connecting for the first time
		setConnectionTime(time(0));
	}

	// The rest
	variables.reset(new PlayerVariables(this));
	buddyList.reset(new PlayerBuddyList(this));
	quests.reset(new PlayerQuests(this));
	randStream.reset(new PlayerRandStream(this));
	monsterBook.reset(new PlayerMonsterBook(this));

	getMonsterBook()->setCover(res[0]["monsterbookcover"]);

	// Key Maps and Macros
	KeyMaps keyMaps;
	keyMaps.load(id);

	stats->checkHpMp(); // Adjust down HP or MP if necessary

	m_stance = 0;
	m_foothold = 0;

	PlayerPacket::connectData(this);

	if (CashServer::Instance()->getScrollingHeader().size() > 0) {
		ServerPacket::showScrollingHeader(this, CashServer::Instance()->getScrollingHeader());
	}

	PlayerPacket::showNX(this);
	PlayerPacket::showWishList(this);
	PlayerPacket::showCashInventory(this);
	PlayerPacket::showGiftItems(this);

	PlayerDataProvider::Instance()->addPlayer(this);

	std::stringstream x;
	x << getName() << " (" << getId() << ") connected from " << IpUtilities::ipToString(getIp());
	CashServer::Instance()->log(LogTypes::Info, x.str());

	setOnline(true);
	is_connect = true;
	SyncPacket::registerPlayer(CashServer::Instance()->getWorldConnection(), getIp(), id, name, map, stats->getJob(), stats->getLevel(), guildid, guildrank, allianceid, alliancerank);
}

void Player::setMap(int32_t mapid, PortalInfo *portal, bool instance) {
}

void Player::changeChannel(int8_t channel) {
	SyncPacket::playerChangeChannel(CashServer::Instance()->getWorldConnection(), this, channel);
}

void Player::changeKey(PacketReader &packet) {

}

void Player::changeSkillMacros(PacketReader &packet) {

}

void Player::setHair(int32_t id) {
	this->hair = id;
}

void Player::setEyes(int32_t id) {
	this->eyes = id;
}

void Player::setSkin(int8_t id) {
	this->skin = id;
}

bool Player::addWarning() {
	int32_t t = TimeUtilities::getTickCount();
	// Deleting old warnings
	for (size_t i = 0; i < warnings.size(); i++) {
		if (warnings[i] + 300000 < t) {
			warnings.erase(warnings.begin() + i);
			i--;
		}
	}
	warnings.push_back(t);
	if (warnings.size() > 50) {
		// Hacker - Temp DCing
		getSession()->disconnect();
		return true;
	}
	return false;
}

void Player::saveStats() {
	mysqlpp::Query query = Database::getCharDB().query();
	query << "UPDATE characters SET "
		<< "level = " << static_cast<int16_t>(stats->getLevel()) << "," // Queries have problems with int8_t due to being derived from ostream
		<< "job = " << stats->getJob() << ","
		<< "str = " << stats->getStr() << ","
		<< "dex = " << stats->getDex() << ","
		<< "`int` = " << stats->getInt() << ","
		<< "luk = " << stats->getLuk() << ","
		<< "chp = " << stats->getHp() << ","
		<< "mhp = " << stats->getMaxHp(true) << ","
		<< "cmp = " << stats->getMp() << ","
		<< "mmp = " << stats->getMaxMp(true) << ","
		<< "hpmp_ap = " << stats->getHpMpAp() << ","
		<< "ap = " << stats->getAp() << ","
		<< "sp = " << stats->getSp() << ","
		<< "exp = " << stats->getExp() << ","
		<< "fame = " << stats->getFame() << ","
		<< "map = " << map << ","
		<< "pos = " << static_cast<int16_t>(map_pos) << ","
		<< "gender = " << static_cast<int16_t>(gender) << ","
		<< "skin = " << static_cast<int16_t>(skin) << ","
		<< "eyes = " << eyes << ","
		<< "hair = " << hair << ","
		<< "mesos = " << inv->getMesos() << ","
		<< "equip_slots = " << static_cast<int16_t>(inv->getMaxSlots(Inventories::EquipInventory)) << ","
		<< "use_slots = " << static_cast<int16_t>(inv->getMaxSlots(Inventories::UseInventory)) << ","
		<< "setup_slots = " << static_cast<int16_t>(inv->getMaxSlots(Inventories::SetupInventory)) << ","
		<< "etc_slots = " << static_cast<int16_t>(inv->getMaxSlots(Inventories::EtcInventory)) << ","
		<< "cash_slots = " << static_cast<int16_t>(inv->getMaxSlots(Inventories::CashInventory)) << ","
		<< "buddylist_size = " << static_cast<int16_t>(buddylist_size) << ","
		<< "guildid = " << guildid << ","
		<< "guildrank = " << static_cast<int16_t>(guildrank) << ","
		<< "allianceid = " << allianceid << ","
		<< "alliancerank = " << static_cast<int16_t>(alliancerank) << ","
		<< "monsterbookcover = " << getMonsterBook()->getCover() 
		<< " WHERE id = " << id;
	query.exec();
}

void Player::saveAll(bool savecooldowns) {
	saveStats();
	getInventory()->save();
	getMonsterBook()->save();
	getMounts()->save();
	getPets()->save();
	getQuests()->save();
	getSkills()->save(savecooldowns);
	getStorage()->save();
	getVariables()->save();
}

void Player::setOnline(bool online) {
	int32_t onlineid = online ? CashServer::Instance()->getOnlineId() : 0;
	mysqlpp::Query query = Database::getCharDB().query();
	query << "UPDATE users INNER JOIN characters ON users.id = characters.userid SET users.online = " << onlineid <<
			", characters.online = " << online << " WHERE characters.id = " << id;
	query.exec();
}

void Player::setLevelDate() {
	mysqlpp::Query query = Database::getCharDB().query();
	query << "UPDATE characters SET time_level = NOW() WHERE characters.id = " << id;
	query.exec();
}

void Player::acceptDeath(bool wheel) {

}

bool Player::hasGmEquip() const {
	if (getInventory()->getEquippedId(EquipSlots::Helm) == Items::GmHat)
		return true;
	if (getInventory()->getEquippedId(EquipSlots::Top) == Items::GmTop)
		return true;
	if (getInventory()->getEquippedId(EquipSlots::Bottom) == Items::GmBottom)
		return true;
	if (getInventory()->getEquippedId(EquipSlots::Weapon) == Items::GmWeapon)
		return true;
	return false;
}

void Player::setBuddyListSize(uint8_t size) {
	buddylist_size = size;
}
