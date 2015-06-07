--[[
Copyright (C) 2008-2015 Vana Development Team

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
-- Trash Can near Muirhat

dofile("scripts/utils/npcHelper.lua");

if not isQuestActive(2162) and not isQuestCompleted(2162) then
	addText("A half-written letter...maybe it's important! ");
	addText("Should I take a look?");
	sendNext();

	if getItemAmount(4031839) > 0 then
		addText("I've already picked one up. ");
		addText("I don't think I'll need to pick up another one.");
		sendOk();
	else
		if not hasOpenSlotsFor(4031839, 1) then
			-- TODO FIXME text?
		else
			giveItem(4031839, 1);
			addText("I can barely maket his out...but it reads Kyrin.");
			sendOk();
		end
	end
else
	-- TODO FIXME text?
end