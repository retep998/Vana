DROP TABLE `keymap`;
CREATE TABLE `keymap` (
`charid` INT NOT NULL ,
`pos` INT NOT NULL ,
`type` TINYINT UNSIGNED NOT NULL ,
`action` INT NOT NULL
);
ALTER TABLE `keymap` ADD UNIQUE (
`charid` ,
`pos`
) ;
