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
-- Ames the Wise (Amoria)

dofile("scripts/utils/npcHelper.lua");

iterationBreak = 1;

choices = {
	makeChoiceHandler("Yes, I'm interested in marriage.", function()
		addText("Ready to get married, eh? ");
		addText("First things first-congratulations! ");
		addText("I'm happy for you. ");
		addText("Second, you'll need to get engaged. ");
		addText("After that, figure out where you want to get married-at the Cathedral or the White Wedding Chapel. ");
		addText("Both are great places as far as I'm concerned.");
		sendNext();

		addText("You'll also need a Normal or Premium Wedding ticket from the Cash Shop for either location-just one per couple. ");
		addText("After that, you'll be happily married and enjoy the benefits of marriage. ");
		addText("I can explain a little more if you want...");
		sendBackNext();
	end),
	makeChoiceHandler("No, no...I'm definitely not ready for that. But I'd still like to see what Amoria's about. Do I have to be married?", function()
		addText("Amoria welcomes everyone. ");
		addText("You do not have to married to help some of the good townspeople around here-or hunt. ");
		addText("I invite you to speak with a few of them and see if you can assist them. ");
		sendNext();
	end),
	makeChoiceHandler("Ok, I think I'll talk to some people around here.", function()
		addText("Great! Stay as long as you like. ");
		addText("See me if you have some questions.");
		sendOk();
		return iterationBreak;
	end),
	makeChoiceHandler("How do I get engaged?", function()
		addText("Good thinking. ");
		addText("You'll have to be male to obtain an Engagement Ring. ");
		addText("After you have one, select the propose option, and wait for a response from your sweetheart. ");
		addText("I recommend both of you being online so you can celebrate. ");
		addText("As for getting an engagement ring, I'd speak with " .. npcRef(9201000) .. ", our ring-maker.");
		sendNext();

		addText("He's crafts the best rings I've ever seen-4 different kinds in fact. ");
		addText("When you do select your engagement ring, please pick carefully-wouldn't want to make a mistake! ");
		addText("What else is on your mind?");
		sendBackNext();
	end),
	makeChoiceHandler("How do I get married?", function()
		addText("Straight to the point, eh? ");
		addText("I like that. ");
		addText("Well, after you're engaged, you'll need to speak with Wedding Coordinator Victoria or " .. npcRef(9201012) .. " and make your reservation. ");
		addText("You will need a normal or Premium Wedding Ticket from the Cash Shop, an Engagement Ring and some time. ");
		addText("If you've recently performed an annulment, then you'll need to wait 7 days before you can marry again and " .. npcRef(9201000) .. " will make you a new ring.");
		sendNext();

		addText("There's a bit more documentation for the Cathedral, they have a more elaborate system over there. ");
		addText("Also, you'll need to tell the Wedding Coordinator your wish lists.");
		sendBackNext();

		addText("Pila Present will hold them for you when your friends turn them in, and you can pick them up afterwards. ");
		addText("You'll also get wedding invitations to send them. ");
		addText("Do you know where you want to get married?");
		sendBackNext();
	end),
	makeChoiceHandler("I want to be married in the White Wedding Chapel!", function()
		addText("Bit of an adventurous streak, I see. ");
		addText("Pelvis Bebop and his crew know how to put on a nice show. ");
		addText("Just speak with Wedding Assistant Bonnie, and be sure you have a Wedding Receipt, an Engagement Ring and about 5-10 minutes.");
		sendNext();

		addText("The White Wedding Chapel is faster, and a little more care-free. ");
		addText("Anything else you're interested in?");
		sendBackNext();
	end),
	makeChoiceHandler("I want the wedding of my dreams in the Cathedral!", function()
		addText("Ah, the Cathedral. ");
		addText("If there's a place you want to really want to get married in style, that's it. ");
		addText("As I mentioned before, they have a more elaborate system, so you'll need to speak with High Priest John for his permission.");
		sendNext();

		addText("The bride-to-be will also have to call on " .. npcRef(9201003) .. " to vouch for you both. ");
		addText("To get married in the Cathedral, Just speak with Wedding Assistant Nicole and be sure you have a Wedding Receipt, the Officiator's Permission, and an Engagement Ring and about 10-20 minutes.");
		sendBackNext();

		addText("Also, the Cathedral allows your guests to give the married couple an experience blessing during the wedding-1 exp per click, in fact. ");
		addText("What else would you like to know?");
		sendBackNext();
	end),
	makeChoiceHandler("How do I invite my friends?", function()
		addText("You will receive Wedding Invitations along with your Wedding ticket. ");
		addText("If you need more, you can talk with one of the Wedding Assistants. ");
		addText("The invitations are pretty simple, you just type in your friend's name, hit 'Send' and off they go.");
		sendNext();

		addText("They'll land in the Etc slot-make sure they have a few spaces free. ");
		addText("What else would you like to know?");
		sendBackNext();
	end),
	makeChoiceHandler("What happens after the Wedding?", function()
		addText("For the White Wedding Chapel and the cathedral, after the Wedding ends, you and your guests are sent to the photo area, Cherished Visage. ");
		addText("You can snap away for 60 seconds, and then relax with the new couple for 5 minutes.");
		sendNext();

		addText("After that, you're whisked back to Amoria. ");
		addText("Unless you have a premium ticket, in which case you get to visit the famous " .. npcRef(9201021) .. ". ");
		addText("What else are you curious about?");
		sendBackNext();
	end),
	makeChoiceHandler("What's the difference between normal and Premium Weddings?", function()
		addText("Well, the normal Wedding ticket is nice enough; however, the Premium Wedding ticket gives the newly married couple and their guests a Wedding Party afterwards at the Untamed Hearts Hunting Ground.");
		sendNext();

		addText("It's hosted by the legendary lady archer herself, " .. npcRef(9201021) .. ". ");
		addText("There's quite a few valuables there I'm told, though you'll see for yourself.");
		sendBackNext();
	end),
	makeChoiceHandler("What about Marriage Benefits?", function()
		addText("Now you're talking. ");
		addText("As a way to remember Elias the Hunter, Married Couples receive the rings, which produce an effect whenever you're near each other-it's different depending on the ring you choose. ");
		addText("You'll also get to do some activities that are for Couples only.");
		sendNext();

		addText("For instance, some of the townsfolk here will give you tasks that they wouldn't give others, they'll trust you a little more. ");
		addText("It's a different lifestyle, and you'll always be with your sweetheart. ");
		addText("What else can I assist with?");
		sendBackNext();
	end),
	makeChoiceHandler("How do I get my marriage annulled?", function()
		addText("Some marriages don't work out for the best. ");
		addText("It's unfortunate, but you can visit " .. npcRef(9201000) .. " to end your marriage. ");
		addText("Make sure you have a good amount of money as well. ");
		addText("Anything else?");
		sendNext();
	end),
	makeChoiceHandler("Ok, I understand the system. Thank you!", function()
		addText("My pleasure, friend! Visit me anytime if you need to know more.");
		sendOk();
		return iterationBreak;
	end),
};

while true do
	addText("Greetings Traveler! ");
	addText("I trust your adventures have proven lively thus far. ");
	addText("My name is Ames the Wise, and I'm the oldest citizen in Amoria. ");
	addText("I can offer a lifetime's worth of advice if you want to get married. ");
	addText("Would you like to know more?\r\n");

	addText(blue(choiceRef(choices)));
	choice = askChoice();

	if selectChoice(choices, choice) == iterationBreak then
		break;
	end
end
