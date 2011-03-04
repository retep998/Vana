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
-- Ames the Wise (Amoria)

while true do
	addText("Greetings Traveler! I trust your adventures have proven lively thus far. My name is Ames the Wise, and I'm the oldest citizen in Amoria. I can offer a lifetime's worth of advice if you want to get married. Would you like to know more?\r\n");
	addText("#b#L0#Yes, I'm interested in marriage.#l\r\n");
	addText("#L1#No, no...I'm definitely not ready for that. But I'd still like to see what Amoria's about. Do I have to be married?#l\r\n");
	addText("#L2#Ok, I think I'll talk to some people around here.#l\r\n");
	addText("#L3#How do I get engaged?#l\r\n");
	addText("#L4#How do I get married?#l\r\n");
	addText("#L5#I want to be married in the White Wedding Chapel!#l\r\n");
	addText("#L6#I want the wedding of my dreams in the Cathedral!#l\r\n");
	addText("#L7#How do I invite my friends?#l\r\n");
	addText("#L8#What happens after the Wedding?#l\r\n");
	addText("#L9#What's the difference between normal and Premium Weddings?#l\r\n");
	addText("#L10#What about Marriage Benefits?#l\r\n");
	addText("#L11#How do I get my marriage annulled?#l\r\n");
	addText("#L12#Ok, I understand the system. Thank you!#l#k");
	value = askChoice();
	if value == 0 then
		addText("Ready to get married, eh? First things first-congratulations! I'm happy for you. Second, you'll need to get engaged. After that, figure out where you want to get married-at the Cathedral or the White Wedding Chapel. Both are great places as far as I'm concerned.");
		sendNext();
		addText("You'll also need a Normal or Premium Wedding ticket from the Cash Shop for either location-just one per couple. After that, you'll be happily married and enjoy the benefits of marriage. I can explain a little more if you want...");
		sendBackNext();
	elseif value == 1 then
		addText("Amoria welcomes everyone. You do not have to married to help some of the good townspeople around here-or hunt. I invite you to speak with a few of them and see if you can assist them. ");
		sendNext();
	elseif value == 2 then
		addText("Great! Stay as long as you like. See me if you have some questions.");
		sendOk();
		break;
	elseif value == 3 then
		addText("Good thinking. You'll have to be male to obtain an Engagement Ring. After you have one, select the propose option, and wait for a response from your sweetheart. I recommend both of you being online so you can celebrate. As for getting an engagement ring, I'd speak with Moony, our ring-maker.");
		sendNext();
		addText("He's crafts the best rings I've ever seen-4 different kinds in fact. When you do select your engagement ring, please pick carefully-wouldn't want to make a mistake! What else is on your mind?");
		sendBackNext();
	elseif value == 4 then
		addText("Straight to the point, eh? I like that. Well, after you're engaged, you'll need to speak with Wedding Coordinator Victoria or Wayne and make your reservation. You will need a normal or Premium Wedding Ticket from the Cash Shop, an Engagement Ring and some time. If you've recently performed an annulment, then you'll need to wait 7 days before you can marry again and Moony will make you a new ring.");
		sendNext();
		addText("There's a bit more documentation for the Cathedral, they have a more elaborate system over there. Also, you'll need to tell the Wedding Coordinator your wish lists.");
		sendBackNext();
		addText("Pila Present will hold them for you when your friends turn them in, and you can pick them up afterwards. You'll also get wedding invitations to send them. Do you know where you want to get married?");
		sendBackNext();
	elseif value == 5 then
		addText("Bit of an adventurous streak, I see. Pelvis Bebop and his crew know how to put on a nice show. Just speak with Wedding Assistant Bonnie, and be sure you have a Wedding Receipt, an Engagement Ring and about 5-10 minutes.");
		sendNext();
		addText("The White Wedding Chapel is faster, and a little more care-free. Anything else you're interested in?");
		sendBackNext();
	elseif value == 6 then
		addText("Bit of an adventurous streak, I see. Pelvis Bebop and his crew know how to put on a nice show. Just speak with Wedding Assistant Bonnie, and be sure you have a Wedding Receipt, an Engagement Ring and about 5-10 minutes.");
		sendNext();
		addText("The White Wedding Chapel is faster, and a little more care-free. Anything else you're interested in?");
		sendBackNext();
	elseif value == 7 then
		addText("Ah, the Cathedral. If there's a place you want to really want to get married in style, that's it. As I mentioned before, they have a more elaborate system, so you'll need to speak with High Priest John for his permission.");
		sendNext();
		addText("The bride-to-be will also have to call on Mom and Dad to vouch for you both. To get married in the Cathedral, Just speak with Wedding Assistant Nicole and be sure you have a Wedding Receipt, the Officiator's Permission, and an Engagement Ring and about 10-20 minutes.");
		sendBackNext();
		addText("Also, the Cathedral allows your guests to give the married couple an experience blessing during the wedding-1 exp per click, in fact. What else would you like to know?");
		sendBackNext();
	elseif value == 8 then
		addText("You will receive Wedding Invitations along with your Wedding ticket. If you need more, you can talk with one of the Wedding Assistants. The invitations are pretty simple, you just type in your friend's name, hit 'Send' and off they go.");
		sendNext();
		addText("They'll land in the Etc slot-make sure they have a few spaces free. What else would you like to know?");
		sendBackNext();
	elseif value == 9 then
		addText("For the White Wedding Chapel and the cathedral, after the Wedding ends, you and your guests are sent to the photo area, Cherished Visage. You can snap away for 60 seconds, and then relax with the new couple for 5 minutes.");
		sendNext();
		addText("After that, you're whisked back to Amoria. Unless you have a premium ticket, in which case you get to visit the famous Robin the Huntress. What else are you curious about?");
		sendBackNext();
	elseif value == 10 then
		addText("Well, the normal Wedding ticket is nice enough; however, the Premium Wedding ticket gives the newly married couple and their guests a Wedding Party afterwards at the Untamed Hearts Hunting Ground.");
		sendNext();
		addText("It's hosted by the legendary lady archer herself, Robin the Huntress. There's quite a few valuables there I'm told, though you'll see for yourself.");
		sendBackNext();
	elseif value == 11 then
		addText("Now you're talking. As a way to remember Elias the Hunter, Married Couples receive the rings, which produce an effect whenever you're near each other-it's different depending on the ring you choose. You'll also get to do some activities that are for Couples only.");
		sendNext();
		addText("For instance, some of the townsfolk here will give you tasks that they wouldn't give others, they'll trust you a little more. It's a different lifestyle, and you'll always be with your sweetheart. What else can I assist with?");
		sendBackNext();
	elseif value == 12 then
		addText("Some marriages don't work out for the best. It's unfortunate, but you can visit Moony to end your marriage. Make sure you have a good amount of money as well. Anything else?");
		sendNext();
		addText("My pleasure, friend! Visit me anytime if you need to know more.");
		sendOk();
		break;
	end
end
