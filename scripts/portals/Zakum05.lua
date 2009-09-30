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
-- Door to Zakum

if isZakumChannel() then
	if getReactorState(211042300, 2118002) == 1 then
		showMessage("The battle with Zakum has already begun.", 5);
	else
		playPortalSE();
		setMap(211042400, "west00");
	end
else
	channels = getZakumChannels();
	if #channels == 0 then
		showMessage("You may not battle Zakum at this time.", 5);
	elseif #channels == 1 then
		showMessage("You may only fight Zakum on channel " .. channels[1] .. ".", 5);
	elseif #channels == 2 then
		showMessage("You may only fight Zakum on channels " .. channels[1] .. " and " .. channels[2] .. ".", 5);
	else
		local msg = channels[1];
		local max = #channels - 1;
		local i = 2;
		for inc = 2, max do
			msg = msg .. ", " .. channels[i];
			i = i + 1;
		end
		msg = msg .. ", and " .. channels[i];
		showMessage("You may only fight Zakum on channels " .. msg .. ".", 5);
	end
end