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
-- Konpei (warps to Hideout entrance from Bodyguard room)

dofile("scripts/utils/npcHelper.lua");

if getItemAmount(4000141) == 0 then
	addText("Once you eliminate the boss, you'll have to show me the boss's flashlight as evidence. ");
	addText("I won't believe it until you show me the flashlight! ");
	addText("What? ");
	addText("You want to leave this room?");
	answer = askYesNo();

	if answer == answer_no then
		addText("I really admire your toughness! ");
		addText("Well, if you decide to return to Showa Town, let me know~!");
		sendOk();
	else
		setMap(801040000);
	end
else
	-- TODO FIXME implement boss
end