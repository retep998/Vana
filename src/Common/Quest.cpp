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
#include "Quest.h"

void Quest::addItemRequest(int32_t itemId, int16_t quantity) {
	itemrequests[itemId] = quantity;
}

void Quest::addMobRequest(int32_t mobid, int16_t quantity) {
	mobrequests[mobid] = quantity;
}

void Quest::addQuestRequest(int16_t questId, int8_t state) {
	questrequests[questId] = state;
}

void Quest::addValidJob(int16_t jobid) {
	jobrequests.push_back(jobid);
}

void Quest::addReward(bool start, const QuestRewardInfo &info, int16_t job) {
	if (start) {
		if (job == -1) {
			startrewards.rewards.push_back(info);
		}
		else {
			startrewards.jobrewards[job].push_back(info);
		}
	}
	else {
		if (job == -1) {
			endrewards.rewards.push_back(info);
		}
		else {
			endrewards.jobrewards[job].push_back(info);
		}
	}
}