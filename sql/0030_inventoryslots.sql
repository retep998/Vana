ALTER TABLE `characters` ADD `i_equip_s` INT NOT NULL DEFAULT 24 AFTER `mesos`;
ALTER TABLE `characters` ADD `i_use_s` INT NOT NULL DEFAULT 24 AFTER `i_equip_s`;
ALTER TABLE `characters` ADD `i_setup_s` INT NOT NULL DEFAULT 24 AFTER `i_use_s`;
ALTER TABLE `characters` ADD `i_etc_s` INT NOT NULL DEFAULT 24 AFTER `i_setup_s`;
ALTER TABLE `characters` ADD `i_cash_s` INT NOT NULL DEFAULT 48 AFTER `i_etc_s`;