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
#include "PlayerSummons.hpp"
#include "Algorithm.hpp"
#include "GameConstants.hpp"
#include "GameLogicUtilities.hpp"
#include "Map.hpp"
#include "PacketReader.hpp"
#include "Player.hpp"
#include "Summon.hpp"
#include "SummonHandler.hpp"
#include "SummonsPacket.hpp"
#include "Timer.hpp"
#include "TimeUtilities.hpp"
#include <functional>

auto PlayerSummons::addSummon(Summon *summon, seconds_t time) -> void {
	summon_id_t summonId = summon->getId();
	Timer::Id id(Timer::Types::BuffTimer, summonId, 1);
	Timer::Timer::create([this, summonId](const time_point_t &now) { SummonHandler::removeSummon(m_player, summonId, false, SummonMessages::OutOfTime, true); },
		id, m_player->getTimerContainer(), time);

	m_summons.push_back(summon);
}

auto PlayerSummons::removeSummon(summon_id_t summonId, bool fromTimer) -> void {
	Summon *summon = getSummon(summonId);
	if (summon != nullptr) {
		if (!fromTimer) {
			Timer::Id id(Timer::Types::BuffTimer, summonId, 1);
			m_player->getTimerContainer()->removeTimer(id);
		}
		ext::remove_element(m_summons, summon);
		delete summon;
	}
}

auto PlayerSummons::getSummon(summon_id_t summonId) -> Summon * {
	for (auto &summon : m_summons) {
		if (summon->getId() == summonId) {
			return summon;
		}
	}
	return nullptr;
}

auto PlayerSummons::forEach(function_t<void(Summon *)> func) -> void {
	auto copy = m_summons;
	for (auto &summon : copy) {
		func(summon);
	}
}

auto PlayerSummons::changedMap() -> void {
	auto copy = m_summons;
	for (auto &summon : copy) {
		if (summon->getMovementType() == Summon::Static) {
			SummonHandler::removeSummon(m_player, summon->getId(), false, SummonMessages::None);
		}
	}
}

auto PlayerSummons::getSummonTimeRemaining(summon_id_t summonId) const -> seconds_t {
	Timer::Id id(Timer::Types::BuffTimer, summonId, 1);
	return m_player->getTimerContainer()->getRemainingTime<seconds_t>(id);
}

auto PlayerSummons::getTransferPacket() const -> PacketBuilder {
	PacketBuilder builder;
	builder.add<uint8_t>(m_summons.size());
	if (m_summons.size() > 0) {
		for (const auto &summon : m_summons) {
			if (summon->getMovementType() == Summon::Static) {
				continue;
			}
			builder.add<skill_id_t>(summon->getSkillId());
			builder.add<seconds_t>(getSummonTimeRemaining(summon->getId()));
			builder.add<skill_level_t>(summon->getSkillLevel());
		}
	}
	return builder;
}

auto PlayerSummons::parseTransferPacket(PacketReader &reader) -> void {
	uint8_t size = reader.get<uint8_t>();
	if (size > 0) {
		for (uint8_t i = 0; i < size; ++i) {
			skill_id_t skillId = reader.get<skill_id_t>();
			seconds_t timeLeft = reader.get<seconds_t>();
			skill_level_t level = reader.get<skill_level_t>();

			Summon *summon = new Summon(SummonHandler::loopId(), skillId, level, m_player->isFacingLeft(), m_player->getPos());
			summon->setPos(m_player->getPos());
			addSummon(summon, timeLeft);
			m_player->sendMap(SummonsPacket::showSummon(m_player->getId(), summon, false));
		}
	}
}