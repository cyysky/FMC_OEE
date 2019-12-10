CREATE TABLE `machine` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `machine_name` varchar(45) NOT NULL,
  `machine_key` varchar(45) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `machine_name_UNIQUE` (`machine_name`),
  UNIQUE KEY `machine_key_UNIQUE` (`machine_key`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=latin1