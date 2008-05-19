map = getMap();
if map == 0 then
	if state == 0 then
		addText("Welcome to the world of MapleStory. The purpose of this training camp is to ");
		addText("help beginners. Would you like to enter this training camp? Some people start thier journey ");
		addText(" without taking the training program. But I strongly recommend you take the training program first.");
		sendYesNo();
	elseif state == 1 then
		if getSelected() == 1 then
			addText("Ok then, I will let you enter the training camp. Please follow your instructor's lead.");
			sendNext();
		else
			addText("Do you really wanted to start your journey right away?");
			setState(state+1);
			sendYesNo();
		end
	elseif state == 2 then
		setMap(1);
		endNPC();
	elseif state == 3 then
		if getSelected() == 1 then
			addText("It seems like you want to start your journey without taking the ");
			addText("training program. Then, I will let you move on the training ground. Be careful~");
			sendNext();

		else
			addText("Please talk to me again when you finally made your decision.");
			sendNext();
			endNPC();
		end
	elseif state == 4 then
		setMap(40000);
		endNPC();
	end
elseif map == 1 then
	if state == 0 then
		addText("The is the image room where your first training program begins. ");
		addText("In this room, you will have an advance look into the job of your choice. ");
		sendNext();
	elseif state == 1 then
		addText("Once you train hard enough, you will be entitled to occupy a job. ");
		addText("You can become a Bowman in Henesys, a Magician in Ellinia, a Warrior in Perion, and a Thief in Kerening City..");
		sendBackOK();
	else
		endNPC();
	end
else
	endNPC();
end
