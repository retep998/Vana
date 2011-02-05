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
-- Hikari (warps to M/F Locker Room)

addText("Would you like to enter the bathhouse? That'll be 300 mesos for you. And don't take the towels!");
yes = askYesNo();

if yes == 0 then
	addText("Please come back some other time. ");
	sendOK();
else
	if giveMesos(-300) then
		if getGender() == 0 then
			setMap(801000100);
		else
			setMap(801000200);
		end
	else
		-- TODO: Find the text
	end
end

