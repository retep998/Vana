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
#include "Types.h"
#include <boost/utility.hpp>
#include <string>

using std::string;

class WorldServerConnection;
class PacketCreator;

// ChannelServer main application class, implemented as singleton
class ChannelServer : public AbstractServer, boost::noncopyable {
public:
	static ChannelServer * Instance() {
		if (singleton == 0)
			singleton = new ChannelServer;
		return singleton;
	}
	void loadData();
	void loadConfig();
	void listen();
	void shutdown();
	void connectWorld();
	void sendToWorld(PacketCreator &packet);

	void setWorld(int8_t id) { world = id; }
	void setMaxMultiLevel(uint8_t level) { maxMultiLevel = level; }
	void setMaxChars(int32_t chars) { maxChars = chars; }
	void setWorldPort(uint16_t port) { world_port = port; }
	void setPort(uint16_t port) { this->port = port; }
	void setChannel(int16_t channel) { this->channel = channel; }
	void setExprate(int32_t exprate) { this->exprate = exprate; }
	void setQuestExprate(int32_t questexprate) { this->questexprate = questexprate; }
	void setMesorate(int32_t mesorate) { this->mesorate = mesorate; }
	void setDroprate(int32_t droprate) { this->droprate = droprate; }
	void setMaxStats(int16_t max) { this->maxStats = max; }
	void setPianusAttempts(int16_t attempts) { pianusAttempts = attempts; }
	void setPapAttempts(int16_t attempts) { papAttempts = attempts; }
	void setZakumAttempts(int16_t attempts) { zakumAttempts = attempts; }
	void setHorntailAttempts(int16_t attempts) { horntailAttempts = attempts; }
	void setPinkBeanAttempts(int16_t attempts) { pinkbeanAttempts = attempts; }
	void setScrollingHeader(const string &message);
	void setWorldIp(uint32_t ip) { world_ip = ip; }

	int8_t getWorld() const { return world; }
	uint8_t getMaxMultiLevel() const { return maxMultiLevel; }
	int16_t getMaxStats() const { return maxStats; }
	int16_t getChannel() const { return channel; }
	int32_t getOnlineId() const { return 20000 + (int32_t) world * 100 + channel; }
	int32_t getExprate() const { return exprate; }
	int32_t getQuestExprate() const { return questexprate; }
	int32_t getMesorate() const { return mesorate; }
	int32_t getDroprate() const { return droprate; }
	int32_t getMaxChars() const { return maxChars; }
	int16_t getPianusAttempts() const { return pianusAttempts; }
	int16_t getPapAttempts() const { return papAttempts; }
	int16_t getZakumAttempts() const { return zakumAttempts; }
	int16_t getHorntailAttempts() const { return horntailAttempts; }
	int16_t getPinkBeanAttempts() const { return pinkbeanAttempts; }
	string getScrollingHeader() const { return scrollingHeader; }
	WorldServerConnection * getWorldConnection() const { return worldPlayer; }
	bool isConnected() const { return channel != -1; }
	
	// Specific bosses that can be battled on this channel
	void setPianusChannel(bool is) { this->pianusChannel = is; }
	void setPapChannel(bool is) { this->papChannel = is; }
	void setZakumChannel(bool is) { this->zakumChannel = is; }
	void setHorntailChannel(bool is) { this->horntailChannel = is; }
	void setPinkBeanChannel(bool is) { this->pinkbeanChannel = is; }
	bool isPianusChannel() const { return pianusChannel; }
	bool isPapChannel() const { return papChannel; }
	bool isZakumChannel() const { return zakumChannel; }
	bool isHorntailChannel() const { return horntailChannel; }
	bool isPinkBeanChannel() const { return pinkbeanChannel; }
	// Boss channel lists
	void setPianusChannels(const vector<int8_t> &channels) { this->pianusChannels = channels; }
	void setPapChannels(const vector<int8_t> &channels) { this->papChannels = channels; }
	void setZakumChannels(const vector<int8_t> &channels) { this->zakumChannels = channels; }
	void setHorntailChannels(const vector<int8_t> &channels) { this->horntailChannels = channels; }
	void setPinkBeanChannels(const vector<int8_t> &channels) { this->pinkbeanChannels = channels; }
	vector<int8_t> getPianusChannels() const { return pianusChannels; }
	vector<int8_t> getPapChannels() const { return papChannels; }
	vector<int8_t> getZakumChannels() const { return zakumChannels; }
	vector<int8_t> getHorntailChannels() const { return horntailChannels; }
	vector<int8_t> getPinkBeanChannels() const { return pinkbeanChannels; }
private:
	ChannelServer() : channel(-1) {};
	static ChannelServer *singleton;

	WorldServerConnection *worldPlayer;

	int8_t world;
	uint8_t maxMultiLevel;
	int16_t maxStats;
	int16_t channel;
	int16_t pianusAttempts;
	int16_t papAttempts;
	int16_t zakumAttempts;
	int16_t horntailAttempts;
	int16_t pinkbeanAttempts;
	uint16_t world_port;
	uint16_t login_inter_port;
	uint16_t port;
	int32_t exprate;
	int32_t questexprate;
	int32_t mesorate;
	int32_t droprate;
	int32_t maxChars;
	uint32_t login_ip;
	uint32_t world_ip;
	string scrollingHeader;
	bool pianusChannel;
	bool papChannel;
	bool zakumChannel;
	bool horntailChannel;
	bool pinkbeanChannel;
	vector<int8_t> pianusChannels;
	vector<int8_t> papChannels;
	vector<int8_t> zakumChannels;
	vector<int8_t> horntailChannels;
	vector<int8_t> pinkbeanChannels;
};
