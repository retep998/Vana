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
#pragma once

#include "AbstractServer.h"
#include "ConfigFile.h"
#include "LoginServerConnection.h"
#include "Types.h"
#include "WorldServerAcceptConnection.h"
#include <string>
#include <boost/utility.hpp>

using std::string;

// WorldServer main application class, implemented as singleton
class WorldServer : public AbstractServer, boost::noncopyable {
public:
	static WorldServer * Instance() {
		if (singleton == 0)
			singleton = new WorldServer;
		return singleton;
	}
	void loadData();
	void loadConfig();
	void listen();

	void setWorldId(int8_t id) { worldId = id; }
	void setMaxMultiLevel(uint8_t level) { maxMultiLevel = level; }
	void setInterPort(uint16_t port) { inter_port = port; }
	void setMaxChannels(int32_t maxchan) { maxChannels = maxchan; }
	void setMaxChars(int32_t maxchar) { maxChars = maxchar; }
	void setExprate(int32_t exprate) { this->exprate = exprate; }
	void setQuestExprate(int32_t questexprate) { this->questexprate = questexprate; }
	void setMesorate(int32_t mesorate) { this->mesorate = mesorate; }
	void setDroprate(int32_t droprate) { this->droprate = droprate; }
	void setMaxStats(int16_t max) { this->maxStats = max; }
	void setScrollingHeader(const string &message);
	void setPianusAttempts(int16_t attempts) { pianusAttempts = attempts; }
	void setPapAttempts(int16_t attempts) { papAttempts = attempts; }
	void setZakumAttempts(int16_t attempts) { zakumAttempts = attempts; }
	void setHorntailAttempts(int16_t attempts) { horntailAttempts = attempts; }
	void setPinkBeanAttempts(int16_t attempts) { pinkbeanAttempts = attempts; }
	void setPianusChannels(const vector<int8_t> &channels) { this->pianusChannels = channels; }
	void setPapChannels(const vector<int8_t> &channels) { this->papChannels = channels; }
	void setZakumChannels(const vector<int8_t> &channels) { this->zakumChannels = channels; }
	void setHorntailChannels(const vector<int8_t> &channels) { this->horntailChannels = channels; }
	void setPinkBeanChannels(const vector<int8_t> &channels) { this->pinkbeanChannels = channels; }

	int8_t getWorldId() const { return worldId; }
	uint8_t getMaxMultiLevel() const { return maxMultiLevel; }
	int16_t getMaxStats() const { return maxStats; }
	int16_t getPianusAttempts() const { return pianusAttempts; }
	int16_t getPapAttempts() const { return papAttempts; }
	int16_t getZakumAttempts() const { return zakumAttempts; }
	int16_t getHorntailAttempts() const { return horntailAttempts; }
	int16_t getPinkBeanAttempts() const { return pinkbeanAttempts; }
	uint16_t getInterPort() const { return inter_port; }
	int32_t getMaxChannels() const { return maxChannels; }
	int32_t getExprate() const { return exprate; }
	int32_t getQuestExprate() const { return questexprate; }
	int32_t getMesorate() const { return mesorate; }
	int32_t getDroprate() const { return droprate; }
	int32_t getMaxChars() const { return maxChars; }
	vector<int8_t> getPianusChannels() const { return pianusChannels; }
	vector<int8_t> getPapChannels() const { return papChannels; }
	vector<int8_t> getZakumChannels() const { return zakumChannels; }
	vector<int8_t> getHorntailChannels() const { return horntailChannels; }
	vector<int8_t> getPinkBeanChannels() const { return pinkbeanChannels; }
	LoginServerConnection * getLoginConnection() const { return loginPlayer; }
	string getScrollingHeader() { return scrollingHeader.c_str(); }
	bool isConnected() const { return worldId != -1; }
private:
	WorldServer() : worldId(-1) { }
	static WorldServer *singleton;

	int8_t worldId;
	uint8_t maxMultiLevel;
	int16_t maxStats;
	int16_t pianusAttempts;
	int16_t papAttempts;
	int16_t zakumAttempts;
	int16_t horntailAttempts;
	int16_t pinkbeanAttempts;
	uint16_t login_inter_port;
	uint16_t inter_port;
	int32_t exprate;
	int32_t questexprate;
	int32_t mesorate;
	int32_t droprate;
	int32_t maxChannels;
	int32_t maxChars;
	uint32_t login_ip;
	string scrollingHeader;
	vector<int8_t> pianusChannels;
	vector<int8_t> papChannels;
	vector<int8_t> zakumChannels;
	vector<int8_t> horntailChannels;
	vector<int8_t> pinkbeanChannels;
	LoginServerConnection *loginPlayer;
};
