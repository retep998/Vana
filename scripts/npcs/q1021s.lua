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
-- Roger: Apple Quest start

if getGender() == 0 then
	addText("Hey, Man~  What's up? Haha!  I am #p2000# who can teach you adorable new Maplers lots of information.");
	sendNext();

	addText("You are asking who made me do this?  Ahahahaha!  Myself!  I wanted to do this and just be kind to you new travellers.");
	sendBackNext();
else
	addText("Hey there, Pretty~ I am #p2000# who teaches you adorable new Maplers lots of information.");
	sendNext();

	addText("I know you are busy! Please spare me some time~ I can teach you some useful information! Ahahaha!");
	sendBackNext();
end


addText("So.....  Let me just do this for fun! Abaracadabra~!");
accept = askAcceptDecline();

if accept == 1 then
	setHP(getHP() / 2);
	giveItem(2010007, 1);
	addQuest(1021);
	addText("Surprised?  If HP becomes 0, then you are in trouble.  Now, I will give you #r#t2010007##k.  Please take it.  You will feel stronger.  Open the Item window and double click to consume.  Hey, It's very simple to open the Item window.  Just press #bI#k on your keyboard.#I");
	sendNext();

	addText("Please take all #t2010007#s that I gave you.  You will be able to see the HP bar increasing.  Please talk to me again when you recover your HP 100%#I ");
	sendBackOK();
else
	if getGender() == 0 then
		addText("I can't believe an attractive guy like myself, got turned down!");
		sendNext();
	else
		addText("I can't believe you have just turned down an attractive guy like me!");
		sendNext();
	end
end