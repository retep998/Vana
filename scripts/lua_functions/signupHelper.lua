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
-- Various functions to make coding sign up scripts much easier

function getList()
	g = getInstanceSignupCount();
	addText("The total number of applicants are #b" .. g .. "#k, and the list of the applicants is as follows.\r\n");
	addText("1 : #b" .. getInstancePlayerByIndex(1) .. "#k (The Leader of the Squad)\r\n");
	for i = 2, g do
		addText(i .. " : " .. getInstancePlayerByIndex(i) .. "\r\n");
	end

	i = g;
	for i = i + 1, getInstanceMax() do
		addText(i .. " : \r\n");
	end
end

function getLinkedList()
	addText("Which of these members would you like to expel?\r\n");
	for i = 2, getInstanceSignupCount() do
		addText("#L" .. i - 2 .. "# " .. i .. " : " .. getInstancePlayerByIndex(i) .. "#l\r\n");
	end
end

function getBannedList()
	addText("Whose application are you willing to accept?\r\n");
	for i = 1, getBannedInstancePlayerCount() do
		addText("#b#L" .. i - 1 .. "#" .. getBannedInstancePlayerByIndex(i) .. "#l#k\r\n");
	end
end

function isListFull()
	return getInstanceSignupCount() >= getInstanceMax();
end

function messageAll(msg)
	for i = 1, getInstanceSignupCount() do
		if setPlayer(getInstancePlayerByIndex(i)) then
			showMessage(msg, m_red);
			revertPlayer();
		end
	end
end