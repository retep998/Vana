/*
Copyright (C) 2008-2013 Vana Development Team

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
#include "TauswortheGenerator.h"

TauswortheGenerator::TauswortheGenerator()
{
}

TauswortheGenerator::TauswortheGenerator(uint32_t seed1, uint32_t seed2, uint32_t seed3)
{
	reset(seed1, seed2, seed3);
}

void TauswortheGenerator::reset(uint32_t seed1, uint32_t seed2, uint32_t seed3) {
	m_seed1 = seed1 | 0x100000;
	m_seed2 = seed2 | 0x1000;
	m_seed3 = seed3 | 0x10;
}

uint32_t TauswortheGenerator::next() {
	m_seed1 = ((m_seed1 & 0xFFFFFFFE) << 12) ^ (((m_seed1 << 13) ^ m_seed1) >> 19);
	m_seed2 = ((m_seed2 & 0xFFFFFFF8) <<  4) ^ (((m_seed2 <<  2) ^ m_seed2) >> 25);
	m_seed3 = ((m_seed3 & 0xFFFFFFF0) << 17) ^ (((m_seed3 <<  3) ^ m_seed3) >> 11);
	return (m_seed1 ^ m_seed2 ^ m_seed3);
}