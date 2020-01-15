CREATE TABLE `machine_counter` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `machine_name_id` int(11) NOT NULL,
  `time_trigged` datetime(6) NOT NULL,
  `quantity` int(11) DEFAULT NULL,
  `missed` int(11) DEFAULT NULL,
  `job_id` int(11) DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `machine_id_index` (`machine_name_id`),
  KEY `time_index` (`time_trigged`)
) ENGINE=InnoDB AUTO_INCREMENT=440 DEFAULT CHARSET=latin1