ALTER TABLE `users` ADD `quiet_ban_reason` tinyint(3) DEFAULT '0' NOT NULL AFTER `quiet_ban`;
ALTER TABLE `users` CHANGE `quiet_ban` `quiet_ban_expire` DATETIME NOT NULL DEFAULT '0000-00-00 00:00:00'