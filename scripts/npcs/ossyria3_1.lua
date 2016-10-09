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
-- Orbis Magic Spot (Orbis Tower: 20th Floor)

dofile("scripts/utils/npcHelper.lua");

if getItemAmount(4001019) >= 1 then
	addText("You can use " .. blue(itemRef(4001019)) .. " to activate " .. blue(npcRef(2012014)) .. ". ");
	addText("Will you teleport to where " .. blue(npcRef(2012015)) .. " is?");
	answer = askYesNo();

	if answer == answer_yes then
		giveItem(4001019, -1);
		setMap(200082100);
	else
		-- Intentionally left blank
	end
else
	addText("There's a " .. blue(npcRef(2012014)) .. " that'll enable you to teleport to where " .. blue(npcRef(2012015)) .. " is, but you can't activate it without the scroll.");
	sendOk();
end