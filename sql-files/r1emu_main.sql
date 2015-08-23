--
-- Database: `r1emu`
--

-- --------------------------------------------------------

--
-- Table structure for table `barrack`
--

CREATE TABLE IF NOT EXISTS `barrack` (
  `account_id` int(11) NOT NULL DEFAULT '0',
  `barrack_name` varchar(63) NOT NULL,
  `type` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`account_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Table structure for table `commander`
--

CREATE TABLE IF NOT EXISTS `commander` (
  `commander_id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `account_id` int(11) unsigned NOT NULL DEFAULT '0',
  `commander_num` smallint(2) NOT NULL DEFAULT '0',
  `name` varchar(64) NOT NULL,
  `family_name` varchar(64) NOT NULL,
  `class` int(11) NOT NULL DEFAULT '0',
  `gender` tinyint(1) NOT NULL DEFAULT '0',
  `level` int(11) NOT NULL DEFAULT '1',
  `head_top` int(11) NOT NULL DEFAULT '0',
  `head_middle` int(11) NOT NULL DEFAULT '0',
  `necklace` int(11) NOT NULL DEFAULT '0',
  `body_armor` int(11) NOT NULL DEFAULT '0',
  `leg_armor` int(11) NOT NULL DEFAULT '0',
  `gloves` int(11) NOT NULL DEFAULT '0',
  `shoes` int(11) NOT NULL DEFAULT '0',
  `weapon` int(11) NOT NULL DEFAULT '0',
  `shield` int(11) NOT NULL DEFAULT '0',
  `costume` int(11) NOT NULL DEFAULT '0',
  `ring` int(11) NOT NULL DEFAULT '0',
  `bracelet_left` int(11) NOT NULL DEFAULT '0',
  `bracelet_right` int(11) NOT NULL DEFAULT '0',
  `hair_type` int(11) NOT NULL DEFAULT '0',
  `last_map` int(11) NOT NULL DEFAULT '1032',
  `last_x` int(11) NOT NULL DEFAULT '0',
  `last_y` int(11) NOT NULL DEFAULT '0',
  `current_xp` int(11) NOT NULL DEFAULT '0',
  `pose` int(11) NOT NULL,
  `current_hp` int(11) NOT NULL DEFAULT '0',
  `current_sp` mediumint(5) NOT NULL DEFAULT '0',
  `str` int(11) NOT NULL DEFAULT '0',
  `vit` int(11) NOT NULL DEFAULT '0',
  `int` int(11) NOT NULL DEFAULT '0',
  `spi` int(11) NOT NULL DEFAULT '0',
  `agi` int(11) NOT NULL DEFAULT '0',
  `online` tinyint(1) NOT NULL DEFAULT '0',
  PRIMARY KEY (`commander_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;

-- --------------------------------------------------------

--
-- Table structure for table `login`
--

CREATE TABLE IF NOT EXISTS `login` (
  `account_id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `login` varchar(32) NOT NULL,
  `password` varchar(32) NOT NULL,
  `email` varchar(39) NOT NULL,
  `state` int(11) NOT NULL DEFAULT '0',
  `unban_time` date NOT NULL DEFAULT '0000-00-00',
  `logincount` int(10) unsigned NOT NULL,
  `lastlogin` date NOT NULL DEFAULT '0000-00-00',
  `lastip` varchar(100) NOT NULL,
  PRIMARY KEY (`account_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;
