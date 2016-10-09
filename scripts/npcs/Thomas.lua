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
-- Thomas Swift (warps to/from Amoria)

dofile("scripts/utils/npcHelper.lua");

if getMap() == 100000000 then
	addText("I can take you to Amoria Village. ");
	addText("Are you ready to go?");
	answer = askYesNo();

	if answer == answer_yes then
		addText("I hope you had a great time! ");
		addText("See you around!");
		sendNext();

		setMap(680000000);
	else
		addText("Ok, feel free to hang around until you're ready to go!");
		sendOk();
	end
else
	addText("I can take you back to your original location. ");
	addText("Are you ready to go?");
	answer = askYesNo();

	if answer == answer_yes then
		addText("I hope you had a great time! ");
		addText("See you around!");
		sendNext();

		setMap(100000000);
	else
		addText("Ok, feel free to hang around until you're ready to go!");
		sendNext();
	end
end