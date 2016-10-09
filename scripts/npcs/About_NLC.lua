--[[
Copyright (C) 2008-2016 Vana Development Team

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
-- Icebyrd Slimm (NLC)

dofile("scripts/utils/npcHelper.lua");

iterationBreak = 1;

choices = {
	makeChoiceHandler("What is this place? ", function()
		addText("I've always dreamed of building a city. ");
		addText("Not just any city, but one where everyone was welcome. ");
		addText("I used to live in Kerning City, so I decided to see if I could create a city. ");
		addText("As I went along in finding the means to do so, I encountered many people, some of whom I've come to regard as friends. ");
		addText("Like Professor Foxwit-he's our resident genius; saved him from a group of man-eating plants. ");
		addText("Jack Masque is an old hunting buddy from Amoria-almost too smooth of a talker for his own good. ");
		addText("Lita and I are old friends from Kerning City-she's saved me a few times with that weapon of hers; so I figured she was a perfect choice for Town Sheriff. ");
		addText("It took a bit of persuasion, but she came to believe her destiny lies here. ");
		addText("About our resident explorer, Barricade came searching for something; he agreed to bring whatever he found to the museum. ");
		addText("I'd heard stories about him and his brother when I was still in Kerning City. ");
		addText("And Elpam...well, let's just say he's not from around here. ");
		addText("At all. ");
		addText("We've spoken before, and he seems to mean well, so I've allowed him to stay. ");
		addText("I just realized that I've rambled quite a bit! ");
		addText("What else would you like to know?");
		sendNext();
	end),
	makeChoiceHandler("Who is Professor Foxwit? ", function()
		addText("A pretty spry guy for being 97. ");
		addText("He' s a time-traveler I ran into outside the city one day. ");
		addText("Old guy had a bit of trouble with some jungle creatures-like they tried to eat him. ");
		addText("In return for me saving him, he agreed to build a time museum. ");
		addText("I get the feeling that he's come here for another reason, as he's mentioned more than a few times that New Leaf City has an interesting role to play in the future. ");
		addText("Maybe you can find out a bit more...");
		sendNext();
	end),
	makeChoiceHandler("What's a Foxwit Door? ", function()
		addText("Heh, I asked the same thing when I saw the Professor building them. ");
		addText("They're warp points. ");
		addText("Pressing Up will warp you to another location. ");
		addText("I recommend getting the hang of them, they're our transport system.");
		sendNext();
	end),
	makeChoiceHandler("Where are the MesoGears? ", function()
		addText("The MesoGears are beneath Bigger Ben. ");
		addText("It's a monster-infested section of Bigger Ben that Barricade discovered. ");
		addText("It seems to reside in a separate section of the tower-quite strange if you ask me. ");
		addText("I hear he needs a bit of help exploring it, you should see him. ");
		addText("Be careful though, the Wolf Spiders in there are no joke.");
		sendNext();
	end),
	makeChoiceHandler("What is the Krakian Jungle?", function()
		addText("Ah...well. ");
		addText("The Krakian Jungle is located on the outskirts of New Leaf City. ");
		addText("Many new and powerful creatures roam those areas, so you'd better be prepared to fight if you head out there. ");
		addText("It's at the left end of town. ");
		addText("Rumors abound that the Jungle leads to a lost city, but we haven't found anything yet.");
		sendNext();
	end),
	makeChoiceHandler("What's a Gear Portal? ", function()
		addText("Well, when John found himself in the MesoGears portion of Bigger Ben, he stood on one and went to another location. ");
		addText("However, he could only head back and forth-they don't cycle through like the Foxwit Door. ");
		addText("Ancient tech for you.");
		sendNext();
	end),
	makeChoiceHandler("What do the street signs mean? ", function()
		addText("Well, you'll see them just about everywhere. ");
		addText("They're areas under construction. ");
		addText("The Red lights mean it's not finished, but the Green lights mean it's open. ");
		addText("Check back often, we're always building!");
		sendNext();
	end),
	makeChoiceHandler("What's the deal with Jack Masque? ", function()
		addText("Ah, Jack. ");
		addText("You know those guys that are too cool for school? ");
		addText("The ones who always seem to get away with everything? ");
		addText("AND get the girl? ");
		addText("Well, that's Jack, but without the girl. ");
		addText("He thinks he blew his chance, and began wearing that mask to hide his true identity. ");
		addText("My lips are sealed about who he is, but he's from Amoria. ");
		addText("He might tell you a bit more if you ask him.");
		sendNext();
	end),
	makeChoiceHandler("Lita Lawless looks like a tough cookie, what's her story?", function()
		addText("I've known Lita for a while, thought we've just recently rekindled our friendship. ");
		addText("I didn't see her for a quite a bit, but I understand why. ");
		addText("She trained for a very, very long time as a Thief. ");
		addText("Matter of fact, that's how we first met? ");
		addText("I was besieged a group of wayward Mushrooms, and she jumped in to help. ");
		addText("When it was time to a pick a sheriff, it was a no-brainer. ");
		addText("She's made a promise to help others in their training and protect the city, so if you're interested in a bit of civic duty, speak with her.");
		sendNext();
	end),
	makeChoiceHandler("When will new boroughs open up in the city? ", function()
		addText("Soon, my friend. ");
		addText("Even though you can't see them, the city developers are hard at work. ");
		addText("When they're ready, we'll open them. ");
		addText("I know you're looking forward to it and so am I!");
		sendNext();
	end),
	makeChoiceHandler("I want to take the quiz!", function()
		if getLevel() < 15 then
			addText("Sorry but this quiz is only available for level 15 and above. ");
			addText("Please come back to me when you are ready to take this quiz.");
			sendNext();
		else
			-- TODO FIXME implement quiz
			return iterationBreak;
		end
	end),
};

while true do
	addText("What up! ");
	addText("Name's " .. npcRef(9201050) .. ", mayor of New Leaf City! ");
	addText("Happy to see you accepted my invite. ");
	addText("So, what can I do for you?\r\n");
	addText(blue(choiceRef(choices)));
	choice = askChoice();

	if selectChoice(choices, choice) == iterationBreak then
		break;
	end
end