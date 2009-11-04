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
#include "ConfigurationPacket.h"
#include "Configuration.h"
#include "PacketCreator.h"
#include "PacketReader.h"

void ConfigurationPacket::addConfig(Configuration &config, PacketCreator &packet) {
	packet.add<int8_t>(config.ribbon);
	packet.add<uint8_t>(config.maxMultiLevel);
	packet.add<int16_t>(config.pianusAttempts);
	packet.add<int16_t>(config.papAttempts);
	packet.add<int16_t>(config.zakumAttempts);
	packet.add<int16_t>(config.horntailAttempts);
	packet.add<int16_t>(config.pinkbeanAttempts);
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
	packet.addVector(config.pianusChannels);
	packet.addVector(config.papChannels);
	packet.addVector(config.zakumChannels);
	packet.addVector(config.horntailChannels);
	packet.addVector(config.pinkbeanChannels);
}

Configuration ConfigurationPacket::getConfig(PacketReader &packet) {
	Configuration conf;
	conf.ribbon = packet.get<int8_t>();
	conf.maxMultiLevel = packet.get<uint8_t>();
	conf.pianusAttempts = packet.get<int16_t>();
	conf.papAttempts = packet.get<int16_t>();
	conf.zakumAttempts = packet.get<int16_t>();
	conf.horntailAttempts = packet.get<int16_t>();
	conf.pinkbeanAttempts = packet.get<int16_t>();
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
	conf.pianusChannels = packet.getVector<int8_t>();
	conf.papChannels = packet.getVector<int8_t>();
	conf.zakumChannels = packet.getVector<int8_t>();
	conf.horntailChannels = packet.getVector<int8_t>();
	conf.pinkbeanChannels = packet.getVector<int8_t>();
	return conf;
}