#include "PlayerStats.h"
#include "ChannelServer.h"
#include "Database.h"
#include "GameConstants.h"
#include "Instance.h"
#include "InstanceMessageConstants.h"
#include "LevelsPacket.h"
#include "PacketCreator.h"
#include "Party.h"
#include "Player.h"
#include "PlayerPacket.h"
#include "Summons.h"
#include "WorldServerConnectPacket.h"

PlayerStats::PlayerStats(Player *p) :
m_player(p),
m_gear_hp(0),
m_gear_mp(0),
m_gear_str(0),
m_gear_dex(0),
m_gear_int(0),
m_gear_luk(0)
{
	load();
}

void PlayerStats::load() {
	mysqlpp::Query query = Database::getCharDB().query();
	query << "SELECT characters.* FROM characters WHERE characters.id = " << m_player->getId();
	mysqlpp::StoreQueryResult res = query.store();

	m_exp = res[0]["exp"];
	m_eyes = res[0]["eyes"];
	m_hair = res[0]["hair"];
	m_gender = static_cast<int8_t>(res[0]["gender"]);
	m_skin = static_cast<int8_t>(res[0]["skin"]);
	m_level = static_cast<uint8_t>(res[0]["level"]);
	m_job = static_cast<int16_t>(res[0]["job"]);
	m_str = static_cast<int16_t>(res[0]["str"]);
	m_dex = static_cast<int16_t>(res[0]["dex"]);
	m_int = static_cast<int16_t>(res[0]["int"]);
	m_luk = static_cast<int16_t>(res[0]["luk"]);
	m_hp = static_cast<int16_t>(res[0]["chp"]);
	m_rmhp = m_mhp = static_cast<int16_t>(res[0]["mhp"]);
	m_mp = static_cast<int16_t>(res[0]["cmp"]);
	m_rmmp = m_mmp = static_cast<int16_t>(res[0]["mmp"]);
	m_ap = static_cast<int16_t>(res[0]["ap"]);
	m_sp = static_cast<int16_t>(res[0]["sp"]);
	m_fame = static_cast<int16_t>(res[0]["fame"]);
	m_hpmp_ap = static_cast<uint16_t>(res[0]["hpmp_ap"]);
}

void PlayerStats::save() {
	mysqlpp::Query query = Database::getCharDB().query();
	query << "UPDATE characters SET "
		<< "level = " << static_cast<int16_t>(m_level) << "," // Queries have problems with int8_t due to being derived from ostream
		<< "job = " << m_job << ","
		<< "str = " << m_str << ","
		<< "dex = " << m_dex << ","
		<< "`int` = " << m_int << ","
		<< "luk = " << m_luk << ","
		<< "chp = " << m_hp << ","
		<< "mhp = " << m_rmhp << ","
		<< "cmp = " << m_mp << ","
		<< "mmp = " << m_rmmp << ","
		<< "hpmp_ap = " << m_hpmp_ap << ","
		<< "ap = " << m_ap << ","
		<< "sp = " << m_sp << ","
		<< "exp = " << m_exp << ","
		<< "fame = " << m_fame << ","
		<< "gender = " << static_cast<int16_t>(m_gender) << ","
		<< "skin = " << static_cast<int16_t>(m_skin) << ","
		<< "eyes = " << m_eyes << ","
		<< "hair = " << m_hair << " "
		<< "WHERE id = " << m_player->getId();
	query.exec();
}

