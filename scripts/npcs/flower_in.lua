--[[
Copyright (C) 2008-2016 Vana Development Team

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
--]]
-- Mysterious Statue

dofile("scripts/utils/npcHelper.lua");

function validQuest(questId)
	return isQuestActive(questId) or isQuestCompleted(questId);
end

if validQuest(2052) or validQuest(2053) or validQuest(2054) then
	item = nil;
	mapId = nil;

	-- Done from last quest to beginning quest, otherwise the completed quest might stick you in an earlier quest
	if validQuest(2054) then
		item = 4031028;
		mapId = 105040314;
	elseif validQuest(2053) then
		item = 4031026;
		mapId = 105040312;
	elseif validQuest(2052) then
		item = 4031025;
		mapId = 105040310;
	end

	if getItemAmount(item) > 0 then
		addText("I laid my hand on the statue but nothing had happened.\r\n");
		addText("Probably because of " .. itemRef(item) .. " that I have, because it looks like it only interferes with the power of the statue.");
		sendNext();
	else
		addText("Once I lay my hand on the statue, a strange light covers me and it feels like I am being sucked into somewhere else. ");
		addText("Is it okay to be moved to somewhere else randomly just like that?");
		answer = askYesNo();

		if answer == answer_no then
			addText("Once I took my hand off the statue it got quiet, as if nothing happened.");
			sendNext();
		else
			setMap(mapId);
		end
	end
else
	addText("I laid my hand on the statue but nothing had happened.");
	sendOk();
end