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
-- A Mysterious Small Egg

addText("Hello, traveler. You have finally come to see me. Have you fulfilled your duties?\r\n\r\n");
addText("#b#L0#What duties? Who are you?#l");
askChoice();

addText("You found a small egg in your pocket? ");
addText("That egg is your duty, your responsibility. ");
addText("Life is hard when you're all by yourself. ");
addText("In times like this, there's nothing quite like having a friend that will be there for you at all times. ");
addText("Have you heard of a #bpet#k? \r\n");
addText("People raise pets to ease the burden, sorrow, and loneliness, because knowing that you have someone, or something in this matter, on your side will really bring a peace of mind. ");
addText("But everything has consequences, and with it comes responsibility...");
sendNext();

addText("Raising a pet requires a huge amount of responsibility. ");
addText("Remember, a pet is a form of life, as well, so you'll need to feed it, name it, share your thoughts with it, and ultimately form a bond. ");
addText("That's how the owners get attached to these pets.");
sendBackNext();

addText("I wanted to instill this in you, and that's why I sent you a baby that I cherish. ");
addText("The egg you have brought is #bRune Snail#k, a creature that is born through the power of Mana. ");
addText("Since you took great care of it as you brought the egg here, the egg will hatch soon.");
sendBackNext();

addText("The Rune Snail is a pet of many skills. ");
addText("It'll pick up items, feed you with potions, and do other things that will astound you. ");
addText("The downside is that since it was born out of power of Mana, its lifespan is very short. ");
addText("Once it turns into a doll, it'll never be able to be revived.");
sendBackNext();

addText("Now do you understand? ");
addText("Every action comes with consequences, and pets are no exception. ");
addText("The egg of the snail shall hatch soon.");
answer = askYesNo();

if answer == 1 then
	addText("This snail will only be alive for #b5 hours#k. ");
	addText("Shower it with love. ");
	addText("Your love will be reciprocated in the end.");
	sendNext();

	giveItem(4032086, -1);
	-- TODO FIXME add time expiration
	giveItem(5000054, 1);
	endQuest(2230);
end