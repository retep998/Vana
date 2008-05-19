if state == 0 then
	addText("Hey there, Pretty~ I am Roger who teachs you adroable new Maplers with lots of information.");
	sendNext();
elseif state == 1 then
	addText("I know you are busy! Please spare me some time~ I can teach you some useful information! Ahahaha!");
	sendBackNext();
elseif state == 2 then
	addText("So..... Let me just do this for fun! Abaracadabra~!");
	sendAcceptDecline();
elseif state == 3 then
	if getSelected() == 1 then
		setHP(25);
		giveItem(2010007, 1);
		addQuest(1021);
		setState(state+1);
		addText("Surprised? If HP becomes 0, then you are in trouble. Now, I will give you #r#t2010007##k. Please take it. ");
		addText("You will feel stronger. Open the Item window and double click to consume. Hey, It's very simple to open the Item window. Just press #bI#k on your keyboard.");
		sendNext();
	else
		addText("I can't believe you just have turned down a attractive guys like me!");
		sendNext();
		endNPC();
	end
elseif state == 4 then
	addText("Surprised? If HP becomes 0, then you are in trouble. Now, I will give you #r#t2010007##k. Please take it. ");
	addText("You will feel stronger. Open the Item window and double click to consume. Hey, It's very simple to open the Item window. Just press #bI#k on your keyboard.");
	sendNext();
elseif state == 5 then
	addText("Please take all #t2010007#s that I gave you. You will be able to see the HP bar increasing. "); 
	addText("Please talk to me again when you recover your HP 100%");
	sendBackOK();
elseif state == 6 then
	endNPC();
end