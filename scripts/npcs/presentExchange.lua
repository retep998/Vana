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
-- Pila Present

addText("How do I help you? \r\n");
addText("#b#L0#I am about to finish my wedding and want to pick my presents which my friends gave to me.#l\r\n");
addText("#L1#I have an #rOnyx Chest#k and want to ask for you to open it.#l\r\n");
addText("#L2#I have an #rOnyx Chest for Bride and Groom#k and want to ask for you to open it.#l#k");
value = askChoice();

if value == 0 then
	addText("You are currently not married.");
	sendNext();
elseif value == 1 then
	addText("I've got some fabulous items ready for you. Are you ready to pick them out?");
	yes = askYesNo();

	if yes == 1 then
		addText("I don't think you have an Onyx Chest that I can open, kid...");
		sendNext();
	else
		addText("Awww, really? I'm the only one who can open your Onyx Chest! I will be here and wait for you~");
		sendNext();
	end
elseif value == 2 then
	addText("I've got some fabulous items ready for you. Are you ready to pick them out?");
	yes = askYesNo();

	if yes == 1 then
		addText("I don't think you have an #rOnyx Chest for Bride and Groom#k that I can open, kid...");
		sendNext();
	else
		addText("Awww, really? I'm the only one who can open your Onyx Chest! I will be here and wait for you~");
		sendOk();
	end
end
