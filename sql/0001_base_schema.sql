CREATE TABLE `active_quests` (
  `id` bigint(20) NOT NULL AUTO_INCREMENT,
  `character_id` int(11) NOT NULL,
  `quest_id` smallint(16) NOT NULL,
  `mob_id` int(11) NOT NULL DEFAULT '0',
  `quantity_killed` int(11) NOT NULL DEFAULT '0',
  `data` varchar(40) DEFAULT NULL,
  PRIMARY KEY (`id`)
);

CREATE TABLE `buddylist` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `character_id` int(11) NOT NULL,
  `buddy_character_id` int(11) NOT NULL,
  `name` varchar(12) DEFAULT NULL,
  `group_name` varchar(13) DEFAULT 'Default Group',
  PRIMARY KEY (`id`),
  UNIQUE KEY `charid` (`character_id`,`buddy_character_id`)
);

CREATE TABLE `buddylist_pending` (
  `character_id` int(11) NOT NULL,
  `inviter_name` varchar(13) NOT NULL,
  `inviter_character_id` int(11) NOT NULL
);

CREATE TABLE `character_variables` (
  `character_id` int(11) NOT NULL,
  `key` varchar(255) NOT NULL,
  `value` varchar(255) NOT NULL,
  UNIQUE KEY `charid_2` (`character_id`,`key`),
  KEY `charid` (`character_id`)
);

