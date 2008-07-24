-- Roger: Apple Quest End
if state == 0 then
	addText("How easy is it to consume the item? Simple, right? You can set a #bhotkey#k on the right bottom slot. Haha you didn't know that! right? ");
	addText("Oh, and if you are a beginner, HP will automatically recover itself as time goes by. Well it takes time but this is one of the strategies for the beginners.");
	sendNext();
elseif state == 1 then
	addText("Alright! Now that you have learned alot, I will give you a present. This is a must for your travel in Maple World, so thank me! Please use this under emergency cases!");
	sendBackNext();
elseif state == 2 then
	addText("Okay, this is all I can teach you. I know it's sad but it is time to say good bye. Well tack care of yourself and Good luck my friend!\r\n\r\n");
	addText("#fUI/UIWindow.img/QuestIcon/4/0#\r\n#v2010000# 3 #t2010000#\r\n#v2010009# 3 #t2010009#\r\n\r\n#fUI/UIWindow.img/QuestIcon/8/0# 10 exp");	
	sendBackNext();
elseif state == 3 then
	endQuest(1021);
	giveItem(2010000, 3);
	giveItem(2010009, 3);
	giveEXP(10);
	endNPC();
end
