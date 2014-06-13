--[[
Copyright (C) 2008-2014 Vana Development Team

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
-- Bush

dofile("scripts/utils/npcHelper.lua");
dofile("scripts/utils/tableHelper.lua");

if isQuestActive(2186) and getItemAmount(4031853) == 0 then
	if getNpcId() == 1094004 then
		if hasOpenSlotsFor(4031853) then
			giveItem(4031853, 1);
			addText("I found Abel's glasses.");
			sendNext();
		else
			-- TODO FIXME text?
		end
	else
		items = {4031854, 4031855};
		item = selectElement(items);
		if hasOpenSlotsFor(item) then
			giveItem(item, 1);
			addText("I found a pair of glasses, but it doesn't seem to be Abel's. ");
			addText("Abel's pair is horn-rimmed...");
			sendOk();
		else
			-- TODO FIXME text?
		end
	end
else
	-- TODO FIXME text?
end