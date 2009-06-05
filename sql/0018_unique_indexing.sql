ALTER TABLE `character_variables` ADD UNIQUE INDEX (`charid`,`key`);
ALTER TABLE `skills` CHANGE points points INT NOT NULL DEFAULT 0;
ALTER TABLE `skills` CHANGE charid charid INT NOT NULL;
ALTER TABLE `skills` CHANGE skillid skillid INT NOT NULL;
ALTER TABLE `skills` ADD UNIQUE INDEX (`charid`,`skillid`);
ALTER TABLE `equip` ADD COLUMN `id` INT NOT NULL auto_increment primary key FIRST;
ALTER TABLE `items` ADD COLUMN `id` INT NOT NULL auto_increment primary key FIRST;
