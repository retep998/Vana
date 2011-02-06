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
-- Shawn, Excavation Site

while true do
	if extra == nil then
		extra = 1;
		addText("We, the Union of Guilds, have been trying to decipher 'Emerald Tablet,' a treasured old relic, for a long time. As a result, we have found out that Sharenian, the mysterious country from the past, lay asleep here. We also found out that clues of #t4001024#, a legendary, mythical jewelry, may be here at the remains of Sharenian. This is why the Union of Guilds have opened Guild Quest to ultimately find #t4001024#.\r\n");
	elseif extra == 1 then
		addText("Do you have any other questions?\r\n");
	end
	addText("#b#L0# What's Sharenian?#l \r\n");
	addText("#b#L1# #t4001024#? What's that?#l\r\n");
	addText("#b#L2# Guild Quest?#l\r\n");
	addText("#b#L3# No, I'm fine now.#l");
	what = askChoice();

	if what == 0 then
		addText("Sharenian was a literate civilization from the past that had control over every area of the Victoria Island. The Temple of Golem, the Shrine in the deep part of the Dungeon, and other old architectural constructions where no one knows who built it are indeed made during the Sharenian times.");
		sendNext();
		
		addText("The last king of Sharenian was a gentleman named Sharen III, and apparently he was a very wise and compassionate king. But one day, the whole kingdom collapsed, and there was no explanation made for it.");
		sendBackNext();
	elseif what == 1 then
		addText("#t4001024# is a legendary jewel that brings eternal youth to the one that possesses it. Ironically, it seems like everyone that had #t4001024# ended up downtrodden, which should explain the downfall of Sharenian.");
		sendNext();
	elseif what == 2 then
		addText("I've sent groups of the explorers to Sharenian before, but none of them ever came back, which prompted us to start the Guild Quest. We've been waiting for guilds that are strong enough to take on tough challenges, guilds like yours.");
		sendNext();
		
		addText("The ultimate goal of this Guild Quest is to explore Sharenian and find #t4001024#. This is not a task where power solves everything. Teamwork is more important here.");
		sendBackNext();
	elseif what == 3 then
		addText("Really? If you have anything else to ask, please feel free to talk to me.");
		sendOK();
		break;
	end
end