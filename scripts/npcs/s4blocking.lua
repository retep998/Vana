--[[
Copyright (C) 2008-2014 Vana Development Team

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
-- Moose

mapId = getMap();
item = 1092041;

if mapId == 924000000 then
	addText("I have to let you know one thing before sending you to the training field. You have to hold #b#t1092041##k that I gave you in shield training field. Otherwise, you're dead.");
	sendNext();

	addText("Don't forget #rto hold shield#k before you get there!\r\n");
	addText("#b#L0# I want to get #t1092041#.#l\r\n");
	addText("#L1# Let me go in to #m924000001#.#l\r\n");
	addText("#L2# Let me out.#l");
	choice = askChoice();

	if choice == 0 then
		if getItemAmount(item) > 0 then
			addText("You already have #t" .. item .. "##k. No need more.");
			sendNext();			
		else
			if destroyEquippedItem(item) then
				showMessage("Shield for learning skill was removed.", env_redMessage);
			end

			if hasOpenSlotsFor(item, result) then
				giveItem(item, 1);
				addText("I gave you #t" .. item .. "#. Check inventory. You have to be equipped with it!");
				sendNext();
			else
				addText("I couldn't give you #t" .. item .. "##k as there's no blank in Equipment box. ");
				addText("Make a blank and try again.");
				sendNext();
			end
		end
	elseif choice == 1 then
		if isInstance("guardian") then
			addText("Other characters are on request. You can't enter.");
			sendNext();
		else
			createInstance("guardian", 20 * 60, true);
			setMap(924000001);
		end
	elseif choice == 2 then
		setMap(924000002);
	end
elseif mapId == 924000001 then
	addText("Do you want to get out of here?");
	verify = askYesNo();
	if verify == 1 then
		setMap(924000002);
	end
elseif mapId == 924000002 then
	addText("I'll let you out. You have to give #t1092041# back.");
	sendNext();

	if destroyEquippedItem(item) then
		showMessage("Shield for learning skill was removed.", env_redMessage);
	end

	setMap(240010400, "st00");
end