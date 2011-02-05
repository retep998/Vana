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
#include "ConfigurationPacket.h"
#include "Configuration.h"
#include "PacketCreator.h"
#include "PacketReader.h"

void ConfigurationPacket::addConfig(Configuration &config, PacketCreator &packet) {
	packet.add<int8_t>(config.ribbon);
	packet.add<uint8_t>(config.maxMultiLevel);
	packet.add<int16_t>(config.maxStats);
	packet.add<int32_t>(config.expRate);
	packet.add<int32_t>(config.questExpRate);
	packet.add<int32_t>(config.mesoRate);
	packet.add<int32_t>(config.dropRate);
	packet.add<int32_t>(config.maxChars);
	packet.add<int32_t>(config.maxPlayerLoad);
	packet.add<int32_t>(config.maxChannels);
	packet.addString(config.eventMsg);
	packet.addString(config.scrollingHeader);
	packet.addString(config.name);
	addMajorBoss(config.pianus, packet);
	addMajorBoss(config.pap, packet);
	addMajorBoss(config.zakum, packet);
	addMajorBoss(config.horntail, packet);
	addMajorBoss(config.pinkbean, packet);
}

Configuration ConfigurationPacket::getConfig(PacketReader &packet) {
	Configuration conf;
	conf.ribbon = packet.get<int8_t>();
	conf.maxMultiLevel = packet.get<uint8_t>();
	conf.maxStats = packet.get<int16_t>();
	conf.expRate = packet.get<int32_t>();
	conf.questExpRate = packet.get<int32_t>();
	conf.mesoRate = packet.get<int32_t>();
	conf.dropRate = packet.get<int32_t>();
	conf.maxChars = packet.get<int32_t>();
	conf.maxPlayerLoad = packet.get<int32_t>();
	conf.maxChannels = packet.get<int32_t>();
	conf.eventMsg = packet.getString();
	conf.scrollingHeader = packet.getString();
	conf.name = packet.getString();
	conf.pianus = getMajorBoss(packet);
	conf.pap = getMajorBoss(packet);
	conf.zakum = getMajorBoss(packet);
	conf.horntail = getMajorBoss(packet);
	conf.pinkbean = getMajorBoss(packet);
	return conf;
}

void ConfigurationPacket::addMajorBoss(const MajorBoss &boss, PacketCreator &packet) {
	packet.add<int16_t>(boss.attempts);
	packet.addVector(boss.channels);
}

MajorBoss ConfigurationPacket::getMajorBoss(PacketReader &packet) {
	MajorBoss boss;
	boss.attempts = packet.get<int16_t>();
	boss.channels = packet.getVector<int8_t>();
	return boss;
}