DROP PROCEDURE IF EXISTS gogo9;

/* START BULK QUERY */

CREATE PROCEDURE gogo9()
BEGIN
	SET @v_worlds = 0;
	SET @v_maxLevel = 200;
	
	WHILE @v_worlds <= 20 DO
	BEGIN
		SET @v_curJobTrack = 1;
		REPEAT
		BEGIN
			IF @v_curJobTrack >= 10 AND @v_curJobTrack < 20 THEN
				/* KoC jobs */
				SET @v_maxLevel = 120;
			ELSE
				/* Adventurer, Aran and Evan jobs */
				SET @v_maxLevel = 200;
			END IF;
			
			INSERT INTO imitate_characters (id, charid, job, gender, skin, eyes, hair)
			SELECT NULL, chara.ID, chara.job, chara.gender, chara.skin, chara.eyes, chara.hair 
			FROM characters chara 
			WHERE chara.level = @v_maxLevel AND chara.world_id = @v_worlds AND FLOOR(chara.job / 100) = @v_curJobTrack ORDER BY time_level DESC LIMIT 20;
			
			SET @v_curJobTrack = @v_curJobTrack + 1;
		END;
		UNTIL @v_curJobTrack = 30 END REPEAT;
		SET @v_worlds = @v_worlds + 1;
	END;
	END WHILE;
END;

/* END BULK QUERY */

CREATE TABLE IF NOT EXISTS imitate_characters (
  `id` INT(11) NOT NULL AUTO_INCREMENT,
  `charid` INT(11) NOT NULL,
  `job` INT(11) NOT NULL,
  `gender` TINYINT(1) NOT NULL,
  `skin` TINYINT(1) NOT NULL,
  `eyes` INT(11) NOT NULL,
  `hair` INT(11) NOT NULL,
  PRIMARY KEY (`id`,`charid`)
) ENGINE=MYISAM AUTO_INCREMENT=1 DEFAULT CHARSET=latin1;

CREATE TABLE IF NOT EXISTS imitate_items (
  `charid` INT(11) NOT NULL,
  `slot` SMALLINT(6) NOT NULL,
  `itemid` INT(11) NOT NULL,
  PRIMARY KEY (`charid`,`slot`)
) ENGINE=MYISAM DEFAULT CHARSET=latin1 COLLATE=latin1_general_ci;

CALL gogo9();

DROP PROCEDURE IF EXISTS gogo9;

REPLACE INTO imitate_items (charid, slot, itemid) 
SELECT items.charid, items.slot, items.itemid FROM items JOIN imitate_characters ON (imitate_characters.charid = items.charid) WHERE items.slot < 0;