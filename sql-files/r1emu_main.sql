use r1emu;

--
-- Database: `r1emu`
--

DELIMITER $$
--
-- Procedimientos
--
CREATE DEFINER=`root`@`localhost` PROCEDURE `bSetFamilyName`(IN `accountId` INT, IN `newName` VARCHAR(64))
    MODIFIES SQL DATA
    COMMENT 'set Family Name'
BEGIN
	DECLARE current_name varchar(64);

	DECLARE EXIT HANDLER FOR SQLEXCEPTION 
    BEGIN
          SET @flag = 1;
          ROLLBACK;
    END;

    START TRANSACTION;

    	SELECT family_name INTO current_name 
        FROM accounts
        WHERE family_name = newName
        LIMIT 1;
        
        IF current_name <> "" THEN
        	SET @flag = -1;
            ROLLBACK;
        ELSE
        	UPDATE accounts
            SET family_name = newName
            WHERE account_id = accountId;
        	SET @flag = 0;
        END IF;


    COMMIT;


END$$

DELIMITER ;

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `accounts`
--

CREATE TABLE IF NOT EXISTS `accounts` (
  `account_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `account_name` varchar(32) NOT NULL,
  `passwd` varchar(32) NOT NULL,
  `is_banned` enum('n','y') NOT NULL DEFAULT 'n',
  `time_banned` datetime NOT NULL,
  `credits` float NOT NULL DEFAULT '0',
  `privilege_level` int(11) NOT NULL DEFAULT '3',
  `time_last_login` datetime NOT NULL,
  `family_name` varchar(64) NOT NULL,
  `barrack_type` int(11) NOT NULL,
  PRIMARY KEY (`account_id`),
  KEY `acc_name-passwd` (`account_name`,`passwd`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `commanders`
--

CREATE TABLE IF NOT EXISTS `commanders` (
  `commander_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `commander_name` varchar(64) NOT NULL,
  `account_id` int(10) unsigned NOT NULL,
  `last_login` datetime NOT NULL,
  `last_logout` datetime NOT NULL,
  `time_created` datetime NOT NULL,
  `is_deleted` enum('n','y') NOT NULL DEFAULT 'n',
  `time_deleted` datetime NOT NULL,
  `gender` int(11) NOT NULL,
  `level` int(11) NOT NULL,
  `exp` int(11) NOT NULL,
  `job_id` int(11) NOT NULL,
  `class_id` int(11) NOT NULL,
  `hair_id` int(11) NOT NULL,
  `map_id` int(11) NOT NULL,
  `position_x` float NOT NULL,
  `position_y` float NOT NULL,
  `position_z` float NOT NULL,
  `hp` float NOT NULL,
  `mp` float NOT NULL,
  `eqslot_head_top` int(11) NOT NULL,
  `eqslot_head_middle` int(11) NOT NULL DEFAULT '0',
  `eqslot_unkown_1` int(11) NOT NULL DEFAULT '0',
  `eqslot_body_armor` int(11) NOT NULL DEFAULT '0',
  `eqslot_gloves` int(11) NOT NULL DEFAULT '0',
  `eqslot_boots` int(11) NOT NULL DEFAULT '0',
  `eqslot_helmet` int(11) NOT NULL DEFAULT '0',
  `eqslot_bracelet` int(11) NOT NULL DEFAULT '0',
  `eqslot_weapon` int(11) NOT NULL DEFAULT '0',
  `eqslot_shield` int(11) NOT NULL DEFAULT '0',
  `eqslot_costume` int(11) NOT NULL DEFAULT '0',
  `eqslot_unkown_3` int(11) NOT NULL,
  `eqslot_unkown_4` int(11) NOT NULL DEFAULT '0',
  `eqslot_unkown_5` int(11) NOT NULL DEFAULT '0',
  `eqslot_leg_armor` int(11) NOT NULL DEFAULT '0',
  `eqslot_unkown_6` int(11) NOT NULL DEFAULT '0',
  `eqslot_unkown_7` int(11) NOT NULL DEFAULT '0',
  `eqslot_ring_left` int(11) NOT NULL DEFAULT '0',
  `eqslot_ring_right` int(11) NOT NULL DEFAULT '0',
  `eqslot_necklace` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`commander_id`),
  KEY `account_id-is_deleted` (`account_id`,`is_deleted`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `items`
--

CREATE TABLE IF NOT EXISTS `items` (
  `item_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `commander_id` int(10) unsigned NOT NULL,
  `item_type` int(10) unsigned NOT NULL,
  `amount` int(10) unsigned NOT NULL,
  `position_in_inventory` int(11) NOT NULL,
  PRIMARY KEY (`item_id`),
  KEY `item_id-commander_id` (`item_id`,`commander_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;

