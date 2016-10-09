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
-- Treasure Chest - Shumi's jump quest 1

dofile("scripts/utils/npcHelper.lua");
dofile("scripts/utils/tableHelper.lua");

-- TODO FIXME all Shumi quests need the 100-minute personal timer and possibly the distance requirement

if isQuestActive(2055) then
	if getItemAmount(4031039) > 0 then
		addText("Looking carefully into Treasure Chest, there seems to be nothing there.");
		sendNext();

		setMap(103000100);
	else
		addText("Looking carefully into Treasure Chest, there seems to be a shiny object inside");

		if hasOpenSlotsFor(4031039) then
			addText(". Reached out with a hand and was able to attain a small coin.");
			giveItem(4031039, 1);
			sendNext();

			setMap(103000100);
		else
			addText(" but since your etc. inventory is full, that item is unattainable.");
			sendNext();
		end
	end
else
	items = {4020000, 4020001, 4020002, 4020003, 4020004, 4020005};
	item = selectElement(items);
	if hasOpenSlotsFor(item, 2) then
		giveItem(item, 2);
		setMap(103000100);
	else
		addText("The item wasn't found because your etc. inventory is full. ");
		addText("Make space for the item and try again.");
		sendNext();
	end
end