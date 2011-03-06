RENAME TABLE `storageitems` TO `storage_items`;
ALTER TABLE `items` ADD `expiration` DATETIME NULL DEFAULT '2079-01-01 00:00:00';