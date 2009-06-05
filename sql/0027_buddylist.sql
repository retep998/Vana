CREATE TABLE `buddylist` (
  `id` INTEGER UNSIGNED NOT NULL AUTO_INCREMENT,
  `charid` INTEGER NOT NULL,
  `buddy_charid` INTEGER NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE (`charid`, `buddy_charid`)
)
