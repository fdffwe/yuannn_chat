-- create chat thread and private chat mapping
DROP TABLE IF EXISTS `private_chat`;
DROP TABLE IF EXISTS `chat_thread`;

CREATE TABLE `chat_thread` (
  `thread_id` BIGINT UNSIGNED NOT NULL AUTO_INCREMENT,
  `type` VARCHAR(16) NOT NULL DEFAULT 'private',
  `created_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `updated_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`thread_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

CREATE TABLE `private_chat` (
  `thread_id` BIGINT UNSIGNED NOT NULL,
  `user1_id` BIGINT UNSIGNED NOT NULL,
  `user2_id` BIGINT UNSIGNED NOT NULL,
  `created_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`thread_id`),
  UNIQUE KEY `uniq_private_thread` (`user1_id`, `user2_id`),
  KEY `idx_private_user1_thread` (`user1_id`, `thread_id`),
  KEY `idx_private_user2_thread` (`user2_id`, `thread_id`),
  CONSTRAINT `fk_private_thread` FOREIGN KEY (`thread_id`) REFERENCES `chat_thread`(`thread_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
