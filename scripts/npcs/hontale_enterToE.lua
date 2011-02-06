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
-- Encrypted Slate of the Squad

if isPartyLeader() then
	if getItemAmount(4001086) > 0 then
		addText("The letters on the slate glitter and the backdoor opens. Do you want to go to the secret path?");
		yes = askYesNo();
		if yes == 1 then
			warpParty(240050400);
		else
			addText("If you want to move, talk to me again.");
			sendNext();
		end
	else
		addText("You can't read the words on the slate. You have no idea where to use it.");
		sendNext();
	end
else
	addText("Please proceed through the Party Leader.");
	sendNext();
end