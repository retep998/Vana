CREATE TABLE `%%PREFIX%%user_accounts` (
  `user_id` int(11) NOT NULL AUTO_INCREMENT,
  `username` varchar(20) NOT NULL,
  `password` char(130) NOT NULL,
  `salt` char(10) NULL,
  `pin` int(4) unsigned NULL,
  `gender` tinyint(1) unsigned NULL,
  `gm_level` tinyint(4) NULL,
  `admin` tinyint(1) NULL,
  `char_delete_password` int(8) unsigned NULL,
  `online` int(5) NOT NULL DEFAULT '0',
  `banned` tinyint(1) NOT NULL DEFAULT '0',
  `ban_expire` datetime NULL,
  `ban_reason` tinyint(2) unsigned NULL,
  `ban_reason_message` varchar(255) NULL,
  `last_login` datetime NULL,
  `quiet_ban_expire` datetime NULL,
  `quiet_ban_reason` tinyint(3) NULL,
  `creation_date` datetime NOT NULL,
  PRIMARY KEY (`user_id`),
  KEY `username` (`username`)
) DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci;

CREATE TABLE `%%PREFIX%%buddylist_pending` (
  `character_id` int(11) NOT NULL,
  `inviter_name` varchar(13) NOT NULL,
  `inviter_character_id` int(11) NOT NULL
) DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci;

CREATE TABLE `%%PREFIX%%fame_log` (
  `fame_id` int(11) NOT NULL AUTO_INCREMENT,
  `from_character_id` int(11) NOT NULL,
  `to_character_id` int(11) NOT NULL,
  `fame_time` datetime NOT NULL,
  PRIMARY KEY (`fame_id`),
  KEY `from` (`from_character_id`,`to_character_id`,`fame_time`)
) DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci;

CREATE TABLE `%%PREFIX%%ip_bans` (
  `ip_ban_id` int(11) NOT NULL AUTO_INCREMENT,
  `ip` varchar(15) NOT NULL,
  PRIMARY KEY (`ip_ban_id`)
) DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci;

CREATE TABLE `%%PREFIX%%logs` (
  `log_id` bigint(23) unsigned NOT NULL AUTO_INCREMENT,
  `log_time` datetime NOT NULL,
  `origin` enum('login','world','channel','cash','mts') NOT NULL,
  `info_type` enum('info','warning','debug','error','critical_error','server_connect','server_disconnect','server_auth_error','login','login_auth_error','logout','client_error','gm_command','admin_command','boss_kill','trade','shop_transaction','storage_transaction','instance_begin','drop','chat','whisper','malformed_packet','script_log','ban','unban') NOT NULL,
  `identifier` varchar(20) NULL,
  `message` text NOT NULL,
  PRIMARY KEY (`log_id`)
) DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci;

CREATE TABLE `%%PREFIX%%storage` (
  `user_id` int(11) NOT NULL,
  `world_id` int(11) NOT NULL,
  `slots` smallint(6) NOT NULL,
  `mesos` int(11) NOT NULL,
  `char_slots` int(11) NOT NULL,
  PRIMARY KEY (`user_id`,`world_id`),
  FOREIGN KEY (`user_id`) REFERENCES `%%PREFIX%%user_accounts`(`user_id`)
) DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci;

