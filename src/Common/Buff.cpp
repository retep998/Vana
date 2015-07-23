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
#include "Buff.hpp"
#include "Algorithm.hpp"

Buff::Buff()
{
}

Buff::Buff(const init_list_t<BuffInfo> &buffInfo) :
	m_buffInfo{buffInfo}
{
	parseBuffsForData();
	sort();
}

Buff::Buff(const vector_t<BuffInfo> &buffInfo) :
	m_buffInfo{buffInfo}
{
	parseBuffsForData();
	sort();
}

auto Buff::parseBuffsForData() -> void {
	for (const auto &buff : m_buffInfo) {
		if (ext::in_range_inclusive<int16_t>(buff.getChance(), 0, 99)) {
			m_isSelectionBuff = true;
		}
		if (buff.hasAct()) {
			m_anyActs = true;
		}
	}
}

auto Buff::sort() -> void {
	std::sort(m_buffInfo.begin(), m_buffInfo.end(), [](const BuffInfo &l, const BuffInfo &r) {
		return l.getBitPosition() < r.getBitPosition();
	});
}

auto Buff::getDelay() const -> milliseconds_t {
	return m_delay;
}

auto Buff::getBuffInfo() const -> const vector_t<BuffInfo> & {
	return m_buffInfo;
}

auto Buff::isSelectionBuff() const -> bool {
	return m_isSelectionBuff;
}

auto Buff::anyBuffs() const -> bool {
	return m_buffInfo.size() > 0;
}

auto Buff::anyActs() const -> bool {
	return m_anyActs;
}

auto Buff::withBuffs(const vector_t<BuffInfo> &buffInfo) const -> Buff {
	Buff result{*this};
	result.m_buffInfo = buffInfo;
	result.sort();
	return result;
}

auto Buff::withDelay(milliseconds_t value) const -> Buff {
	Buff result{*this};
	result.m_delay = value;
	return result;
}