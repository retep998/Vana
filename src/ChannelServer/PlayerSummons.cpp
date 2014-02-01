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

auto PlayerSummons::addSummon(Summon *summon, int32_t time) -> void {
	bool puppet = GameLogicUtilities::isPuppet(summon->getSummonId());
	if (!puppet) {
		m_summon = summon;
	}
	else {
		m_puppet = summon;
	}
	Timer::Id id(Timer::Types::BuffTimer, summon->getSummonId(), 0);
	Timer::Timer::create([this, puppet](const time_point_t &now) { SummonHandler::removeSummon(m_player, puppet, false, SummonMessages::OutOfTime, true); },
		id, m_player->getTimerContainer(), seconds_t(time));
}

auto PlayerSummons::removeSummon(bool puppet, bool fromTimer) -> void {
	if (!puppet && m_summon != nullptr) {
		if (!fromTimer) {
			Timer::Id id(Timer::Types::BuffTimer, m_summon->getSummonId(), 0);
			m_player->getTimerContainer()->removeTimer(id);
		}
		delete m_summon;
		m_summon = nullptr;
	}
	else if (m_puppet != nullptr) {
		if (!fromTimer) {
			Timer::Id id(Timer::Types::BuffTimer, m_puppet->getSummonId(), 0);
			m_player->getTimerContainer()->removeTimer(id);
		}
		delete m_puppet;
		m_puppet = nullptr;
	}
}

auto PlayerSummons::getSummon(int32_t summonId) -> Summon * {
	if (m_summon != nullptr && m_summon->getId() == summonId) {
		return m_summon;
	}
	if (m_puppet != nullptr && m_puppet->getId() == summonId) {
		return m_puppet;
	}
	return nullptr;
}

auto PlayerSummons::getSummonTimeRemaining() const -> seconds_t {
	Timer::Id id(Timer::Types::BuffTimer, m_summon->getSummonId(), 0);
	return m_player->getTimerContainer()->getRemainingTime<seconds_t>(id);
}

auto PlayerSummons::write(PacketBuilder &builder) const -> void {
	int32_t summonId = 0;
	int32_t timeLeft = 0;
	uint8_t level = 0;
	if (m_summon != nullptr) {
		summonId = m_summon->getSummonId();
		timeLeft = static_cast<int32_t>(getSummonTimeRemaining().count());
		level = m_summon->getLevel();
	}
	builder.add<int32_t>(summonId);
	builder.add<int32_t>(timeLeft);
	builder.add<uint8_t>(level);
}

auto PlayerSummons::read(PacketReader &reader) -> void {
	int32_t skillId = reader.get<int32_t>();
	int32_t timeLeft = reader.get<int32_t>();
	uint8_t level = reader.get<uint8_t>();
	if (skillId != 0) {
		Summon *summon = new Summon(SummonHandler::loopId(), skillId, level);
		summon->setPos(m_player->getPos());
		addSummon(summon, timeLeft);
		m_player->sendMap(SummonsPacket::showSummon(m_player->getId(), summon, true));
	}
}