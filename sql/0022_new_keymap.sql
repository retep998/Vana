DROP TABLE IF EXISTS `keymap`;
CREATE TABLE `keymap` (
`charid` INT NOT NULL ,
`pos` INT NOT NULL ,
`type` TINYINT UNSIGNED NOT NULL ,
`action` INT NOT NULL,
unique key (`charid`, `pos`)
);