void PlayerStats::connectData(PacketCreator &packet) {
	packet.add<int8_t>(getGender());
	packet.add<int8_t>(getSkin());
	packet.add<int32_t>(getEyes());
	packet.add<int32_t>(getHair());
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int8_t>(getLevel());
	packet.add<int16_t>(getJob());
	packet.add<int16_t>(getBaseStat(Stats::Str));
	packet.add<int16_t>(getBaseStat(Stats::Dex));
	packet.add<int16_t>(getBaseStat(Stats::Int));
	packet.add<int16_t>(getBaseStat(Stats::Luk));
	packet.add<int16_t>(getHp());
	packet.add<int16_t>(getRMHp());
	packet.add<int16_t>(getMp());
	packet.add<int16_t>(getRMMp());
	packet.add<int16_t>(getBaseStat(Stats::Ap));
	packet.add<int16_t>(getBaseStat(Stats::Sp));
	packet.add<int32_t>(getExp());
	packet.add<int16_t>(getBaseStat(Stats::Fame));
}

void PlayerStats::setHp(int16_t shp, bool is) {
	if (shp < 0)
		m_hp = 0;
	else if (shp > m_mhp)
		m_hp = m_mhp;
	else
		m_hp = shp;
	if (is) {
		PlayerPacket::updateStatShort(m_player, Stats::Hp, m_hp);
		modifiedHp();
	}
}

void PlayerStats::modifyHp(int16_t nhp, bool is) {
	if ((m_hp + nhp) < 0)
		m_hp = 0;
	else if ((m_hp + nhp) > m_mhp)
		m_hp = m_mhp;
	else
		m_hp = (m_hp + nhp);
	if (is)
		PlayerPacket::updateStatShort(m_player, Stats::Hp, m_hp);
	modifiedHp();
}

void PlayerStats::damageHp(uint16_t dhp) {
	m_hp = (dhp > m_hp ? 0 : m_hp - dhp);
	PlayerPacket::updateStatShort(m_player, Stats::Hp, m_hp);
	modifiedHp();
}

void PlayerStats::setMp(int16_t smp, bool is) {
	if (!m_player->getActiveBuffs()->hasInfinity()) {
		if (smp < 0)
			m_mp = 0;
		else if (smp > m_mmp)
			m_mp = m_mmp;
		else
			m_mp = smp;
	}
	PlayerPacket::updateStatShort(m_player, Stats::Mp, m_mp, is);
}

void PlayerStats::modifyMp(int16_t nmp, bool is) {
	if (!m_player->getActiveBuffs()->hasInfinity()) {
		if ((m_mp + nmp) < 0)
			m_mp = 0;
		else if ((m_mp + nmp) > m_mmp)
			m_mp = m_mmp;
		else
			m_mp = (m_mp + nmp);
	}
	PlayerPacket::updateStatShort(m_player, Stats::Mp, m_mp, is);
}

void PlayerStats::damageMp(uint16_t dmp) {
	if (!m_player->getActiveBuffs()->hasInfinity()) {
		m_mp = (dmp > m_mp ? 0 : m_mp - dmp);
	}
	PlayerPacket::updateStatShort(m_player, Stats::Mp, m_mp, false);
}

void PlayerStats::setJob(int16_t job) {
	m_job = job;
	PlayerPacket::updateStatShort(m_player, Stats::Job, job);
	LevelsPacket::jobChange(m_player);
	WorldServerConnectPacket::updateJob(ChannelServer::Instance()->getWorldPlayer(), m_player->getId(), job);
}

void PlayerStats::setMHp(int16_t mhp) {
	if (mhp > Stats::MaxMaxHp)
		mhp = Stats::MaxMaxHp;
	else if (mhp < Stats::MinMaxHp)
		mhp = Stats::MinMaxHp;
	m_mhp = mhp;
	PlayerPacket::updateStatShort(m_player, Stats::MaxHp, m_rmhp);
	modifiedHp();
}

void PlayerStats::setMMp(int16_t mmp) {
	if (mmp > Stats::MaxMaxMp)
		mmp = Stats::MaxMaxMp;
	else if (mmp < Stats::MinMaxMp)
		mmp = Stats::MinMaxMp;
	m_mmp = mmp;
	PlayerPacket::updateStatShort(m_player, Stats::MaxMp, m_rmmp);
}

