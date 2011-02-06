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
-- Shanks

if getItemAmount(4031801) > 0 then
	lucas = true;
end

addText("Take this ship and you'll head off to a bigger continent.#e For 150 mesos#n, I'll take you to #bVictoria Island#k. The thing is, once you leave this place, you can't ever come back. What do you think? Do you want to go to Victoria Island?");
yes = askYesNo();

if yes == 1 then
	if getLevel() >= 7 then
		if lucas then
			addText("Okay, now give me 150 mesos... Hey, what's that? Is that the recommendation letter from Lucas, the chief of Amherst? Hey, you should have told me you had this. I, Shanks, recognize greatness when I see one, and since you have been recommended by Lucas, I see that you have a great, great potential as an adventurer. No way would I charge you for this trip!");
			sendNext();

			addText("Since you have the recommendation letter, I won't charge you for this. Alright, buckle up, because we're going to head to Victoria Island right now, and it might get a bit turbulent!!");
			sendBackNext();

			giveItem(4031801, -1);
			setMap(104000000);
		elseif getMesos() >= 150 then
			addText("Bored of this place? Here... Give me 150 mesos first...");
			sendNext();

			addText("Awesome! #e150 mesos#n accepted! Alright, off to Victoria Island!");
			sendNext();

			giveMesos(-150);
			setMap(104000000);
		else
			addText("What? You're telling me you wanted to go without any money? You're one weirdo...");
			sendBackNext();
		end
	else
		addText("Let's see... I don't think you are strong enough. You'll have to be at least #bLevel 7#k ");
		addText("to go to Victoria Island.");
		sendNext();
	end
else
	addText("Hmm... I guess you still have things to do here?");
	sendNext();
end
