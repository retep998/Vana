ALTER TABLE `characters` CHANGE `i_equip_s` `equip_slots` INT NOT NULL DEFAULT 24;
ALTER TABLE `characters` CHANGE `i_use_s` `use_slots` INT NOT NULL DEFAULT 24;
ALTER TABLE `characters` CHANGE `i_setup_s` `setup_slots` INT NOT NULL DEFAULT 24;
ALTER TABLE `characters` CHANGE `i_etc_s` `etc_slots` INT NOT NULL DEFAULT 24;
ALTER TABLE `characters` CHANGE `i_cash_s` `cash_slots` INT NOT NULL DEFAULT 48;