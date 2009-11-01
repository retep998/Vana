/*
Copyright (C) 2008-2009 Vana Development Team

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
#include "PlayerRandomStream.h"
#include "PacketCreator.h"
#include "Player.h"
#include "Randomizer.h"

PlayerRandStream::PlayerRandStream(Player *p) :
m_player(p)
{
}

void PlayerRandStream::reset(int32_t seed1, int32_t seed2, int32_t seed3) {
	m_seed1 = seed1 | 0x100000;
	m_seed2 = seed2 | 0x1000;
	m_seed3 = seed3 | 0x10;
}

int32_t PlayerRandStream::next() {
	m_seed1 = ((m_seed1 & 0xFFFFFFFE) << 12) ^ ((m_seed1 & 0x7FFC0 ^ (m_seed1 >> 13)) >> 6);
	m_seed2 = 16 * (m_seed2 & 0xFFFFFFF8) ^ (((m_seed2 >> 2) ^ m_seed2 & 0x3F800000) >> 23);
	m_seed3 = ((m_seed3 & 0xFFFFFFF0) << 17) ^ (((m_seed3 >> 3) ^ m_seed3 & 0x1FFFFF00) >> 8);
	return (m_seed1 ^ m_seed2 ^ m_seed3);
}

void PlayerRandStream::connectData(PacketCreator &packet) {
	// Depending on how this works, may need to simply send m_seed1/etc. after reset call
	int32_t s1 = Randomizer::Instance()->randInt();
	int32_t s2 = Randomizer::Instance()->randInt();
	int32_t s3 = Randomizer::Instance()->randInt();

	reset(s1, s2, s3);

	packet.add<int32_t>(s1);
	packet.add<int32_t>(s2);
	packet.add<int32_t>(s3);
}