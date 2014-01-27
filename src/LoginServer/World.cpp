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
#include "World.h"
#include "LoginServerAcceptConnection.h"
#include "PacketCreator.h"
#include "Randomizer.h"

auto World::runChannelFunction(function_t<void (Channel *)> func) -> void {
	for (const auto &kvp : m_channels) {
		func(kvp.second.get());
	}
}

auto World::getRandomChannel() const -> uint16_t {
	return Randomizer::rand<uint16_t>(getMaxChannels() - 1);
}

auto World::send(const PacketCreator &packet) -> void {
	m_connection->getSession()->send(packet);
}

auto World::matchSubnet(const Ip &test) -> Ip {
	return m_connection->matchSubnet(test);
}