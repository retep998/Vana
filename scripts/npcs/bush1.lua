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
-- A Pile of Flowers

dofile("scripts/utils/npcHelper.lua");
dofile("scripts/utils/tableHelper.lua");

-- TODO FIXME implement distance properly
-- You can't see the inside of the pile of flowers very well because you're too far. Go a little closer.

if isQuestActive(2050) then
	item = 4031020;
	addText("Are you sure you want to take " .. blue(itemRef(item)) .. " with you?");
	answer = askYesNo();

	if answer == answer_yes then
		if giveItem(item, 1) then
			setMap(101000000);
		else
			addText("Your etc. inventory seems to be full. ");
			addText("Please make room in order to take the item.");
			sendNext();
		end
	end
else
	items = {4010000, 4010001, 4010002, 4010003, 4010004, 4010005, 4020000, 4020001, 4020002, 4020003, 4020004, 4020005, 4020006};
	item = selectElement(items);
	if giveItem(item, 2) then
		setMap(101000000);
	else
		addText("You need to have at least one free slot available on your etc. inventory to keep the item you found in the midst of flowers. ");
		addText("Please make room and then try again.");
		sendNext();
	end
end