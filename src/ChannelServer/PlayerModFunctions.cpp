/*
Copyright (C) 2008-2014 Vana Development Team

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
#include "SkillConstants.h"

auto PlayerModFunctions::disconnect(Player *player, const string_t &args) -> bool {
	player->getSession()->disconnect();
	return true;
}

auto PlayerModFunctions::save(Player *player, const string_t &args) -> bool {
	player->saveAll();
	ChatHandlerFunctions::showInfo(player, "Your progress has been saved");
	return true;
}

auto PlayerModFunctions::modMesos(Player *player, const string_t &args) -> bool {
	if (args.length() != 0) {
		player->getInventory()->setMesos(atoi(args.c_str()));
		return true;
	}
	return false;
}

auto PlayerModFunctions::heal(Player *player, const string_t &args) -> bool {
	player->getActiveBuffs()->useDispel();
	player->getActiveBuffs()->removeDebuff(MobSkills::Seduce);
	player->getActiveBuffs()->removeDebuff(MobSkills::CrazySkull);
	player->getStats()->setHp(player->getStats()->getMaxHp());
	player->getStats()->setMp(player->getStats()->getMaxMp());
	return true;
}

auto PlayerModFunctions::modStr(Player *player, const string_t &args) -> bool {
	if (args.length() != 0) {
		player->getStats()->setStr(atoi(args.c_str()));
		return true;
	}
	return false;
}

auto PlayerModFunctions::modDex(Player *player, const string_t &args) -> bool {
	if (args.length() != 0) {
		player->getStats()->setDex(atoi(args.c_str()));
		return true;
	}
	return false;
}

auto PlayerModFunctions::modInt(Player *player, const string_t &args) -> bool {
	if (args.length() != 0) {
		player->getStats()->setInt(atoi(args.c_str()));
		return true;
	}
	return false;
}

auto PlayerModFunctions::modLuk(Player *player, const string_t &args) -> bool {
	if (args.length() != 0) {
		player->getStats()->setLuk(atoi(args.c_str()));
		return true;
	}
	return false;
}

auto PlayerModFunctions::maxStats(Player *player, const string_t &args) -> bool {
	player->getStats()->setFame(Stats::MaxFame);
	player->getStats()->setMaxHp(Stats::MaxMaxHp);
	player->getStats()->setMaxMp(Stats::MaxMaxMp);
	player->getStats()->setStr(32767);
	player->getStats()->setDex(32767);
	player->getStats()->setInt(32767);
	player->getStats()->setLuk(32767);
	return true;
}

auto PlayerModFunctions::hp(Player *player, const string_t &args) -> bool {
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

auto PlayerModFunctions::mp(Player *player, const string_t &args) -> bool {
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

auto PlayerModFunctions::sp(Player *player, const string_t &args) -> bool {
	if (args.length() != 0) {
		player->getStats()->setSp(atoi(args.c_str()));
		return true;
	}
	return false;
}

auto PlayerModFunctions::ap(Player *player, const string_t &args) -> bool {
	if (args.length() != 0) {
		player->getStats()->setAp(atoi(args.c_str()));
		return true;
	}
	return false;
}

auto PlayerModFunctions::fame(Player *player, const string_t &args) -> bool {
	if (args.length() != 0) {
		player->getStats()->setFame(atoi(args.c_str()));
		return true;
	}
	return false;
}

auto PlayerModFunctions::level(Player *player, const string_t &args) -> bool {
	if (args.length() != 0) {
		player->getStats()->setLevel(atoi(args.c_str()));
		return true;
	}
	return false;
}

auto PlayerModFunctions::job(Player *player, const string_t &args) -> bool {
	if (args.length() != 0) {
		int16_t job = ChatHandlerFunctions::getJob(args);
		if (job >= 0) {
			player->getStats()->setJob(job);
		}
		else {
			ChatHandlerFunctions::showError(player, "Invalid job: " + args);
		}
	}
	else {
		out_stream_t message;
		message << "Current job: " << player->getStats()->getJob();
		ChatHandlerFunctions::showInfo(player, message.str());
	}
	return true;
}

auto PlayerModFunctions::addSp(Player *player, const string_t &args) -> bool {
	match_t matches;
	if (ChatHandlerFunctions::runRegexPattern(args, R"((\d+) ?(-{0,1}\d+)?)", matches)) {
		string_t rawSkill = matches[1];
		int32_t skillId = atoi(rawSkill.c_str());
		if (SkillDataProvider::getInstance().isValidSkill(skillId)) {
			// Don't allow skills that do not exist to be added
			string_t countString = matches[2];
			uint8_t count = countString.length() > 0 ? atoi(countString.c_str()) : 1;

			player->getSkills()->addSkillLevel(skillId, count);
		}
		else {
			ChatHandlerFunctions::showError(player, "Invalid skill: " + rawSkill);
		}
		return true;
	}
	return false;
}

auto PlayerModFunctions::maxSp(Player *player, const string_t &args) -> bool {
	match_t matches;
	if (ChatHandlerFunctions::runRegexPattern(args, R"((\d+) ?(-{0,1}\d+)?)", matches)) {
		string_t rawSkill = matches[1];
		int32_t skillId = atoi(rawSkill.c_str());
		if (SkillDataProvider::getInstance().isValidSkill(skillId)) {
			// Don't allow skills that do not exist to be added
			string_t max = matches[2];
			uint8_t maxLevel = max.length() > 0 ? atoi(max.c_str()) : 1;

			player->getSkills()->setMaxSkillLevel(skillId, maxLevel);
		}
		else {
			ChatHandlerFunctions::showError(player, "Invalid skill: " + rawSkill);
		}
		return true;
	}
	return false;
}