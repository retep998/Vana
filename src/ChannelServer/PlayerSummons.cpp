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
#include "PlayerSummons.h"
#include "GameConstants.h"
#include "GameLogicUtilities.h"
#include "PacketCreator.h"
#include "PacketReader.h"
#include "Player.h"
#include "Summon.h"
#include "SummonHandler.h"
#include "SummonsPacket.h"
#include "TimeUtilities.h"
#include "Timer.h"
#include <functional>

using std::tr1::bind;

void PlayerSummons::addSummon(Summon *summon, int32_t time) {
	bool puppet = GameLogicUtilities::isPuppet(summon->getSummonId());
	if (!puppet) {
		m_summon = summon;
	}
	else {
		m_puppet = summon;
	}
	Timer::Id id(Timer::Types::BuffTimer, summon->getSummonId(), 0);
	new Timer::Timer(bind(&SummonHandler::removeSummon, m_player, puppet, false, SummonMessages::OutOfTime, true),
		id, m_player->getTimers(), TimeUtilities::fromNow(time * 1000));
}

void PlayerSummons::removeSummon(bool puppet, bool fromTimer) {
	if (!puppet && m_summon != nullptr) {
		if (!fromTimer) {
			Timer::Id id(Timer::Types::BuffTimer, m_summon->getSummonId(), 0);
			m_player->getTimers()->removeTimer(id);
		}
		delete m_summon;
		m_summon = nullptr;
	}
	else if (m_puppet != nullptr) {
		if (!fromTimer) {
			Timer::Id id(Timer::Types::BuffTimer, m_puppet->getSummonId(), 0);
			m_player->getTimers()->removeTimer(id);
		}
		delete m_puppet;
		m_puppet = nullptr;
	}
}

Summon * PlayerSummons::getSummon(int32_t summonId) {
	if (m_summon != nullptr && m_summon->getId() == summonId) {
		return m_summon;
	}
	if (m_puppet != nullptr && m_puppet->getId() == summonId) {
		return m_puppet;
	}
	return nullptr;
}

int32_t PlayerSummons::getSummonTimeRemaining() {
	Timer::Id id(Timer::Types::BuffTimer, m_summon->getSummonId(), 0);
	return m_player->getTimers()->checkTimer(id);
}

void PlayerSummons::write(PacketCreator &packet) {
	int32_t summonId = 0;
	int32_t timeleft = 0;
	uint8_t level = 0;
	if (m_summon != nullptr) {
		summonId = m_summon->getSummonId();
		timeleft = getSummonTimeRemaining();
		level = m_summon->getLevel();
	}
	packet.add<int32_t>(summonId);
	packet.add<int32_t>(timeleft);
	packet.add<uint8_t>(level);
}

void PlayerSummons::read(PacketReader &packet) {
	int32_t skillId = packet.get<int32_t>();
	int32_t timeleft = packet.get<int32_t>();
	uint8_t level = packet.get<uint8_t>();
	if (skillId != 0) {
		Summon *summon = new Summon(SummonHandler::loopId(), skillId, level);
		summon->setPos(m_player->getPos());
		addSummon(summon, timeleft);
		SummonsPacket::showSummon(m_player, summon, true);
	}
}