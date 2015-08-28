/*
Copyright (C) 2008-2015 Vana Development Team

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
#include "PlayerModFunctions.hpp"
#include "ChannelServer.hpp"
#include "Maps.hpp"
#include "Player.hpp"
#include "PlayerPacket.hpp"
#include "PlayerDataProvider.hpp"
#include "SkillConstants.hpp"

namespace Vana {

auto PlayerModFunctions::disconnect(Player *player, const chat_t &args) -> ChatResult {
	player->disconnect();
	return ChatResult::HandledDisplay;
}

auto PlayerModFunctions::save(Player *player, const chat_t &args) -> ChatResult {
	player->saveAll();
	ChatHandlerFunctions::showInfo(player, "Your progress has been saved");
	return ChatResult::HandledDisplay;
}

auto PlayerModFunctions::modMesos(Player *player, const chat_t &args) -> ChatResult {
	if (!args.empty()) {
		player->getInventory()->setMesos(atoi(args.c_str()));
		return ChatResult::HandledDisplay;
	}
	return ChatResult::ShowSyntax;
}

auto PlayerModFunctions::heal(Player *player, const chat_t &args) -> ChatResult {
	player->getActiveBuffs()->usePlayerDispel();
	player->getActiveBuffs()->removeDebuff(MobSkills::Seduce);
	player->getActiveBuffs()->removeDebuff(MobSkills::CrazySkull);
	player->getStats()->setHp(player->getStats()->getMaxHp());
	player->getStats()->setMp(player->getStats()->getMaxMp());
	return ChatResult::HandledDisplay;
}

auto PlayerModFunctions::modStr(Player *player, const chat_t &args) -> ChatResult {
	if (!args.empty()) {
		player->getStats()->setStr(atoi(args.c_str()));
		return ChatResult::HandledDisplay;
	}
	return ChatResult::ShowSyntax;
}

auto PlayerModFunctions::modDex(Player *player, const chat_t &args) -> ChatResult {
	if (!args.empty()) {
		player->getStats()->setDex(atoi(args.c_str()));
		return ChatResult::HandledDisplay;
	}
	return ChatResult::ShowSyntax;
}

auto PlayerModFunctions::modInt(Player *player, const chat_t &args) -> ChatResult {
	if (!args.empty()) {
		player->getStats()->setInt(atoi(args.c_str()));
		return ChatResult::HandledDisplay;
	}
	return ChatResult::ShowSyntax;
}

auto PlayerModFunctions::modLuk(Player *player, const chat_t &args) -> ChatResult {
	if (!args.empty()) {
		player->getStats()->setLuk(atoi(args.c_str()));
		return ChatResult::HandledDisplay;
	}
	return ChatResult::ShowSyntax;
}

auto PlayerModFunctions::maxStats(Player *player, const chat_t &args) -> ChatResult {
	player->getStats()->setFame(Stats::MaxFame);
	player->getStats()->setMaxHp(Stats::MaxMaxHp);
	player->getStats()->setMaxMp(Stats::MaxMaxMp);
	auto max = std::numeric_limits<stat_t>::max();
	player->getStats()->setStr(max);
	player->getStats()->setDex(max);
	player->getStats()->setInt(max);
	player->getStats()->setLuk(max);
	return ChatResult::HandledDisplay;
}

auto PlayerModFunctions::hp(Player *player, const chat_t &args) -> ChatResult {
	if (!args.empty()) {
		health_t amount = atoi(args.c_str());
		player->getStats()->setMaxHp(amount);
		if (player->getStats()->getHp() > amount) {
			player->getStats()->setHp(player->getStats()->getMaxHp());
		}
		return ChatResult::HandledDisplay;
	}
	return ChatResult::ShowSyntax;
}

auto PlayerModFunctions::mp(Player *player, const chat_t &args) -> ChatResult {
	if (!args.empty()) {
		health_t amount = atoi(args.c_str());
		player->getStats()->setMaxMp(amount);
		if (player->getStats()->getMp() > amount) {
			player->getStats()->setMp(player->getStats()->getMaxMp());
		}
		return ChatResult::HandledDisplay;
	}
	return ChatResult::ShowSyntax;
}

auto PlayerModFunctions::sp(Player *player, const chat_t &args) -> ChatResult {
	if (!args.empty()) {
		player->getStats()->setSp(atoi(args.c_str()));
		return ChatResult::HandledDisplay;
	}
	return ChatResult::ShowSyntax;
}

auto PlayerModFunctions::ap(Player *player, const chat_t &args) -> ChatResult {
	if (!args.empty()) {
		player->getStats()->setAp(atoi(args.c_str()));
		return ChatResult::HandledDisplay;
	}
	return ChatResult::ShowSyntax;
}

auto PlayerModFunctions::fame(Player *player, const chat_t &args) -> ChatResult {
	if (!args.empty()) {
		player->getStats()->setFame(atoi(args.c_str()));
		return ChatResult::HandledDisplay;
	}
	return ChatResult::ShowSyntax;
}

auto PlayerModFunctions::level(Player *player, const chat_t &args) -> ChatResult {
	if (!args.empty()) {
		player->getStats()->setLevel(atoi(args.c_str()));
		return ChatResult::HandledDisplay;
	}
	return ChatResult::ShowSyntax;
}

auto PlayerModFunctions::job(Player *player, const chat_t &args) -> ChatResult {
	if (!args.empty()) {
		job_id_t job = ChatHandlerFunctions::getJob(args);
		if (job >= 0) {
			player->getStats()->setJob(job);
		}
		else {
			ChatHandlerFunctions::showError(player, "Invalid job: " + args);
		}
	}
	else {
		ChatHandlerFunctions::showInfo(player, [&](out_stream_t &message) {
			message << "Current job: " << player->getStats()->getJob();
		});
	}
	return ChatResult::HandledDisplay;
}

auto PlayerModFunctions::addSp(Player *player, const chat_t &args) -> ChatResult {
	match_t matches;
	if (ChatHandlerFunctions::runRegexPattern(args, R"((\d+) ?(-{0,1}\d+)?)", matches) == MatchResult::AnyMatches) {
		string_t rawSkill = matches[1];
		skill_id_t skillId = atoi(rawSkill.c_str());
		if (ChannelServer::getInstance().getSkillDataProvider().isValidSkill(skillId)) {
			// Don't allow skills that do not exist to be added
			string_t countString = matches[2];
			skill_level_t count = countString.empty() ? 1 : atoi(countString.c_str());

			player->getSkills()->addSkillLevel(skillId, count);
		}
		else {
			ChatHandlerFunctions::showError(player, "Invalid skill: " + rawSkill);
		}
		return ChatResult::HandledDisplay;
	}
	return ChatResult::ShowSyntax;
}

auto PlayerModFunctions::maxSp(Player *player, const chat_t &args) -> ChatResult {
	match_t matches;
	if (ChatHandlerFunctions::runRegexPattern(args, R"((\d+) ?(-{0,1}\d+)?)", matches) == MatchResult::AnyMatches) {
		string_t rawSkill = matches[1];
		skill_id_t skillId = atoi(rawSkill.c_str());
		if (ChannelServer::getInstance().getSkillDataProvider().isValidSkill(skillId)) {
			// Don't allow skills that do not exist to be added
			string_t max = matches[2];
			skill_level_t maxLevel = max.empty() ? 1 : atoi(max.c_str());

			player->getSkills()->setMaxSkillLevel(skillId, maxLevel);
		}
		else {
			ChatHandlerFunctions::showError(player, "Invalid skill: " + rawSkill);
		}
		return ChatResult::HandledDisplay;
	}
	return ChatResult::ShowSyntax;
}

}