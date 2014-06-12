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
-- A Pile of Pink Flowers

dofile("scripts/lua_functions/npcHelper.lua");
dofile("scripts/lua_functions/tableHelper.lua");

-- TODO FIXME implement distance properly
--You can't see the inside of the pile of flowers very well because you're too far. Go a little closer.

if isQuestActive(2052) then
	item = 4031025;
	if hasOpenSlotsFor(item, 10) then
		giveItem(item, 10);
		setMap(105040300);
	else
		addText("Sorry, but your etc. inventory is full. ");
		addText("Please make some space and try again.");
		sendNext();
	end
else
	items = {4010000, 4010001, 4010002, 4010003, 4010004, 4010005};
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