CREATE TABLE `%%PREFIX%%characters` (
  `character_id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(12) NOT NULL,
  `user_id` int(11) NOT NULL,
  `world_id` tinyint(3) unsigned NOT NULL,
  `level` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `job` smallint(6) NOT NULL DEFAULT '0',
  `str` smallint(6) NOT NULL DEFAULT '4',
  `dex` smallint(6) NOT NULL DEFAULT '4',
  `int` smallint(6) NOT NULL DEFAULT '4',
  `luk` smallint(6) NOT NULL DEFAULT '4',
  `chp` smallint(6) NOT NULL DEFAULT '50',
  `mhp` smallint(6) NOT NULL DEFAULT '50',
  `cmp` smallint(6) NOT NULL DEFAULT '5',
  `mmp` smallint(6) NOT NULL DEFAULT '5',
  `hpmp_ap` int(11) NOT NULL DEFAULT '0',
  `ap` smallint(6) NOT NULL DEFAULT '9',
  `sp` smallint(6) NOT NULL DEFAULT '0',
  `exp` int(11) NOT NULL DEFAULT '0',
  `fame` smallint(6) NOT NULL DEFAULT '0',
  `map` int(11) NOT NULL DEFAULT '0',
  `pos` smallint(6) NOT NULL DEFAULT '0',
  `gender` tinyint(1) NOT NULL,
  `skin` tinyint(4) NOT NULL,
  `eyes` int(11) NOT NULL,
  `hair` int(11) NOT NULL,
  `mesos` int(11) NOT NULL DEFAULT '0',
  `equip_slots` int(11) NOT NULL DEFAULT '24',
  `use_slots` int(11) NOT NULL DEFAULT '24',
  `setup_slots` int(11) NOT NULL DEFAULT '24',
  `etc_slots` int(11) NOT NULL DEFAULT '24',
  `cash_slots` int(11) NOT NULL DEFAULT '48',
  `buddylist_size` int(3) unsigned NOT NULL DEFAULT '20',
  `online` tinyint(1) NOT NULL DEFAULT '0',
  `time_level` datetime NULL,
  `overall_cpos` int(11) unsigned NULL,
  `overall_opos` int(11) unsigned NULL,
  `world_cpos` int(11) unsigned NULL,
  `world_opos` int(11) unsigned NULL,
  `job_cpos` int(11) unsigned NULL,
  `job_opos` int(11) unsigned NULL,
  `fame_cpos` int(11) unsigned NULL,
  `fame_opos` int(11) unsigned NULL,
  `book_cover` int(11) unsigned NULL,
  PRIMARY KEY (`character_id`),
  KEY `userid` (`user_id`),
  KEY `world_id` (`world_id`),
  KEY `name` (`name`),
  FOREIGN KEY (`user_id`) REFERENCES `%%PREFIX%%user_accounts`(`user_id`) ON DELETE CASCADE
) DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci;

CREATE TABLE `%%PREFIX%%active_quests` (
  `id` bigint(20) NOT NULL AUTO_INCREMENT,
  `character_id` int(11) NOT NULL,
  `quest_id` smallint(16) NOT NULL,
  `data` varchar(40) NULL,
  PRIMARY KEY (`id`),
  FOREIGN KEY (`character_id`) REFERENCES `%%PREFIX%%characters`(`character_id`) ON DELETE CASCADE
) DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci;

CREATE TABLE `%%PREFIX%%active_quests_mobs` (
	`id` bigint(20) NOT NULL AUTO_INCREMENT,
	`active_quest_id` bigint(20) NOT NULL,
	`mob_id` int(11) NOT NULL,
	`quantity_killed` int(11) NOT NULL,
	PRIMARY KEY (`id`),
	FOREIGN KEY (`active_quest_id`) REFERENCES `%%PREFIX%%active_quests`(`id`) ON DELETE CASCADE
) DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci;

CREATE TABLE `%%PREFIX%%buddylist` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `character_id` int(11) NOT NULL,
  `buddy_character_id` int(11) NOT NULL,
  `name` varchar(12) NULL,
  `group_name` varchar(13) NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `charid` (`character_id`,`buddy_character_id`),
  FOREIGN KEY (`character_id`) REFERENCES `%%PREFIX%%characters`(`character_id`) ON DELETE CASCADE
) DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci;

CREATE TABLE `%%PREFIX%%character_variables` (
  `character_id` int(11) NOT NULL,
  `key` varchar(255) NOT NULL,
  `value` varchar(255) NOT NULL,
  UNIQUE KEY `charid_2` (`character_id`,`key`),
  KEY `charid` (`character_id`),
  FOREIGN KEY (`character_id`) REFERENCES `%%PREFIX%%characters`(`character_id`) ON DELETE CASCADE
) DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci;

CREATE TABLE `%%PREFIX%%completed_quests` (
  `character_id` int(11) NOT NULL,
  `quest_id` smallint(6) NOT NULL,
  `end_time` bigint(20) NOT NULL,
  PRIMARY KEY (`character_id`,`quest_id`),
  FOREIGN KEY (`character_id`) REFERENCES `%%PREFIX%%characters`(`character_id`) ON DELETE CASCADE
) DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci;

CREATE TABLE `%%PREFIX%%cooldowns` (
  `cooldown_id` int(11) NOT NULL AUTO_INCREMENT,
  `character_id` int(11) NOT NULL,
  `skill_id` int(11) NOT NULL,
  `remaining_time` smallint(6) NOT NULL,
  PRIMARY KEY (`cooldown_id`),
  KEY `charid` (`character_id`),
  FOREIGN KEY (`character_id`) REFERENCES `%%PREFIX%%characters`(`character_id`) ON DELETE CASCADE
) DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci;

CREATE TABLE `%%PREFIX%%pets` (
  `pet_id` bigint(23) unsigned NOT NULL AUTO_INCREMENT,
  `index` tinyint(3) NULL,
  `name` varchar(12) NOT NULL,
  `level` tinyint(3) NOT NULL DEFAULT '1',
  `closeness` smallint(6) NOT NULL DEFAULT '0',
  `fullness` tinyint(3) NOT NULL DEFAULT '1',
  PRIMARY KEY (`pet_id`)
) DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci;

CREATE TABLE `%%PREFIX%%items` (
  `character_id` int(11) NOT NULL,
  `inv` smallint(6) NOT NULL,
  `slot` smallint(6) NOT NULL,
  `location` enum('inventory','storage') NOT NULL,
  `user_id` int(11) NOT NULL,
  `world_id` int(11) NOT NULL,
  `item_id` int(11) NOT NULL,
  `amount` int(11) NOT NULL DEFAULT '1',
  `slots` tinyint(4) NULL,
  `scrolls` smallint(6) NULL,
  `istr` smallint(6) NULL,
  `idex` smallint(6) NULL,
  `iint` smallint(6) NULL,
  `iluk` smallint(6) NULL,
  `ihp` smallint(6) NULL,
  `imp` smallint(6) NULL,
  `iwatk` smallint(6) NULL,
  `imatk` smallint(6) NULL,
  `iwdef` smallint(6) NULL,
  `imdef` smallint(6) NULL,
  `iacc` smallint(6) NULL,
  `iavo` smallint(6) NULL,
  `ihand` smallint(6) NULL,
  `ispeed` smallint(6) NULL,
  `ijump` smallint(6) NULL,
  `flags` tinyint(3) NULL,
  `hammers` tinyint(3) NULL,
  `pet_id` BIGINT(23) UNSIGNED NULL,
  `name` varchar(12) NULL,
  `expiration` bigint(23) NULL,
  PRIMARY KEY (`character_id`,`inv`,`slot`,`location`),
  FOREIGN KEY (`character_id`) REFERENCES `%%PREFIX%%characters`(`character_id`) ON DELETE CASCADE,
  FOREIGN KEY (`pet_id`) REFERENCES `%%PREFIX%%pets`(`pet_id`) ON DELETE CASCADE
) DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci;

CREATE TABLE `%%PREFIX%%keymap` (
  `character_id` int(11) NOT NULL,
  `pos` int(11) NOT NULL,
  `type` tinyint(3) unsigned NOT NULL,
  `action` int(11) NOT NULL,
  UNIQUE KEY `charid` (`character_id`,`pos`),
  FOREIGN KEY (`character_id`) REFERENCES `%%PREFIX%%characters`(`character_id`) ON DELETE CASCADE
) DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci;

CREATE TABLE `%%PREFIX%%monster_book` (
  `character_id` int(11) NOT NULL,
  `card_id` int(11) NOT NULL,
  `level` int(1) DEFAULT '1',
  PRIMARY KEY (`character_id`,`card_id`),
  FOREIGN KEY (`character_id`) REFERENCES `%%PREFIX%%characters`(`character_id`) ON DELETE CASCADE
) DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci;

CREATE TABLE `%%PREFIX%%mounts` (
  `character_id` int(11) NOT NULL,
  `mount_id` int(11) NOT NULL,
  `exp` smallint(6) DEFAULT '0',
  `level` int(3) unsigned NOT NULL DEFAULT '1',
  `tiredness` int(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`character_id`,`mount_id`),
  KEY `mountid` (`mount_id`),
  FOREIGN KEY (`character_id`) REFERENCES `%%PREFIX%%characters`(`character_id`) ON DELETE CASCADE
) DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci;

CREATE TABLE `%%PREFIX%%skill_macros` (
  `character_id` int(11) NOT NULL AUTO_INCREMENT,
  `pos` int(10) unsigned NOT NULL,
  `name` varchar(255) NOT NULL,
  `shout` tinyint(1) unsigned NOT NULL,
  `skill_1` int(11) NULL,
  `skill_2` int(11) NULL,
  `skill_3` int(11) NULL,
  PRIMARY KEY (`character_id`,`pos`),
  FOREIGN KEY (`character_id`) REFERENCES `%%PREFIX%%characters`(`character_id`) ON DELETE CASCADE
) DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci;

CREATE TABLE `%%PREFIX%%skills` (
  `character_id` int(11) NOT NULL,
  `skill_id` int(11) NOT NULL,
  `points` smallint(6) NOT NULL DEFAULT '1',
  `max_level` smallint(6) NOT NULL,
  UNIQUE KEY `charid_2` (`character_id`,`skill_id`),
  KEY `charid` (`character_id`),
  FOREIGN KEY (`character_id`) REFERENCES `%%PREFIX%%characters`(`character_id`) ON DELETE CASCADE
) DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci;

CREATE TABLE `%%PREFIX%%teleport_rock_locations` (
  `character_id` int(11) NOT NULL,
  `map_index` tinyint(3) NOT NULL,
  `map_id` int(11) NOT NULL,
  PRIMARY KEY (`character_id`,`map_index`),
  FOREIGN KEY (`character_id`) REFERENCES `%%PREFIX%%characters`(`character_id`) ON DELETE CASCADE
) DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci;