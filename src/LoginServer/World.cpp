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
#include "World.h"
#include "LoginServerAcceptConnection.h"
#include "PacketCreator.h"
#include "Randomizer.h"

void World::runChannelFunction(function<void (Channel *)> func) {
	for (ChannelMap::iterator iter = m_channels.begin(); iter != m_channels.end(); ++iter) {
		func(iter->second.get());
	}
}

uint16_t World::getRandomChannel() const {
	return Randomizer::rand<uint16_t>(getMaxChannels() - 1);
}

void World::send(const PacketCreator &packet) {
	m_connection->getSession()->send(packet);
}

Ip World::matchSubnet(const Ip &test) {
	return m_connection->matchSubnet(test);
}