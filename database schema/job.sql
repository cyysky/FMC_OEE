CREATE TABLE `job` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `job` varchar(45) DEFAULT NULL,
  `mold_name` varchar(45) DEFAULT NULL,
  `quantity` double DEFAULT NULL,
  `bag` varchar(45) DEFAULT NULL,
  `material` varchar(45) DEFAULT NULL,
  `cavity` int(11) DEFAULT NULL,
  `date_time` datetime DEFAULT CURRENT_TIMESTAMP,
  `user_id` int(11) DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `id_UNIQUE` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci