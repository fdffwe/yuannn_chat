-- Create messages table for chat history
DROP TABLE IF EXISTS `message`;
CREATE TABLE `message` (
  `message_id` BIGINT NOT NULL AUTO_INCREMENT,
  `thread_id` INT NOT NULL DEFAULT 0,
  `fromuid` INT NOT NULL DEFAULT 0,
  `touid` INT NOT NULL DEFAULT 0,
  `content` TEXT,
  `created_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`message_id`),
  INDEX `idx_thread_msgid`(`thread_id`, `message_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
