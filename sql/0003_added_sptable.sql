CREATE TABLE `character_sp`(  
  `character_id` INT NOT NULL,
  `job_id` TINYINT NOT NULL,
  `points` TINYINT NOT NULL DEFAULT 0,
  UNIQUE INDEX `character_id__job_id` (`character_id`, `job_id`),
  CONSTRAINT `character_id_fk` FOREIGN KEY (`character_id`) REFERENCES `maplestory`.`characters`(`character_id`) ON DELETE CASCADE
);
