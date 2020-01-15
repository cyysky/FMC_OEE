CREATE TABLE `machine` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `machine_name` varchar(45) NOT NULL,
  `machine_key` varchar(45) NOT NULL,
  `location` varchar(45) DEFAULT NULL,
  `active` tinyint(4) DEFAULT NULL,
  `alive` tinyint(4) DEFAULT NULL,
  `active_job_id` int(11) DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `machine_name_UNIQUE` (`machine_name`),
  UNIQUE KEY `machine_key_UNIQUE` (`machine_key`)
) ENGINE=InnoDB AUTO_INCREMENT=34 DEFAULT CHARSET=latin1