ALTER TABLE `characters` ADD `world_id` TINYINT( 1 ) UNSIGNED NOT NULL AFTER `userid` ;

ALTER TABLE `characters` ADD INDEX ( `world_id` ) ;
