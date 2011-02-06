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
-- Nautilus' Mid-Sized Taxi

if getJob() == 0 then
	prices = {120, 100, 100, 120, 100};
else
	prices = {1200, 1000, 1000, 1200, 1000};
end

maps = {104000000, 102000000, 100000000, 103000000, 101000000};

addText("How's it going? I drive the #p1092014#. If you want to go from town to town safely and fast, then ride our cab. We'll gladly take you to your destination with an affordable price.");
sendNext();

if getJob() == 0 then
	addText("We have a special 90% discount for beginners. Choose your destination, for fees will change from place to place.\r\n");
	addText("#b#L0# #m104000000#(120 mesos)#l\r\n");
	addText("#L1# #m102000000#(100 mesos)#l\r\n");
	addText("#L2# #m100000000#(100 mesos)#l\r\n");
	addText("#L3# #m103000000#(120 mesos)#l\r\n");
	addText("#L4# #m101000000#(100 mesos)#l");
else
	addText("Choose your destination, for fees will change from place to place.\r\n");
	addText("#b#L0# #m104000000#(1200 mesos)#l\r\n");
	addText("#L1# #m102000000#(1000 mesos)#l\r\n");
	addText("#L2# #m100000000#(1000 mesos)#l\r\n");
	addText("#L3# #m103000000#(1200 mesos)#l\r\n");
	addText("#L4# #m101000000#(1000 mesos)#l");
end
where = askChoice() + 1;

addText("You don't have anything else to do here, huh? Do you really want to go to #b#m" .. maps[where] .. "##k? It'll cost you #b" .. prices[where] .. " mesos#k.");
yes = askYesNo();

if yes == 1 then
	if giveMesos(-prices[where]) then
		setMap(maps[where]);
	else
		addText("You don't have enough mesos. Sorry to say this, but without them, you won't be able to ride this cab.");
		sendOK();
	end
else
	addText("There’s a lot to see in this town, too. Come back and find me when you need to go to a different town.");
	sendOK();
end