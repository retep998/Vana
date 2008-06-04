ALTER TABLE `equip` MODIFY `charid` INT NOT NULL FIRST;
ALTER TABLE `equip` MODIFY `pos` smallint(6) NOT NULL AFTER `charid`;
