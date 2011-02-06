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
-- Level 200/120 Player NPC's

if getName() == getPlayerNpcName() then
	lastTime = getPlayerVariable("rank_last");
	
	nowTime = getSecondsSinceEpoch();
	if lastTime == "" or ((nowTime - lastTime) / 60) >= 24 then
		addText("Will you transform your other self's appearance to your current state? You may transform your other self once a day.");
		yes = askYesNo();
		if yes == 1 then
			makeNewImitation();
			setPlayerVariable("rank_last", "" .. nowTime .. "");
			addText("Your other self has been transformed to resemble your current state. The transformation is available only once a day.");
			sendOK();
		else
			addText("It's okay to take your time. Let me know when you are ready.");
			sendOK();
		end
	else
		addText("You may transform your other self only once a day. Please try again tomorrow.");
		sendOK();
	end
else
	-- Todo: add LEVEL 120 message for KoC!
	addText("Hello, I am #b" .. getPlayerNpcName() .. "#k, and I am LEVEL " .. getPlayerNpcLevel() .. ".");
	sendOK();
end