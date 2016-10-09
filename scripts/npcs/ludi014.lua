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
-- First Eos Rock

dofile("scripts/utils/npcHelper.lua");

if isGm() or getItemAmount(4001020) > 0 then
	addText("You can use " .. blue(itemRef(4001020)) .. " to activate " .. blue(npcRef(2040024)) .. ". ");
	addText("Will you head over to " .. blue(npcRef(2040025)) .. " at the 71st floor??");
	answer = askYesNo();

	if answer == answer_yes then
		giveItem(4001020, -1);
		setMap(221022900, "go00");
	end
else
	addText("There's a rock that will enable you to teleport to " .. blue(npcRef(2040025)) .. ", but it cannot be activated without the scroll.");
	sendOk();
end