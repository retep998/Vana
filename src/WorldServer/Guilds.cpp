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

#include "Guilds.h"
#include "Database.h"
#include "Guild.h"
#include "GuildPacket.h"
#include "InitializeCommon.h"
#include "Players.h"
#include "PacketCreator.h"

using Initializing::outputWidth;

Guilds * Guilds::singleton = 0;

Guild * Guilds::addGuild(Guild *guild) {
	m_guilds[guild->getId()] = guild;
	m_guilds_names[guild->getName()] = guild;
	return m_guilds[guild->getId()];
}

Guild * Guilds::getGuild(const string &name) {
	return (m_guilds_names.find(name) == m_guilds_names.end() ? 0 : m_guilds_names[name]);
}

Guild * Guilds::getGuild(int32_t id) {
	return (m_guilds.find(id) == m_guilds.end() ? 0 : m_guilds[id]);
}

void Guilds::removeGuild(Guild *guild) {
	GuildPacket::InterServerPacket::unloadGuild(guild->getId());

	for (unordered_map<int32_t, Player *>::iterator iter = guild->m_players.begin(); iter != guild->m_players.end(); iter++) {
		iter->second->guildid = 0;
		iter->second->guildrank = 5;
		iter->second->allianceid = 0;
		iter->second->alliancerank = 5;
	}

	m_guilds.erase(guild->getId());
	m_guilds_names.erase(guild->getName());
	delete guild;
}

void Guilds::createGuild(string name, string notice, int32_t id, int32_t leaderid, int32_t capacity, int16_t logo, uint8_t logocolor, int16_t logobg, uint8_t logobgcolor, int32_t gp, string title1, string title2, string title3, string title4, string title5, int32_t alliance) {
	Guild *guild = getGuild(id);
	if (guild == 0) {
		guild = new Guild(name, notice, id, leaderid, capacity, logo, logocolor, logobg, logobgcolor, gp, title1, title2, title3, title4, title5, alliance);
		addGuild(guild);
	}
	else {
		guild->setNotice(notice);
		guild->setGuildPoints(gp);
		guild->setAllianceId(alliance);
		guild->setLogo(logo);
		guild->setLogoColor(logocolor);
		guild->setLogoBg(logobg);
		guild->setLogoBgColor(logobgcolor);
		guild->setTitle(1, title1);
		guild->setTitle(2, title2);
		guild->setTitle(3, title3);
		guild->setTitle(4, title4);
		guild->setTitle(5, title5);
	}
}

void Guilds::getChannelConnectPacket(PacketCreator &packet) {
	packet.add<int32_t>(m_guilds.size());
	for (unordered_map<int32_t, Guild *>::iterator iter = m_guilds.begin(); iter != m_guilds.end(); iter++) {
		packet.add<int32_t>(iter->second->getId());
		packet.addString(iter->second->getName());
		packet.add<int16_t>(iter->second->getLogo());
		packet.add<uint8_t>(iter->second->getLogoColor());
		packet.add<int16_t>(iter->second->getLogoBg());
		packet.add<uint8_t>(iter->second->getLogoBgColor());
		packet.add<int32_t>(iter->second->getCapacity());
		packet.add<int32_t>(iter->second->getAllianceId());
	}
}
