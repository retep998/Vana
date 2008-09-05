-- Door to Zakum
if getReactorsState(211042300, 2118002) == 1 then
	sendMessage("The battle with Zakum has already begun.", 5);
else
	setMap(211042400, "west00");
end
