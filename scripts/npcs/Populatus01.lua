--[[
Copyright (C) 2008-2011 Vana Development Team

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
-- Machine Apparatus - Origin of Clocktower

addText("Beep... beep... you can make you escape to a safer place through me. ");
addText("Beep ... beep ... would you like to leave this place?");
yes = askYesNo();

if yes == 1 then
	setMap(220080000);
	if isInstance("papulatus") then
		removeInstancePlayer(getID());
		if getInstancePlayerCount() == 0 then
			setReactorState(220080000, 2208001, 0);
			setReactorState(220080000, 2208003, 0);
			setReactorState(220080001, 2201004, 0);
			markForDelete();
		end
	end
	if getNumPlayers(220080001) == 0 then
		clearDrops(220080001);
		clearMobs(220080001);
	end
end