void PlayerStats::setHyperBody(int16_t modx, int16_t mody) {
	modx += 100;
	mody += 100;
	m_mhp = (((m_rmhp + m_gear_hp) * modx / 100) > Stats::MaxMaxHp ? Stats::MaxMaxHp : (m_rmhp + m_gear_hp) * modx / 100);
	m_mmp = (((m_rmmp + m_gear_mp) * mody / 100) > Stats::MaxMaxMp ? Stats::MaxMaxMp : (m_rmmp + m_gear_mp) * mody / 100);
	PlayerPacket::updateStatShort(m_player, Stats::MaxHp, m_rmhp);
	PlayerPacket::updateStatShort(m_player, Stats::MaxMp, m_rmmp);
	if (m_player->getParty())
		m_player->getParty()->showHpBar(m_player);
	m_player->getActiveBuffs()->checkBerserk();
}

void PlayerStats::setRMHp(int16_t rmhp) {
	if (rmhp > Stats::MaxMaxHp)
		rmhp = Stats::MaxMaxHp;
	else if (rmhp < Stats::MinMaxHp)
		rmhp = Stats::MinMaxHp;
	m_rmhp = rmhp;
	PlayerPacket::updateStatShort(m_player, Stats::MaxHp, rmhp);
}

void PlayerStats::setRMMp(int16_t rmmp) {
	if (rmmp > Stats::MaxMaxMp)
		rmmp = Stats::MaxMaxMp;
	else if (rmmp < Stats::MinMaxMp)
		rmmp = Stats::MinMaxMp;
	m_rmmp = rmmp;
	PlayerPacket::updateStatShort(m_player, Stats::MaxMp, rmmp);
}

void PlayerStats::modifyRMHp(int16_t mod) {
	m_rmhp = (((m_rmhp + mod) > Stats::MaxMaxHp) ? Stats::MaxMaxHp : (m_rmhp + mod));
	PlayerPacket::updateStatShort(m_player, Stats::MaxHp, m_rmhp);
}

void PlayerStats::modifyRMMp(int16_t mod) {
	m_rmmp = (((m_rmmp + mod) > Stats::MaxMaxMp) ? Stats::MaxMaxMp : (m_rmmp + mod));
	PlayerPacket::updateStatShort(m_player, Stats::MaxMp, m_rmmp);
}

void PlayerStats::setExp(int32_t exp) {
	if (exp < 0)
		exp = 0;
	m_exp = exp;
	PlayerPacket::updateStatInt(m_player, Stats::Exp, exp);
}

void PlayerStats::setLevel(uint8_t level) {
	m_level = level;
	PlayerPacket::updateStatShort(m_player, Stats::Level, level);
	LevelsPacket::levelUp(m_player);
	WorldServerConnectPacket::updateLevel(ChannelServer::Instance()->getWorldPlayer(), m_player->getId(), level);
}
void PlayerStats::setHair(int32_t id) {
	m_hair = id;
	PlayerPacket::updateStatInt(m_player, Stats::Hair, id);
}

void PlayerStats::setEyes(int32_t id) {
	m_eyes = id;
	PlayerPacket::updateStatInt(m_player, Stats::Eyes, id);
}

void PlayerStats::setSkin(int8_t id) {
	m_skin = id;
	PlayerPacket::updateStatInt(m_player, Stats::Skin, id);
}

void PlayerStats::setFame(int16_t fame) {
	if (fame < Stats::MinFame)
		fame = Stats::MinFame;
	else if (fame > Stats::MaxFame)
		fame = Stats::MaxFame;
	m_fame = fame;
	PlayerPacket::updateStatInt(m_player, Stats::Fame, fame);
}

