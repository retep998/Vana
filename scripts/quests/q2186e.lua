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
-- Abel's Glasses

dofile("scripts/utils/npcHelper.lua");

addText("What? ");
addText("You found my glasses? ");
addText("I better put it on first, to make sure that it's really mine. ");
addText("Oh, it really is mine. ");
addText("Thank you so much!\r\n\r\n");
addText(questCompleteIcon() .. "\r\n");
addText(questItemIcon(2030019, 5) .. "\r\n\r\n");
addText(questExpIcon(1000) .. "\r\n");
sendNext();

if hasOpenSlotsFor(2030019, 5) then
	giveItem(4031853, -1);
	giveItem(2030019, 5);
	giveExp(1000);
	endQuest(2186);

	addText("Yes...time for some fishing!");
	sendBackNext();
else
	-- TODO FIXME text?
end