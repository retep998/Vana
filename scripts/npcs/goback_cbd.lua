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
-- Kerny, sends player back to Singapore (from both in flight and waiting room and sends player to Kerning on the trip from Kerning)

dofile("scripts/lua_functions/npcHelper.lua");

m = getMap();
if m == 540010001 then
	-- Waiting room to Kerning
	addText("The plane will be taking off soon. ");
	addText("Will you leave now? ");
	addText("You will have buy the plane ticket again to come in here.");
	answer = askYesNo();

	if answer == answer_yes then
		addText("I have already told you the ticket is not refunable~ hope to see you again~");
		sendOk();

		setMap(540010000);
	else
		addText("Please hold on for a sec. ");
		addText("The plane will be taking off soon. ");
		addText("Thanks for your patience.");
		sendOk();
	end
elseif m == 540010002 then
	-- Trip to Kerning
	addText("The plane will be arriving soon. ");
	addText("Are you sure you want to leave now? ");
	answer = askYesNo();

	if answer == answer_yes then
		setMap(540010000);
	else
		addText("Please hold on for a sec. ");
		addText("The plane will be arriving shortly. ");
		addText("Thanks for your patience.");
		sendOk();
	end
elseif m == 540010101 then
	-- Trip to CBD
	addText("The plane will be arriving soon. ");
	addText("Are you sure you want to leave now? ");
	answer = askYesNo();

	if answer == answer_yes then
		setMap(103000000);
	else
		addText("Please hold on for a sec. ");
		addText("The plane will be arriving shortly. ");
		addText("Thanks for your patience.");
		sendOk();
	end
end