ALTER TABLE `items` MODIFY `charid` INT NOT NULL FIRST;
ALTER TABLE `items` MODIFY `inv` tinyint(4) NOT NULL AFTER `charid`;
ALTER TABLE `items` MODIFY `pos` smallint(6) NOT NULL AFTER `inv`;
