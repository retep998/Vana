--[[
Copyright (C) 2008-2009 Vana Development Team

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
if state == 0 then
	addText("Beep... beep... you can make you escape to a safer place through me. ");
	addText("Beep ... beep ... would you like to leave this place?");
	sendYesNo();
elseif state == 1 then
	if getSelected() == 1 then
		if getNumPlayers(220080001) == 1 then
			setReactorsState(220080000, 2208001, 0);
			setReactorsState(220080000, 2208003, 0);
			setReactorsState(220080001, 2201004, 0);
			clearDrops();
			clearMobs();
		end
		setMap(220080000);
	end
	endNPC();
else
	endNPC();
end
