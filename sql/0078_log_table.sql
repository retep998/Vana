CREATE TABLE `logs` (
  `id` bigint(23) UNSIGNED NOT NULL AUTO_INCREMENT,
  `log_time` datetime NOT NULL,
  `origin` enum('login','world','channel','cash','mts') NOT NULL,
  `info_type` enum('info','warning','debug','error','critical_error','server_connect','server_disconnect','server_auth_error','login','login_auth_error','logout','client_error','gm_command','admin_command','boss_kill','trade','shop_transaction','storage_transaction','instance_begin','drop','chat','whisper','malformed_packet','script_log') NOT NULL,
  `identifier` varchar(20) NOT NULL DEFAULT '',
  `message` TEXT NOT NULL,
  PRIMARY KEY (`id`)
);