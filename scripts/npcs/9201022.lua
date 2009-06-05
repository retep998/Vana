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
-- Thomas Swift (warps to/from Amoria)

if getMap() == 100000000 then
	if state == 0 then
		addText("I can take you to Amoria Village. Are you ready to go?");
		sendYesNo();
	elseif state == 1 then
		if getSelected() == 1 then
			addText("I hope you had a great time! See you around!");
			sendNext();
		else
			addText("Ok, feel free to hang around until you're ready to go!");
			sendOK();
			endNPC();
		end
	elseif state == 2 then
		setMap(680000000);
		endNPC();
	end
else	
	if state == 0 then
		addText("I can take you back to your original location. Are you ready to go?");
		sendYesNo();
	elseif state == 1 then
		if getSelected() == 1 then
			addText("I hope you had a great time! See you around!");
			sendNext();
		else
			addText("Ok, feel free to hang around until you're ready to go!");
			sendNext();
			endNPC();
		end
	elseif state == 2 then
		setMap(100000000);
		endNPC();
	end
end