int16_t PlayerStats::getBaseStat(int32_t stat) const {
	int16_t ret = 0;
	switch (stat) {
		case Stats::Ap:
			ret = m_ap;
			break;
		case Stats::Sp:
			ret = m_sp;
			break;
		case Stats::Str:
			ret = m_str;
			break;
		case Stats::Dex:
			ret = m_dex;
			break;
		case Stats::Int:
			ret = m_int;
			break;
		case Stats::Luk:
			ret = m_luk;
			break;
		case Stats::Fame:
			ret = m_fame;
			break;
	}
	return ret;
}

int16_t PlayerStats::getGearStat(int32_t stat) const {
	int16_t ret = 0;
	switch (stat) {
		case Stats::Str:
			ret = m_gear_str;
			break;
		case Stats::Dex:
			ret = m_gear_dex;
			break;
		case Stats::Int:
			ret = m_gear_int;
			break;
		case Stats::Luk:
			ret = m_gear_luk;
			break;
		case Stats::MaxHp:
			ret = m_gear_hp;
			break;
		case Stats::MaxMp:
			ret = m_gear_mp;
			break;
	}
	return ret;
}

void PlayerStats::setGearStat(int32_t stat, int16_t amount, bool firstLoad) {
	switch (stat) {
		case Stats::Str:
			m_gear_str = amount;
			break;
		case Stats::Dex:
			m_gear_dex = amount;
			break;
		case Stats::Int:
			m_gear_int = amount;
			break;
		case Stats::Luk:
			m_gear_luk = amount;
			break;
		case Stats::MaxHp:
			m_mhp = m_rmhp;
			m_gear_hp = amount;
			if (m_gear_hp + amount > Stats::MaxMaxHp)
				m_mhp = Stats::MaxMaxHp;
			else
				m_mhp += amount;
			if (m_hp > m_mhp && !firstLoad)
				setHp(m_mhp);
			break;
		case Stats::MaxMp:
			m_mmp = m_rmmp;
			m_gear_mp = amount;
			if (m_gear_mp + amount > Stats::MaxMaxMp)
				m_mmp = Stats::MaxMaxMp;
			else
				m_mmp += amount;
			if (m_mp > m_mmp && !firstLoad)
				setMp(m_mmp);
			break;
	}
}

void PlayerStats::setBaseStat(int32_t stat, int16_t amount, bool showPacket) {
	switch (stat) {
		case Stats::Ap:
			m_ap = amount;
			break;
		case Stats::Sp:
			m_sp = amount;
			break;
		case Stats::Str:
			m_str = amount;
			break;
		case Stats::Dex:
			m_dex = amount;
			break;
		case Stats::Int:
			m_int = amount;
			break;
		case Stats::Luk:
			m_luk = amount;
			break;
	}
	if (showPacket) {
		PlayerPacket::updateStatShort(m_player, stat, amount);
	}
}

int16_t PlayerStats::getBuffStat(int32_t stat) const {
	int16_t ret = 0;
	switch (stat) {
		case Stats::Str:
		case Stats::Dex:
		case Stats::Int:
		case Stats::Luk:
			ret = getBaseStat(stat) * m_player->getActiveBuffs()->getMapleWarriorValue() / 100;
			break;
	}
	return ret;
}

int16_t PlayerStats::getTotalStat(int32_t stat) const {
	int16_t ret = 0;
	switch (stat) {
		case Stats::MaxHp:
		case Stats::MaxMp:
		case Stats::Str:
		case Stats::Dex:
		case Stats::Int:
		case Stats::Luk:
			ret = getBaseStat(stat) + getGearStat(stat) + getBuffStat(stat);
			break;
	}
	return ret;
}

void PlayerStats::modifiedHp() {
	if (m_player->getParty())
		m_player->getParty()->showHpBar(m_player);
	m_player->getActiveBuffs()->checkBerserk();
	if (m_hp == 0) {
		if (m_player->getInstance() != 0) {
			m_player->getInstance()->sendMessage(PlayerDeath, m_player->getId());
		}
		m_player->loseExp();
		Summons::removeSummon(m_player, false, true, false, 2);
	}
}