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
-- Moony

addText("Have you found true love? If so, I can make you a ring worthy of your devotion...\r\n");
addText("#b#L0#I would like to make an engagement ring for my lover.\r\n");
addText("#L1#I want an annulment.#l#k");
value = askChoice();

if value == 0 then
	addText("Looks like you're not quite done. If you want that ring, you'd better hurry and get me the following all materials to make an engagement ring. I need you to bring me #b4 Proof of Love#k from Nana.");
	sendNext();
elseif value == 1 then
	addText("It looks like you haven't married yet. I don't have anything to talk to you about this.");
	sendOk();
end