CREATE TABLE `characters` (
  `character_id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(12) COLLATE latin1_general_ci NOT NULL,
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
  `ap` smallint(6) NOT NULL DEFAULT '0',
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
  `time_level` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  `overall_cpos` int(11) unsigned NOT NULL DEFAULT '0',
  `overall_opos` int(11) unsigned NOT NULL DEFAULT '0',
  `world_cpos` int(11) unsigned NOT NULL DEFAULT '0',
  `world_opos` int(11) unsigned NOT NULL DEFAULT '0',
  `job_cpos` int(11) unsigned NOT NULL DEFAULT '0',
  `job_opos` int(11) unsigned NOT NULL DEFAULT '0',
  `fame_cpos` int(11) unsigned NOT NULL DEFAULT '0',
  `fame_opos` int(11) unsigned NOT NULL DEFAULT '0',
  `book_cover` int(11) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`character_id`),
  KEY `userid` (`user_id`),
  KEY `world_id` (`world_id`),
  KEY `name` (`name`)
);

CREATE TABLE `completed_quests` (
  `character_id` int(11) NOT NULL,
  `quest_id` smallint(6) NOT NULL,
  `end_time` bigint(20) DEFAULT '0',
  PRIMARY KEY (`character_id`,`quest_id`)
);

CREATE TABLE `cooldowns` (
  `cooldown_id` int(11) NOT NULL AUTO_INCREMENT,
  `character_id` int(11) DEFAULT NULL,
  `skill_id` int(11) DEFAULT '0',
  `remaining_time` smallint(6) DEFAULT '0',
  PRIMARY KEY (`cooldown_id`),
  KEY `charid` (`character_id`)
);

CREATE TABLE `fame_log` (
  `fame_id` int(11) NOT NULL AUTO_INCREMENT,
  `from_character_id` int(11) NOT NULL,
  `to_character_id` int(11) NOT NULL,
  `fame_time` datetime NOT NULL,
  PRIMARY KEY (`fame_id`),
  KEY `from` (`from_character_id`,`to_character_id`,`fame_time`)
);

CREATE TABLE `ip_bans` (
  `ip_ban_id` int(11) NOT NULL AUTO_INCREMENT,
  `ip` varchar(15) NOT NULL,
  PRIMARY KEY (`ip_ban_id`)
);

CREATE TABLE `items` (
  `character_id` int(11) NOT NULL,
  `inv` smallint(6) NOT NULL,
  `slot` smallint(6) NOT NULL,
  `location` enum('inventory','storage') COLLATE latin1_general_ci NOT NULL,
  `user_id` int(11) NOT NULL,
  `world_id` int(11) NOT NULL,
  `item_id` int(11) NOT NULL,
  `amount` int(11) NOT NULL DEFAULT '1',
  `slots` tinyint(4) DEFAULT '7',
  `scrolls` smallint(6) DEFAULT '0',
  `istr` smallint(6) DEFAULT '0',
  `idex` smallint(6) DEFAULT '0',
  `iint` smallint(6) DEFAULT '0',
  `iluk` smallint(6) DEFAULT '0',
  `ihp` smallint(6) DEFAULT '0',
  `imp` smallint(6) DEFAULT '0',
  `iwatk` smallint(6) DEFAULT '0',
  `imatk` smallint(6) DEFAULT '0',
  `iwdef` smallint(6) DEFAULT '0',
  `imdef` smallint(6) DEFAULT '0',
  `iacc` smallint(6) DEFAULT '0',
  `iavo` smallint(6) DEFAULT '0',
  `ihand` smallint(6) DEFAULT '0',
  `ispeed` smallint(6) DEFAULT '0',
  `ijump` smallint(6) DEFAULT '0',
  `flags` tinyint(3) NOT NULL DEFAULT '0',
  `hammers` tinyint(3) NOT NULL DEFAULT '0',
  `pet_id` int(11) DEFAULT '0',
  `name` varchar(12) COLLATE latin1_general_ci NOT NULL,
  `expiration` bigint(23) NOT NULL DEFAULT '150842304000000000',
  PRIMARY KEY (`character_id`,`inv`,`slot`,`location`)
);

CREATE TABLE `keymap` (
  `character_id` int(11) NOT NULL,
  `pos` int(11) NOT NULL,
  `type` tinyint(3) unsigned NOT NULL,
  `action` int(11) NOT NULL,
  UNIQUE KEY `charid` (`character_id`,`pos`)
);

CREATE TABLE `logs` (
  `log_id` bigint(23) unsigned NOT NULL AUTO_INCREMENT,
  `log_time` datetime NOT NULL,
  `origin` enum('login','world','channel','cash','mts') NOT NULL,
  `info_type` enum('info','warning','debug','error','critical_error','server_connect','server_disconnect','server_auth_error','login','login_auth_error','logout','client_error','gm_command','admin_command','boss_kill','trade','shop_transaction','storage_transaction','instance_begin','drop','chat','whisper','malformed_packet','script_log') NOT NULL,
  `identifier` varchar(20) NOT NULL DEFAULT '',
  `message` text NOT NULL,
  PRIMARY KEY (`log_id`)
);

CREATE TABLE `monster_book` (
  `character_id` int(11) NOT NULL,
  `card_id` int(11) NOT NULL,
  `level` int(1) DEFAULT '1',
  PRIMARY KEY (`character_id`,`card_id`)
);

CREATE TABLE `mounts` (
  `character_id` int(11) NOT NULL,
  `mount_id` int(11) NOT NULL,
  `exp` smallint(6) DEFAULT '0',
  `level` int(3) unsigned NOT NULL DEFAULT '1',
  `tiredness` int(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`character_id`,`mount_id`),
  KEY `mountid` (`mount_id`)
);

CREATE TABLE `pets` (
  `pet_id` bigint(23) unsigned NOT NULL AUTO_INCREMENT,
  `index` tinyint(3) NOT NULL DEFAULT '-1',
  `name` varchar(12) NOT NULL,
  `level` tinyint(3) NOT NULL DEFAULT '1',
  `closeness` smallint(6) NOT NULL DEFAULT '0',
  `fullness` tinyint(3) NOT NULL DEFAULT '1',
  UNIQUE KEY `id` (`pet_id`)
);

CREATE TABLE `skill_macros` (
  `character_id` int(11) NOT NULL AUTO_INCREMENT,
  `pos` int(10) unsigned NOT NULL,
  `name` varchar(255) NOT NULL,
  `shout` tinyint(1) unsigned NOT NULL,
  `skill_1` int(11) DEFAULT NULL,
  `skill_2` int(11) DEFAULT NULL,
  `skill_3` int(11) DEFAULT NULL,
  PRIMARY KEY (`character_id`,`pos`)
);

CREATE TABLE `skills` (
  `character_id` int(11) NOT NULL,
  `skill_id` int(11) NOT NULL,
  `points` smallint(6) NOT NULL DEFAULT '1',
  `max_level` smallint(6) NOT NULL DEFAULT '0',
  UNIQUE KEY `charid_2` (`character_id`,`skill_id`),
  KEY `charid` (`character_id`)
);

CREATE TABLE `storage` (
  `user_id` int(11) NOT NULL,
  `world_id` int(11) NOT NULL,
  `slots` smallint(6) NOT NULL DEFAULT '4',
  `mesos` int(11) NOT NULL DEFAULT '0',
  `char_slots` int(11) NOT NULL DEFAULT '3',
  PRIMARY KEY (`user_id`,`world_id`)
);

CREATE TABLE `teleport_rock_locations` (
  `character_id` int(11) NOT NULL,
  `map_index` tinyint(3) NOT NULL,
  `map_id` int(11) NOT NULL DEFAULT '999999999',
  PRIMARY KEY (`character_id`,`map_index`)
);

CREATE TABLE `user_accounts` (
  `user_id` int(11) NOT NULL AUTO_INCREMENT,
  `username` varchar(20) COLLATE latin1_general_ci NOT NULL,
  `password` char(130) COLLATE latin1_general_ci NOT NULL,
  `salt` char(10) COLLATE latin1_general_ci DEFAULT NULL,
  `pin` int(4) unsigned DEFAULT NULL,
  `gender` tinyint(1) unsigned DEFAULT NULL,
  `gm_level` tinyint(4) NOT NULL DEFAULT '0',
  `admin` tinyint(1) NOT NULL DEFAULT '0',
  `char_delete_password` int(8) unsigned NOT NULL DEFAULT '0',
  `online` int(5) NOT NULL DEFAULT '0',
  `ban_expire` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  `ban_reason` tinyint(2) unsigned NOT NULL DEFAULT '0',
  `ban_reason_message` varchar(255) COLLATE latin1_general_ci NOT NULL DEFAULT '',
  `last_login` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  `quiet_ban_expire` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  `quiet_ban_reason` tinyint(3) NOT NULL DEFAULT '0',
  `creation_date` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  PRIMARY KEY (`user_id`),
  KEY `username` (`username`)
);

CREATE TABLE `vana_info` (
  `version` int(10) unsigned DEFAULT NULL
);