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
-- Puddle in Horntail's cave

dofile("scripts/utils/npcHelper.lua");

if isQuestActive(6280) and getItemAmount(4031454) > 0 then
	if getItemAmount(4031455) > 0 then
		addText("You already have " .. blue(itemRef(4031455)) .. ".");
		sendOk();
	else
		giveItem(4031454, -1);
		giveItem(4031455, 1);
		addText("Holy water was contained from spring.");
		sendOk();
	end
elseif isQuestActive(6169) and getItemAmount(4031461) < 1 then
	if giveItem(4031461, 1) then
		addText("You found a small Life's Root growing in a place of death...");
		sendOk();
	end
end