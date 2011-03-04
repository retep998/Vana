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
-- Wayne

addText("You are looking lovely today! I'm here to help you prepare for your Wedding. I can help you make a Reservation, get additional Invitations, or tell you what you'll need to get married in our Chapel. What would you like to know?\r\n");
addText("#b#L0#How can I get married here?#l\r\n");
addText("#L1#I'd like to make a Premium Reservation.#l\r\n");
addText("#L2#I'd like to make a Normal Reservation.#l\r\n");
addText("#L3#I have more guests coming, I'd like some more Invitations.#l\r\n");
addText("#L4#I would like to cancel my wedding reservation.#l#k");
value = askChoice();

if value == 0 then
	addText("To get married in the Chapel, you'll need #ra Chapel Wedding Ticket, any Engagement Ring or an Empty Engagement Ring Box#k and some time. Soon as you have them, we'll be happy to assist with your Wedding plans!");
	sendNext();
elseif value == 1 then
	addText("To make a Reservation, you'll need to be grouped with your fiance... ");
	sendNext();
elseif value == 2 then
	addText("To make a Reservation, you'll need to be grouped with your fiance... ");
	sendNext();
elseif value == 3 then
	addText("To receive some more invitations, you'll need to be grouped with your fiance... ");
	sendNext();
elseif value == 4 then
	addText("If you cancel your wedding reservation, all items and quest information pertaining to weddings will disappear. Would you really like to cancel?");
	yes = askYesNo();
	if yes == 1 then
		addText("You have not yet made a wedding reservation.");
		sendOk();
	end
end
