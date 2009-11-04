UPDATE `characters` SET guildid = 0 WHERE guildid IS NULL;
UPDATE `characters` SET allianceid = 0 WHERE allianceid IS NULL;
ALTER TABLE `users` CHANGE `ban_reason_message` `ban_reason_message` VARCHAR(255) NOT NULL DEFAULT '';
ALTER TABLE `characters` CHANGE `guildid` `guildid` int(11) NOT NULL DEFAULT '0';
ALTER TABLE `characters` CHANGE `allianceid` `allianceid` int(11) NOT NULL DEFAULT '0';
ALTER TABLE `characters` CHANGE `guildrank` `guildrank` tinyint(3) NOT NULL DEFAULT '5';
ALTER TABLE `characters` CHANGE `alliancerank` `alliancerank` tinyint(3) NOT NULL DEFAULT '5';