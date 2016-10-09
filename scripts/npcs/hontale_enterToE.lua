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
-- Encrypted Slate of the Squad

dofile("scripts/utils/npcHelper.lua");

-- TODO FIXME implement Horntail PQ

if isPartyLeader() then
	if getItemAmount(4001086) > 0 then
		addText("The letters on the slate glitter and the backdoor opens. ");
		addText("Do you want to go to the secret path?");
		answer = askYesNo();

		if answer == answer_yes then
			warpParty(240050400);
		else
			addText("If you want to move, talk to me again.");
			sendOk();
		end
	else
		addText("You can't read the words on the slate. ");
		addText("You have no idea where to use it.");
		sendOk();
	end
else
	addText("Please proceed through the Party Leader.");
	sendOk();
end