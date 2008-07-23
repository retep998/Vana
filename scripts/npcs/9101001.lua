if state == 0 then
	addText("You have finished all your trainings. Good job. ");
	addText("You seem to be ready to start with the journey right away! Good , I will let you on to the next place.");
	sendNext();
elseif state == 1 then
	addText("But remember, once you get out of here, you will enter a village full with monsters. Well them, good bye!");
	sendBackNext();
elseif state == 2 then
	setMap(40000);
	endNPC();
end
