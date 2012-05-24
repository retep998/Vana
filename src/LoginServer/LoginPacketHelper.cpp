/*
Copyright (C) 2008-2012 Vana Development Team

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
#include "LoginPacketHelper.h"
#include "Characters.h"
#include "ItemConstants.h"
#include "PacketCreator.h"
#include "TimeUtilities.h"

void LoginPacketHelper::addCharacter(PacketCreator &packet, const Character &charc, CharacterAddModus cam) {
	packet.add<int32_t>(charc.id);
	packet.addString(charc.name, 13);
	packet.add<int8_t>(charc.gender);
	packet.add<int8_t>(charc.skin);
	packet.add<int32_t>(charc.eyes);
	packet.add<int32_t>(charc.hair);
	packet.add<int64_t>(0);
	packet.add<int64_t>(0);
	packet.add<int64_t>(0);
	packet.add<uint8_t>(charc.level);
	packet.add<int16_t>(charc.job);
	packet.add<int16_t>(charc.str);
	packet.add<int16_t>(charc.dex);
	packet.add<int16_t>(charc.intt);
	packet.add<int16_t>(charc.luk);
	packet.add<int32_t>(charc.hp); // V.83+: MP and HP max up (short -> int)
	packet.add<int32_t>(charc.mhp);
	packet.add<int32_t>(charc.mp);
	packet.add<int32_t>(charc.mmp);
	packet.add<int16_t>(charc.ap);

	if (GameLogicUtilities::isExtendedSpJob(charc.job)) {
		packet.add<int8_t>(charc.sp_table.size());
		for (vector<pair<int8_t, int8_t>>::const_iterator iter = charc.sp_table.begin(); iter != charc.sp_table.end(); iter++) {
			packet.add<int8_t>(iter->first);
			packet.add<int8_t>(iter->second);
		}
	}
	else {
		packet.add<int16_t>(charc.sp);
	}

	packet.add<int32_t>(charc.exp);
	packet.add<int32_t>(charc.fame); // V.??+: Fame max up (short -> int)
	packet.add<int32_t>(0);
	packet.add<int32_t>(charc.map);
	packet.add<int8_t>(charc.pos);
	packet.add<int32_t>(0);


	// New
	packet.add<int16_t>(0);// charc.jobType); // For Cannoneer and Dual Blader
	// TODO: Implement
	if (charc.job / 100 == Jobs::JobTracks::DemonSlayer || charc.job == Jobs::JobIds::DemonSlayer) {
		packet.add<int32_t>(1012280);
	}

	packet.add<int8_t>(20);

	{
		time_t rawtime = time(NULL);
		struct tm * timeinfo;
		timeinfo = localtime ( &rawtime );

		char buff[15];
		sprintf(buff, "%04d%02d%02d%02d", (timeinfo->tm_year + 1900), (timeinfo->tm_mon + 1), timeinfo->tm_mday, timeinfo->tm_hour);
		packet.add<int32_t>(atoi(buff)); // YYYYMMDDHH
	}

	packet.add<int32_t>(0);
	packet.add<int32_t>(0);

	packet.add<int32_t>(0);
	packet.add<int32_t>(0);

	packet.add<int32_t>(0);
	packet.add<int32_t>(0);

	// 12 bytes?
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	
	packet.add<int32_t>(0);

	packet.add<int8_t>(10);
	packet.add<int32_t>(0);
	packet.add<int8_t>(5);
	packet.add<int32_t>(0);

	packet.add<int64_t>(18293692055145101115); // Filetime thing?



	packet.add<int8_t>(charc.gender);
	packet.add<int8_t>(charc.skin);
	packet.add<int32_t>(charc.eyes);
	packet.add<int32_t>(charc.job); // New addition.

	packet.add<int8_t>(1);
	packet.add<int32_t>(charc.hair);
	int32_t equips[Inventories::EquippedSlots][2] = {0};
	for (size_t i = 0; i < charc.equips.size(); i++) {
		int16_t slot = -charc.equips[i].slot;
		if (slot > 100) {
			slot -= 100;
		}
		if (equips[slot][0] > 0) {
			if (charc.equips[i].slot < -100) {
				equips[slot][1] = equips[slot][0];
				equips[slot][0] = charc.equips[i].id;
			}
			else {
				equips[slot][1] = charc.equips[i].id;
			}
		}
		else {
			equips[slot][0] = charc.equips[i].id;
		}
	}


	for (int8_t i = 0; i < Inventories::EquippedSlots; i++) {
		// Shown items
		if (equips[i][0] > 0) {
			packet.add<int8_t>(i);
			if (i == EquipSlots::Weapon && equips[i][1] > 0) {
				// Normal weapons always here
				packet.add<int32_t>(equips[i][1]);
			}
			else {
				packet.add<int32_t>(equips[i][0]);
			}
		}
	}
	packet.add<int8_t>(-1);
	for (int8_t i = 0; i < Inventories::EquippedSlots; i++) {
		// Covered items
		if (equips[i][1] > 0 && i != EquipSlots::Weapon) {
			packet.add<int8_t>(i);
			packet.add<int32_t>(equips[i][1]);
		}
	}
	packet.add<int8_t>(-1);

	packet.add<int32_t>(equips[EquipSlots::Weapon][0]); // Cash weapon

	packet.add<int8_t>(0);

	packet.add<int32_t>(0); // Pets
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);

	if (charc.job / 100 == Jobs::JobTracks::DemonSlayer || charc.job == Jobs::JobIds::DemonSlayer) {
		packet.add<int32_t>(1012280);
	}

	if (cam != AddedNewCharacter) {
		if (cam != ViewAllCharacters) {
			packet.add<int8_t>(0); // Sometimes 1, sometimes 0
		}

		// Rankings
		packet.addBool(true);
		packet.add<int32_t>(charc.worldRank);
		packet.add<int32_t>(charc.worldRankChange);
		packet.add<int32_t>(charc.jobRank);
		packet.add<int32_t>(charc.jobRankChange);
	}
}