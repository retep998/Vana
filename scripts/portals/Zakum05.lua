-- Door to Zakum
if getReactorsState(211042300, 2118002) == 1 then
	sendMessage("The battle with Zakum has already begun.", 5);
	setPortalToId(-1);
else
	setPortalToId(211042400);
	setPortalTo("west00");
end
