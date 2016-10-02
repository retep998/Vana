-- Inter-server password, change this or others will be able to hack your server
inter_password = "changeme";

-- Inter-server salt, an arbitrary string used to hash the password more securely
inter_salt = "changeme";

-- Use encryption to communicate with clients?
use_client_encryption = true;

-- Ping inter-server connections? This should generally remain enabled, but it's useful for using a debugger
inter_ping = {
	["enabled"] = true,
	["inital_delay"] = 60000,
	["interval"] = 30000,
	["timeout_ping_count"] = 4,
};

-- Ping clients? This should generally remain enabled, but it's useful for using a debugger
client_ping = {
	["enabled"] = true,
	["inital_delay"] = 60000,
	["interval"] = 30000,
	["timeout_ping_count"] = 4,
};

-- What IP and port should the server use to connect to the LoginServer?
login_ip = "127.0.0.1";
login_inter_port = 8485;

-- External IP configuration
-- The server will try to check if the client and the server are on the same subnet
-- If the client is, the IP is sent to the client
-- Otherwise, the next subnet is parsed from top-to-bottom

-- An example configuration:
--	external_ip = {
--		{["ip"] = "127.0.0.1", ["mask"] = "255.0.0.0"},
--		{["ip"] = "192.168.1.2", ["mask"] = "255.255.255.0"},
--		{["ip"] = "your.wan.ip", ["mask"] = "0.0.0.0"}
--	};
-- This rule reads:
-- If the client is in the 127.x.x.x block, send 127.0.0.1 to the client
-- If the client is in the 192.168.1.x block, send 192.168.1.2 to the client
-- Otherwise, send your.wan.ip to the client

-- The 127.0.0.1 rule is needed so that the ChannelServer knows how to connect to the WorldServer
-- Domain names are supported in the IP field
-- By default, only the local machine can access your server

function makeIp(ip, mask)
	return {
		["ip"] = ip,
		["mask"] = mask,
	};
end

external_ip = {
	makeIp("127.0.0.1", "255.0.0.0"),
};