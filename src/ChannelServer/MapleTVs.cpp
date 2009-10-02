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
#include "MapleTVs.h"
#include "Map.h"
#include "Player.h"
#include "PlayerPacketHelper.h"
#include "SendHeader.h"
#include "Timer/Time.h"
#include "Timer/Timer.h"
#include <functional>

using std::tr1::bind;

MapleTVs * MapleTVs::singleton = 0;

MapleTVs::MapleTVs() : m_timers(new Timer::Container), m_counter(0), m_hasmessage(false) {
	m_tvs.push_back(9201066); // NLC
	m_tvs.push_back(9250023); // Aquarium
	m_tvs.push_back(9250024); // El Nath
	m_tvs.push_back(9250025); // Free Market
	m_tvs.push_back(9250026); // Ludibrium
	m_tvs.push_back(9250042); // Henesys
	m_tvs.push_back(9250043); // Kerning
	m_tvs.push_back(9250044); // Ellinia
	m_tvs.push_back(9250045); // Perion
	m_tvs.push_back(9250046); // Orbis
	m_tvs.push_back(9270000); // Amoria
	m_tvs.push_back(9270001); // Lith Harbor
	m_tvs.push_back(9270002); // Sleepywood
	m_tvs.push_back(9270003); // Omega Sector
	m_tvs.push_back(9270004); // Korean Folk Town
	m_tvs.push_back(9270005); // Leafre
	m_tvs.push_back(9270006); // Mu Lung
	m_tvs.push_back(9270007); // Bak Cho (?)
	m_tvs.push_back(9270008); // Y Tan (??)
	m_tvs.push_back(9270009); // Yuan (???)
	m_tvs.push_back(9270010); // SeuMunJung (?!)
	m_tvs.push_back(9270011); // Night Market
	m_tvs.push_back(9270012); // Mushroom Shrine
	m_tvs.push_back(9270013); // Showa
	m_tvs.push_back(9270014); // Ninja (!?)
	m_tvs.push_back(9270015); // Taiwan
	m_tvs.push_back(9270016); // Golden
	m_tvs.push_back(9270040); // Singapore
}

void MapleTVs::addMap(Map *map) {
	m_maps[map->getInfo()->id] = map;
}

void MapleTVs::addMessage(Player *sender, Player *receiver, const string &msg, const string &msg2, const string &msg3, const string &msg4, const string &msg5, int32_t megaphoneid, int32_t time) {
	MapleTVMessage message;
	message.hasreceiver = (receiver != 0);
	message.megaphoneid = megaphoneid;
	message.senderid = sender->getId();
	message.time = time;
	message.counter = getCounter();
	message.msg1 = msg;
	message.msg2 = msg2;
	message.msg3 = msg3;
	message.msg4 = msg4;
	message.msg5 = msg5;
	PlayerPacketHelper::addPlayerDisplay(message.senddisplay, sender); // We need to save the packet since it gets buffered and there's no guarantee the player will exist later
	message.sendname = sender->getName();
	if (receiver != 0) {
		PlayerPacketHelper::addPlayerDisplay(message.recvdisplay, receiver);
		message.recvname = receiver->getName();
	}

	m_buffer.push_back(message);

	if (!m_hasmessage) { // First element pushed
		parseBuffer();
		m_hasmessage = true;
	}
}

void MapleTVs::parseBuffer() {
	PacketCreator packet;
	if (m_buffer.size() > 0) {
		MapleTVMessage message = m_buffer.front();
		m_buffer.pop_front();

		getMapleTVPacket(message, packet);
		sendPacket(packet);

		m_currentmessage = message;

		Timer::Id id(Timer::Types::MapleTVTimer, message.senderid, message.counter);
		new Timer::Timer(bind(&MapleTVs::parseBuffer, this),
			id, getTimers(), Timer::Time::fromNow(message.time * 1000));
	}
	else {
		m_hasmessage = false;
		endMapleTVPacket(packet);
		sendPacket(packet);
	}
}

void MapleTVs::sendPacket(PacketCreator &packet) {
	for (unordered_map<int32_t, Map *>::iterator iter = m_maps.begin(); iter != m_maps.end(); iter++) {
		iter->second->sendPacket(packet);
	}
}

bool MapleTVs::isMapleTVNPC(int32_t id) const {
	bool is = false;
	for (size_t i = 0; i < m_tvs.size(); i++) {
		if (m_tvs[i] == id) {
			is = true;
			break;
		}
	}
	return is;
}

int32_t MapleTVs::checkMessageTimer() const {
	Timer::Id id(Timer::Types::MapleTVTimer, m_currentmessage.senderid, m_currentmessage.counter);
	return getTimers()->checkTimer(id);
}

void MapleTVs::getMapleTVEntryPacket(PacketCreator &packet) {
	 getMapleTVPacket(m_currentmessage, packet, checkMessageTimer());
}

void MapleTVs::getMapleTVPacket(MapleTVMessage &message, PacketCreator &packet, int32_t timeleft) {
	packet.add<int16_t>(SMSG_MAPLETV_ON);
	packet.add<int8_t>(message.hasreceiver ? 3 : 1);
	packet.add<int8_t>((int8_t)(message.megaphoneid - 5075000)); // Positively will be within -128 to 127
	packet.addBuffer(message.senddisplay);
	packet.addString(message.sendname);
	packet.addString(message.hasreceiver ? message.recvname : "");
	packet.addString(message.msg1);
	packet.addString(message.msg2);
	packet.addString(message.msg3);
	packet.addString(message.msg4);
	packet.addString(message.msg5);
	packet.add<int32_t>(timeleft == 0 ? message.time : timeleft);
	if (message.hasreceiver) {
		packet.addBuffer(message.recvdisplay);
	}
}

void MapleTVs::endMapleTVPacket(PacketCreator &packet) {
	packet.add<int16_t>(SMSG_MAPLETV_OFF);
}