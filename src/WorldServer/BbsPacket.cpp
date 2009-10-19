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
#include "BbsPacket.h"
#include "Channels.h"
#include "Guild.h"
#include "GuildBbs.h"
#include "InterHeader.h"
#include "MapleSession.h"
#include "PacketCreator.h"
#include "SendHeader.h"
#include "TimeUtilities.h"

void BbsPacket::sendThreadList(uint16_t channel, Guild *guild, int32_t playerid, int16_t page) {
	GuildBbs *bbs = guild->getBbs();
	int32_t threads = bbs->m_threads.size();
	int16_t counter1 = 0;
	int16_t counter2 = 0;
	int32_t startNumber = page * 10; // 10 threads per page
	int32_t threadsOnPage = std::min<int32_t>(10, (threads - startNumber));

	PacketCreator packet;
	packet.add<int16_t>(INTER_FORWARD_TO);
	packet.add<int32_t>(playerid);
	
	packet.add<int16_t>(SMSG_BBS);
	packet.add<int8_t>(0x06);
	
	BbsThread *notice = bbs->getNotice();
	if (notice != 0) {
		packet.addBool(true);
		packet.add<int32_t>(notice->getListId());
		packet.add<int32_t>(notice->getUserId());
		packet.addString(notice->getTitle());
		packet.add<int64_t>(TimeUtilities::timeToTick(notice->getTime()));
		packet.add<int32_t>(notice->getIcon());
		packet.add<int32_t>(notice->getSize());
	} else {
		packet.addBool(false);
	}

	packet.add<int32_t>(threads);
	packet.add<int32_t>(threadsOnPage);

	for (map<int32_t, BbsThread *>::reverse_iterator iter = bbs->m_threads.rbegin(); iter != bbs->m_threads.rend(); iter++) {
		counter1++;
		if (counter1 > startNumber && counter2 <= threadsOnPage) {
			counter2++;
			packet.add<int32_t>(iter->second->getListId());
			packet.add<int32_t>(iter->second->getUserId());
			packet.addString(iter->second->getTitle());
			packet.add<int64_t>(TimeUtilities::timeToTick(iter->second->getTime()));
			packet.add<int32_t>(iter->second->getIcon());
			packet.add<int32_t>(iter->second->m_replies.size());
		}
	}

	Channels::Instance()->sendToChannel(channel, packet);
}

void BbsPacket::sendThreadData(uint16_t channel, BbsThread *thread, int32_t playerid) {
	PacketCreator packet;
	packet.add<int16_t>(INTER_FORWARD_TO);
	packet.add<int32_t>(playerid);

	packet.add<int16_t>(SMSG_BBS);
	packet.add<int8_t>(0x07);

	packet.add<int32_t>(thread->getListId());
	packet.add<int32_t>(thread->getUserId());
	packet.add<int64_t>(TimeUtilities::timeToTick(thread->getTime()));
	packet.addString(thread->getTitle());
	packet.addString(thread->getContent());
	packet.add<int32_t>(thread->getIcon());

	packet.add<int32_t>(thread->m_replies.size());
		
	for (map<int32_t, BbsReply *>::iterator iter = thread->m_replies.begin(); iter != thread->m_replies.end(); iter++) {
		packet.add<int32_t>(iter->second->getId());
		packet.add<int32_t>(iter->second->getUserId());
		packet.add<int64_t>(TimeUtilities::timeToTick(iter->second->getTime()));
		packet.addString(iter->second->getContent());
	}

	Channels::Instance()->sendToChannel(channel, packet);
}
