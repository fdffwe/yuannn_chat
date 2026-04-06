-- MySQL dump 10.13  Distrib 8.0.45, for Linux (x86_64)
--
-- Host: localhost    Database: yuannn
-- ------------------------------------------------------
-- Server version	8.0.45

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!50503 SET NAMES utf8mb4 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Current Database: `yuannn`
--

CREATE DATABASE /*!32312 IF NOT EXISTS*/ `yuannn` /*!40100 DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci */ /*!80016 DEFAULT ENCRYPTION='N' */;

USE `yuannn`;

--
-- Table structure for table `chat_thread`
--

DROP TABLE IF EXISTS `chat_thread`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `chat_thread` (
  `thread_id` bigint unsigned NOT NULL AUTO_INCREMENT,
  `type` varchar(16) COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT 'private',
  `created_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `updated_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`thread_id`)
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `chat_thread`
--

LOCK TABLES `chat_thread` WRITE;
/*!40000 ALTER TABLE `chat_thread` DISABLE KEYS */;
INSERT INTO `chat_thread` VALUES (1,'private','2026-04-03 07:54:37','2026-04-03 07:54:37'),(2,'private','2026-04-05 03:47:55','2026-04-05 03:47:55'),(3,'private','2026-04-05 04:55:07','2026-04-05 04:55:07');
/*!40000 ALTER TABLE `chat_thread` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `friend`
--

DROP TABLE IF EXISTS `friend`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `friend` (
  `id` int unsigned NOT NULL AUTO_INCREMENT,
  `self_id` int NOT NULL,
  `friend_id` int NOT NULL,
  `back` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci DEFAULT '',
  PRIMARY KEY (`id`) USING BTREE,
  UNIQUE KEY `self_friend` (`self_id`,`friend_id`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=107 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `friend`
--

LOCK TABLES `friend` WRITE;
/*!40000 ALTER TABLE `friend` DISABLE KEYS */;
INSERT INTO `friend` VALUES (105,1059,1060,'222'),(106,1060,1059,'yuannn');
/*!40000 ALTER TABLE `friend` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `friend_apply`
--

DROP TABLE IF EXISTS `friend_apply`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `friend_apply` (
  `id` bigint NOT NULL AUTO_INCREMENT,
  `from_uid` int NOT NULL,
  `to_uid` int NOT NULL,
  `status` smallint NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`) USING BTREE,
  UNIQUE KEY `from_to_uid` (`from_uid`,`to_uid`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=82 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `friend_apply`
--

LOCK TABLES `friend_apply` WRITE;
/*!40000 ALTER TABLE `friend_apply` DISABLE KEYS */;
INSERT INTO `friend_apply` VALUES (81,1060,1059,1);
/*!40000 ALTER TABLE `friend_apply` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `message`
--

DROP TABLE IF EXISTS `message`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `message` (
  `message_id` bigint NOT NULL AUTO_INCREMENT,
  `thread_id` int NOT NULL DEFAULT '0',
  `fromuid` int NOT NULL DEFAULT '0',
  `touid` int NOT NULL DEFAULT '0',
  `content` text COLLATE utf8mb4_unicode_ci,
  `created_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`message_id`),
  KEY `idx_thread_msgid` (`thread_id`,`message_id`)
) ENGINE=InnoDB AUTO_INCREMENT=38 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `message`
--

LOCK TABLES `message` WRITE;
/*!40000 ALTER TABLE `message` DISABLE KEYS */;
INSERT INTO `message` VALUES (14,1,1059,1060,'你好，这是我发送的离线测试呢！！！','2026-04-05 04:00:46'),(15,1,1060,1059,'欧克欧克，我已经收到你发的测试消息了！！！','2026-04-05 04:01:18'),(16,1,1060,1059,'这是怎么回事呢？？？','2026-04-05 04:39:00'),(17,1,1059,1060,'功能测试完成啦','2026-04-05 04:48:02'),(18,1,1060,1059,'收到收到','2026-04-05 04:48:17'),(19,1,1059,1060,'111','2026-04-05 04:48:43'),(20,1,1059,1060,'11','2026-04-05 04:48:45'),(21,1,1059,1060,'1','2026-04-05 04:48:46'),(22,1,1059,1060,'11','2026-04-05 04:48:46'),(23,1,1059,1060,'1','2026-04-05 04:48:46'),(24,1,1059,1060,'1','2026-04-05 04:48:46'),(25,1,1059,1060,'1','2026-04-05 04:48:47'),(26,1,1059,1060,'1','2026-04-05 04:48:47'),(27,1,1059,1060,'1','2026-04-05 04:48:47'),(28,1,1059,1060,'1','2026-04-05 04:48:47'),(29,1,1060,1059,'11','2026-04-05 04:48:51'),(30,1,1060,1059,'11','2026-04-05 04:48:51'),(31,1,1060,1059,'1','2026-04-05 04:48:51'),(32,1,1060,1059,'1','2026-04-05 04:48:51'),(33,3,1059,0,'发的伽师','2026-04-05 04:55:07'),(34,3,1059,0,'士大夫啊','2026-04-05 04:55:09'),(35,3,1059,0,'方法撒','2026-04-05 04:55:25'),(36,1,1059,1060,'111','2026-04-05 04:55:34'),(37,2,1060,0,'111','2026-04-05 04:58:47');
/*!40000 ALTER TABLE `message` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `private_chat`
--

DROP TABLE IF EXISTS `private_chat`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `private_chat` (
  `thread_id` bigint unsigned NOT NULL,
  `user1_id` bigint unsigned NOT NULL,
  `user2_id` bigint unsigned NOT NULL,
  `created_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`thread_id`),
  UNIQUE KEY `uniq_private_thread` (`user1_id`,`user2_id`),
  KEY `idx_private_user1_thread` (`user1_id`,`thread_id`),
  KEY `idx_private_user2_thread` (`user2_id`,`thread_id`),
  CONSTRAINT `fk_private_thread` FOREIGN KEY (`thread_id`) REFERENCES `chat_thread` (`thread_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `private_chat`
--

LOCK TABLES `private_chat` WRITE;
/*!40000 ALTER TABLE `private_chat` DISABLE KEYS */;
INSERT INTO `private_chat` VALUES (1,1059,1060,'2026-04-03 07:54:37'),(2,0,1060,'2026-04-05 03:47:55'),(3,0,1059,'2026-04-05 04:55:07');
/*!40000 ALTER TABLE `private_chat` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `user`
--

DROP TABLE IF EXISTS `user`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `user` (
  `id` int NOT NULL AUTO_INCREMENT,
  `uid` int NOT NULL DEFAULT '0',
  `name` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '',
  `email` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '',
  `pwd` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '',
  `nick` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '',
  `desc` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '',
  `sex` int NOT NULL DEFAULT '0',
  `icon` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '',
  PRIMARY KEY (`id`) USING BTREE,
  UNIQUE KEY `uid` (`uid`) USING BTREE,
  UNIQUE KEY `email` (`email`) USING BTREE,
  KEY `name` (`name`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=63 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `user`
--

LOCK TABLES `user` WRITE;
/*!40000 ALTER TABLE `user` DISABLE KEYS */;
INSERT INTO `user` VALUES (3,1002,'llfc','secondtonone1@163.com','654321)','llfc','',0,':/res/head_1.jpg'),(4,1003,'tc','18165031775@qq.com','123456','tc','',0,':/res/head_1.jpg'),(5,1004,'yuanweihua','1456188862@qq.com','}kyn;89>?<','yuanweihua','',0,':/res/head_1.jpg'),(6,1005,'test','2022202210033@whu.edu.cn','}kyn;89>?<','test','',0,':/res/head_1.jpg'),(8,1007,'fhr','3157199927@qq.com','xuexi1228','fhr','',0,':/res/head_1.jpg'),(9,1008,'zglx2008','zglx2008@163.com','123456','zglx2008','',0,':/res/head_1.jpg'),(13,1012,'resettest','1042958553@qq.com','230745','resettest','',0,':/res/head_1.jpg'),(14,1013,'rss_test','1685229455@qq.com','123456','rss_test','',0,':/res/head_1.jpg'),(15,1014,'123456789','1152907774@qq.com','123456789','123456789','',0,':/res/head_1.jpg'),(16,1015,'aaaaaaa','3031719794@qq.com','777777','aaaaaaa','',0,':/res/head_1.jpg'),(17,1016,'aaa','2996722319@qq.com','222222','aaa','',0,':/res/head_1.jpg'),(20,1019,'zack','1017234088@qq.com','654321)','zack','',0,':/res/head_1.jpg'),(21,1020,'aatext','1584736136@qq.com','745230','aatext','',0,':/res/head_1.jpg'),(22,1021,'ferrero1','1220292901@qq.com','1234','ferrero1','',0,':/res/head_1.jpg'),(23,1022,'ferrero2','15504616642@163.com','1234','ferrero2','',0,':/res/head_1.jpg'),(24,1023,'lyf','3194811890@qq.com','123456','lyf','',0,':/res/head_1.jpg'),(25,1024,'lh','2494350589@qq.com','fb8::>:;8<','lh','',0,':/res/head_1.jpg'),(26,1025,'jf','luojianfeng553@163.com','745230','jf','',0,':/res/head_1.jpg'),(33,1031,'zjm','1013049201@qq.com','745230','zjm','',0,':/res/head_1.jpg'),(34,1032,'yxc','1003314442@qq.com','123','yxc','',0,':/res/head_1.jpg'),(37,1035,'wangyu','962087148@qq.com','123456','wangyu','',0,':/res/head_1.jpg'),(39,1037,'chae','318192621@qq.com','123456','chae','',0,':/res/head_1.jpg'),(40,1038,'summer','1586856388@qq.com','654321)','summer','',0,':/res/head_2.jpg'),(44,1042,'zzz','3434321837@qq.com','|l~745','','',0,''),(45,1043,'sadda','z1668047679@163.com','123456','','',0,''),(46,1044,'qwe','1368326038@qq.com','1234','','',0,''),(52,1050,'test23','161945471@qq.com','230745','test23','',0,':/res/head_3.jpg'),(53,1051,'123','1767269204@qq.com','123','','',0,''),(54,1052,'zjc','766741776@qq.com','745230','','',0,''),(55,1053,'test_1','zzsr_0719@qq.com','123456','','',0,''),(56,1054,'sqy','3175614975@qq.com','745230','sqy','',0,':/res/head_2.jpg'),(57,1055,'ocz','q3175614975@163.com','745230','ocz','',0,':/res/head_3.jpg'),(58,1056,'test28','1669475972@qq.com','230745','test28','',0,':/res/head_1.jpg'),(60,1058,'NoOne','1764850358@qq.com','745230','','',0,''),(61,1059,'111','3517649062@qq.com','111111','111','',0,':/res/head_3.jpg'),(62,1060,'222','2270378522@qq.com','222222','222','',0,':/res/head_2.jpg');
/*!40000 ALTER TABLE `user` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `user_id`
--

DROP TABLE IF EXISTS `user_id`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `user_id` (
  `id` int NOT NULL,
  PRIMARY KEY (`id`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `user_id`
--

LOCK TABLES `user_id` WRITE;
/*!40000 ALTER TABLE `user_id` DISABLE KEYS */;
INSERT INTO `user_id` VALUES (1060);
/*!40000 ALTER TABLE `user_id` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2026-04-05  5:48:16
