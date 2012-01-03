/*
Copyright (C) 2008-2012 Vana Development Team

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
#include "PlayerModFunctions.h"
#include "Maps.h"
#include "Player.h"
#include "PlayerPacket.h"
#include "PlayerDataProvider.h"
#include <boost/lexical_cast.hpp>

using boost::lexical_cast;

bool PlayerModFunctions::disconnect(Player *player, const string &args) {
	player->getSession()->disconnect();
	return true;
}

bool PlayerModFunctions::save(Player *player, const string &args) {
	player->saveAll();
	PlayerPacket::showMessage(player, "Your progress has been saved.", PlayerPacket::NoticeTypes::Blue);
	return true;
}

bool PlayerModFunctions::modMesos(Player *player, const string &args) {
	if (args.length() != 0) {
		player->getInventory()->setMesos(atoi(args.c_str()));
		return true;
	}
	return false;
}

bool PlayerModFunctions::heal(Player *player, const string &args) {
	player->getStats()->setHp(player->getStats()->getMaxHp());
	player->getStats()->setMp(player->getStats()->getMaxMp());
	return true;
}

bool PlayerModFunctions::modStr(Player *player, const string &args) {
	if (args.length() != 0) {
		player->getStats()->setStr(atoi(args.c_str()));
		return true;
	}
	return false;
}

bool PlayerModFunctions::modDex(Player *player, const string &args) {
	if (args.length() != 0) {
		player->getStats()->setDex(atoi(args.c_str()));
		return true;
	}
	return false;
}

bool PlayerModFunctions::modInt(Player *player, const string &args) {
	if (args.length() != 0) {
		player->getStats()->setInt(atoi(args.c_str()));
		return true;
	}
	return false;
}

bool PlayerModFunctions::modLuk(Player *player, const string &args) {
	if (args.length() != 0) {
		player->getStats()->setLuk(atoi(args.c_str()));
		return true;
	}
	return false;
}

bool PlayerModFunctions::maxStats(Player *player, const string &args) {
	player->getStats()->setFame(Stats::MaxFame);
	player->getStats()->setMaxHp(Stats::MaxMaxHp);
	player->getStats()->setMaxMp(Stats::MaxMaxMp);
	player->getStats()->setStr(32767);
	player->getStats()->setDex(32767);
	player->getStats()->setInt(32767);
	player->getStats()->setLuk(32767);
	return true;
}

bool PlayerModFunctions::hp(Player *player, const string &args) {
	if (args.length() != 0) {
		uint16_t amount = atoi(args.c_str());
		player->getStats()->setMaxHp(amount);
		if (player->getStats()->getHp() > amount) {
			player->getStats()->setHp(player->getStats()->getMaxHp());
		}
		return true;
	}
	return false;
}

bool PlayerModFunctions::mp(Player *player, const string &args) {
	if (args.length() != 0) {
		uint16_t amount = atoi(args.c_str());
		player->getStats()->setMaxMp(amount);
		if (player->getStats()->getMp() > amount) {
			player->getStats()->setMp(player->getStats()->getMaxMp());
		}
		return true;
	}
	return false;
}

bool PlayerModFunctions::sp(Player *player, const string &args) {
	if (args.length() != 0) {
		player->getStats()->setSp(atoi(args.c_str()));
		return true;
	}
	return false;
}

bool PlayerModFunctions::ap(Player *player, const string &args) {
	if (args.length() != 0) {
		player->getStats()->setAp(atoi(args.c_str()));
		return true;
	}
	return false;
}

bool PlayerModFunctions::fame(Player *player, const string &args) {
	if (args.length() != 0) {
		player->getStats()->setFame(atoi(args.c_str()));
		return true;
	}
	return false;
}

bool PlayerModFunctions::level(Player *player, const string &args) {
	if (args.length() != 0) {
		player->getStats()->setLevel(atoi(args.c_str()));
		return true;
	}
	return false;
}

bool PlayerModFunctions::job(Player *player, const string &args) {
	if (args.length() != 0) {
		int16_t job = ChatHandlerFunctions::getJob(args);
		if (job >= 0) {
			player->getStats()->setJob(job);
		}
		else {
			PlayerPacket::showMessage(player, "Invalid job.", PlayerPacket::NoticeTypes::Red);
		}
	}
	else {
		std::ostringstream message;
		message << "Current Job: " << player->getStats()->getJob();
		PlayerPacket::showMessage(player, message.str(), PlayerPacket::NoticeTypes::Blue);
	}
	return true;
}

bool PlayerModFunctions::addSp(Player *player, const string &args) {
	cmatch matches;
	if (ChatHandlerFunctions::runRegexPattern(args, "(\\d+) ?(-{0,1}\\d+)?", matches)) {
		int32_t skillId = atoi(string(matches[1]).c_str());
		if (SkillDataProvider::Instance()->isSkill(skillId)) {
			// Don't allow skills that do not exist to be added
			string countString = matches[2];
			uint8_t count = countString.length() > 0 ? atoi(countString.c_str()) : 1;

			player->getSkills()->addSkillLevel(skillId, count);
		}
		else {
			PlayerPacket::showMessage(player, "Invalid Skill ID.", PlayerPacket::NoticeTypes::Red);
		}
		return true;
	}
	return false;
}