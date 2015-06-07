--[[
Copyright (C) 2008-2015 Vana Development Team

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
-- A Pile of Pink Flowers

dofile("scripts/utils/npcHelper.lua");
dofile("scripts/utils/tableHelper.lua");

-- TODO FIXME implement distance properly
--You can't see the inside of the pile of flowers very well because you're too far. Go a little closer.

if isQuestActive(2054) then
	item = 4031028;
	if hasOpenSlotsFor(item, 30) then
		giveItem(item, 30);
		setMap(105040300);
	else
		addText("Sorry, but your etc. inventory is full. ");
		addText("Please make some space and try again.");
		sendNext();
	end
else
	items = {4010006, 4020007, 4020008};
	item = selectElement(items);
	if hasOpenSlotsFor(item, 2) then
		giveItem(item, 2);
		setMap(105040300);
	else
		addText("Sorry, but your etc. inventory is full. ");
		addText("Please make some space and try again.");
		sendNext();
	end
end