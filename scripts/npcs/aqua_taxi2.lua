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
-- Dolphin (Herb Town Pier)

dofile("scripts/utils/npcHelper.lua");

addText("Will you move to " .. blue(mapRef(230000000)) .. " now? ");
addText("The price is " .. blue("10000 mesos") .. ".");
answer = askYesNo();

if answer == answer_yes then
	if giveMesos(-10000) then
		setMap(230000000);
	else
		addText("I don't think you have enough money...");
		sendNext();
	end
else
	addText("Hmmm ...too busy to do it right now? ");
	addText("If you feel like doing it, though, come back and find me.");
	sendOk();
end