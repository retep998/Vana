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
#include "LoginServer.h"
#include "InitializeCommon.h"
#include "InitializeLogin.h"
#include "ConnectionManager.h"
#include "RankingCalculator.h"
#include "Worlds.h"
#include <boost/format.hpp>

LoginServer * LoginServer::singleton = 0;

void LoginServer::listen() {
	ConnectionManager::Instance()->accept(port, new PlayerLoginFactory(), "0");
	ConnectionManager::Instance()->accept(inter_port, new LoginServerAcceptConnectionFactory());
}

void LoginServer::loadData() {
	Initializing::checkSchemaVersion(true);
	Initializing::setUsersOffline(1);
	Initializing::loadData();
	RankingCalculator::setTimer();
}

void LoginServer::loadConfig() {
	ConfigFile config("conf/loginserver.lua");
	pinEnabled = config.getBool("pin");
	port = config.getShort("port");
	inter_port = config.getShort("inter_port");
	invalid_login_threshold = config.getInt("invalid_login_threshold");
	to_listen = true;

	loadWorlds();
}

void LoginServer::loadWorlds() {
	ConfigFile config("conf/worlds.lua");
	boost::format formatter("world%i_%s"); // The formatter we'll be using
	size_t i = 0;
	while (1) {
		formatter % i % "name";
		if (!config.keyExist(formatter.str()))
			break; // No more worlds

		World *world = new World();
		world->name = config.getString(formatter.str());

		formatter % i % "channels";
		world->maxChannels = config.getInt(formatter.str());

		formatter % i % "id";
		world->id = (uint8_t) config.getInt(formatter.str());

		formatter % i % "ribbon";
		world->ribbon = (uint8_t) config.getInt(formatter.str());

		formatter % i % "port";
		world->port = config.getShort(formatter.str());

		formatter % i % "exprate";
		world->exprate = config.getInt(formatter.str());

		formatter % i % "questexprate";
		world->questexprate = config.getInt(formatter.str());

		formatter % i % "mesorate";
		world->mesorate = config.getInt(formatter.str());

		formatter % i % "droprate";
		world->droprate = config.getInt(formatter.str());

		formatter % i % "maxstats";
		world->maxStats = config.getShort(formatter.str());

		formatter % i % "max_multi_level";
		world->maxMultiLevel = (uint8_t) config.getInt(formatter.str());

		formatter % i % "event_msg";
		world->eventMsg = config.getString(formatter.str());

		formatter % i % "scrolling_header";
		world->scrollingHeader = config.getString(formatter.str());

		formatter % i % "max_player_load";
		world->maxPlayerLoad = config.getInt(formatter.str());

		formatter % i % "maxchars";
		world->maxChars = config.getInt(formatter.str());

		formatter % i % "pianus_channels";
		world->pianusChannels = config.getBossChannels(formatter.str(), world->maxChannels);

		formatter % i % "pap_channels";
		world->papChannels = config.getBossChannels(formatter.str(), world->maxChannels);

		formatter % i % "zakum_channels";
		world->zakumChannels = config.getBossChannels(formatter.str(), world->maxChannels);

		formatter % i % "horntail_channels";
		world->horntailChannels = config.getBossChannels(formatter.str(), world->maxChannels);

		formatter % i % "pinkbean_channels";
		world->pinkbeanChannels = config.getBossChannels(formatter.str(), world->maxChannels);

		formatter % i % "pianus_attempts";
		world->pianusAttempts = config.getShort(formatter.str());

		formatter % i % "pap_attempts";
		world->papAttempts = config.getShort(formatter.str());

		formatter % i % "zakum_attempts";
		world->zakumAttempts = config.getShort(formatter.str());

		formatter % i % "horntail_attempts";
		world->horntailAttempts = config.getShort(formatter.str());

		formatter % i % "pinkbean_attempts";
		world->pinkbeanAttempts = config.getShort(formatter.str());

		Worlds::worlds[world->id] = world;
		i++;
	}
}