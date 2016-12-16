# Mysql InnoDB deadlock 相关资料

## 1. 查询InnoDB锁的整体情况

	MySQL [(none)]> show status like 'innodb_row_lock%';
	+-------------------------------+------------+--------------------------------+
	| Variable_name                 | Value      | Description                    |
	+-------------------------------+------------+--------------------------------+
	| Innodb_row_lock_current_waits | 43         |  当前等待row lock的数目          |
	| Innodb_row_lock_time          | 2600509530 |  获取row lock花费的总时间，单位毫秒|
	| Innodb_row_lock_time_avg      | 47165      |  平均时间                       |
	| Innodb_row_lock_time_max      | 51994      |  最大时间                       |
	| Innodb_row_lock_waits         | 55136      |  操作等待row lock的数目          |
	+-------------------------------+------------+--------------------------------+

如果 **Innodb_row_lock_waits** 和 **Innodb_row_lock_time_avg** 比较大，则表明锁竞争比较严重。

see also: [5.1.7 Server Status Variables](https://dev.mysql.com/doc/refman/5.6/en/server-status-variables.html)

## 2. 分析InnoDB锁的具体情况
MySQL 5.5 中，information_schema 库中新增了三个关于锁的表，innodb_trx、innodb_locks 和 innodb_lock_waits。
其中 innodb_trx 表记录当前运行的所有事务，innodb_locks 表记录当前出现的锁，innodb_lock_waits 表记录锁等待的对应关系。

	MySQL [(none)]> use information_schema; 
	MySQL [information_schema]> select * from information_schema.innodb_trx;
	MySQL [information_schema]> select * from information_schema.innodb_locks;
	MySQL [information_schema]> select * from information_schema.innodb_lock_waits;

innodb_trx 表结构：
	
	MySQL [information_schema]> DESC information_schema.innodb_trx;
	+----------------------------+---------------------+------+-----+---------------------+-------+
	| Field                      | Type                | Null | Key | Default             | Extra |
	+----------------------------+---------------------+------+-----+---------------------+-------+
	| trx_id                     | varchar(18)         | NO   |     |                     |       |
	| trx_state                  | varchar(13)         | NO   |     |                     |       |
	| trx_started                | datetime            | NO   |     | 0000-00-00 00:00:00 |       |
	| trx_requested_lock_id      | varchar(81)         | YES  |     | NULL                |       |
	| trx_wait_started           | datetime            | YES  |     | NULL                |       |
	| trx_weight                 | bigint(21) unsigned | NO   |     | 0                   |       |
	| trx_mysql_thread_id        | bigint(21) unsigned | NO   |     | 0                   |       |
	| trx_query                  | varchar(1024)       | YES  |     | NULL                |       |
	| trx_operation_state        | varchar(64)         | YES  |     | NULL                |       |
	| trx_tables_in_use          | bigint(21) unsigned | NO   |     | 0                   |       |
	| trx_tables_locked          | bigint(21) unsigned | NO   |     | 0                   |       |
	| trx_lock_structs           | bigint(21) unsigned | NO   |     | 0                   |       |
	| trx_lock_memory_bytes      | bigint(21) unsigned | NO   |     | 0                   |       |
	| trx_rows_locked            | bigint(21) unsigned | NO   |     | 0                   |       |
	| trx_rows_modified          | bigint(21) unsigned | NO   |     | 0                   |       |
	| trx_concurrency_tickets    | bigint(21) unsigned | NO   |     | 0                   |       |
	| trx_isolation_level        | varchar(16)         | NO   |     |                     |       |
	| trx_unique_checks          | int(1)              | NO   |     | 0                   |       |
	| trx_foreign_key_checks     | int(1)              | NO   |     | 0                   |       |
	| trx_last_foreign_key_error | varchar(256)        | YES  |     | NULL                |       |
	| trx_adaptive_hash_latched  | int(1)              | NO   |     | 0                   |       |
	| trx_adaptive_hash_timeout  | bigint(21) unsigned | NO   |     | 0                   |       |
	| trx_is_read_only           | int(1)              | NO   |     | 0                   |       |
	| trx_autocommit_non_locking | int(1)              | NO   |     | 0                   |       |
	+----------------------------+---------------------+------+-----+---------------------+-------+

innodb_locks 表结构：
	
	MySQL [information_schema]> DESC information_schema.innodb_locks;
	+-------------+---------------------+------+-----+---------+-------+
	| Field       | Type                | Null | Key | Default | Extra |
	+-------------+---------------------+------+-----+---------+-------+
	| lock_id     | varchar(81)         | NO   |     |         |       |
	| lock_trx_id | varchar(18)         | NO   |     |         |       |
	| lock_mode   | varchar(32)         | NO   |     |         |       |
	| lock_type   | varchar(32)         | NO   |     |         |       |
	| lock_table  | varchar(1024)       | NO   |     |         |       |
	| lock_index  | varchar(1024)       | YES  |     | NULL    |       |
	| lock_space  | bigint(21) unsigned | YES  |     | NULL    |       |
	| lock_page   | bigint(21) unsigned | YES  |     | NULL    |       |
	| lock_rec    | bigint(21) unsigned | YES  |     | NULL    |       |
	| lock_data   | varchar(8192)       | YES  |     | NULL    |       |
	+-------------+---------------------+------+-----+---------+-------+
	10 rows in set (0.00 sec)
	
innodb_ lock__waits 表结构:

	MySQL [information_schema]> DESC information_schema.innodb_lock_waits;
	+-------------------+-------------+------+-----+---------+-------+
	| Field             | Type        | Null | Key | Default | Extra |
	+-------------------+-------------+------+-----+---------+-------+
	| requesting_trx_id | varchar(18) | NO   |     |         |       |
	| requested_lock_id | varchar(81) | NO   |     |         |       |
	| blocking_trx_id   | varchar(18) | NO   |     |         |       |
	| blocking_lock_id  | varchar(81) | NO   |     |         |       |
	| requesting_thd_id | varchar(18) | NO   |     |         |       |
	| blocking_thd_id   | varchar(18) | NO   |     |         |       |
	+-------------------+-------------+------+-----+---------+-------+
	6 rows in set (0.00 sec)

## 3. 参看当前运行的事务

	MySQL [(none)]> show processlist; or show full processlist;
	

## 4. 模式InnoDB死锁

### 4.1 数据准备

创建数据表：

	CREATE TABLE user(id INT PRIMARY KEY, name VARCHAR(20)) DEFAULT CHARSET utf8 ENGINE = INNODB;

插入数据：

	INSERT INTO user(id, name) VALUES(1, 'A');
	INSERT INTO user(id, name) VALUES(2, 'B');
	INSERT INTO user(id, name) VALUES(3, 'C');
	
### 4.2 模拟死锁

#### 4.2.1 事务1开始

	mysql> START TRANSACTION;
	Query OK, 0 rows affected (0.00 sec)
	
	mysql> UPDATE user SET name='AA' WHERE id = 1;
	Query OK, 1 row affected (0.01 sec)
	Rows matched: 1  Changed: 1  Warnings: 0
	
查看相关事务记录：

	mysql> SELECT * FROM information_schema.innodb_trx \G
	*************************** 1. row ***************************
	                    trx_id: 266782
	                 trx_state: RUNNING
	               trx_started: 2016-11-23 14:44:53
	     trx_requested_lock_id: NULL
	          trx_wait_started: NULL
	                trx_weight: 3
	       trx_mysql_thread_id: 2
	                 trx_query: NULL
	       trx_operation_state: NULL
	         trx_tables_in_use: 0
	         trx_tables_locked: 1
	          trx_lock_structs: 2
	     trx_lock_memory_bytes: 1136
	           trx_rows_locked: 1
	         trx_rows_modified: 1
	   trx_concurrency_tickets: 0
	       trx_isolation_level: REPEATABLE READ
	         trx_unique_checks: 1
	    trx_foreign_key_checks: 1
	trx_last_foreign_key_error: NULL
	 trx_adaptive_hash_latched: 0
	 trx_adaptive_hash_timeout: 0
	          trx_is_read_only: 0
	trx_autocommit_non_locking: 0
	1 row in set (0.00 sec)
	
	
	mysql> SELECT * FROM information_schema.innodb_locks \G
	Empty set (0.00 sec)

	mysql> SELECT * FROM information_schema.innodb_lock_waits \G
	Empty set (0.00 sec)
	

#### 4.2.2 事务2开始

锁超时时间设置，默认时间为50s，如果超过50s，则提示ERROR 1205 (HY000): Lock wait timeout exceeded。

	mysql> SHOW VARIABLES LIKE '%innodb_lock_wait%';
	+--------------------------+-------+
	| Variable_name            | Value |
	+--------------------------+-------+
	| innodb_lock_wait_timeout | 50    |
	+--------------------------+-------+
	1 row in set (0.01 sec)

	mysql> UPDATE user SET name='AAA' WHERE id = 1;
	ERROR 1205 (HY000): Lock wait timeout exceeded; try restarting transaction

	mysql> SELECT * FROM information_schema.innodb_trx \G
	*************************** 1. row ***************************
	                    trx_id: 266783
	                 trx_state: LOCK WAIT
	               trx_started: 2016-11-23 14:54:32
	     trx_requested_lock_id: 266783:172:3:5
	          trx_wait_started: 2016-11-23 14:54:32
	                trx_weight: 2
	       trx_mysql_thread_id: 3
	                 trx_query: UPDATE user SET name='AAA' WHERE id = 1
	       trx_operation_state: starting index read
	         trx_tables_in_use: 1
	         trx_tables_locked: 1
	          trx_lock_structs: 2
	     trx_lock_memory_bytes: 1136
	           trx_rows_locked: 1
	         trx_rows_modified: 0
	   trx_concurrency_tickets: 0
	       trx_isolation_level: REPEATABLE READ
	         trx_unique_checks: 1
	    trx_foreign_key_checks: 1
	trx_last_foreign_key_error: NULL
	 trx_adaptive_hash_latched: 0
	 trx_adaptive_hash_timeout: 0
	          trx_is_read_only: 0
	trx_autocommit_non_locking: 0
	*************************** 2. row ***************************
	                    trx_id: 266782
	                 trx_state: RUNNING
	               trx_started: 2016-11-23 14:44:53
	     trx_requested_lock_id: NULL
	          trx_wait_started: NULL
	                trx_weight: 3
	       trx_mysql_thread_id: 2
	                 trx_query: NULL
	       trx_operation_state: NULL
	         trx_tables_in_use: 0
	         trx_tables_locked: 1
	          trx_lock_structs: 2
	     trx_lock_memory_bytes: 1136
	           trx_rows_locked: 1
	         trx_rows_modified: 1
	   trx_concurrency_tickets: 0
	       trx_isolation_level: REPEATABLE READ
	         trx_unique_checks: 1
	    trx_foreign_key_checks: 1
	trx_last_foreign_key_error: NULL
	 trx_adaptive_hash_latched: 0
	 trx_adaptive_hash_timeout: 0
	          trx_is_read_only: 0
	trx_autocommit_non_locking: 0
	2 rows in set (0.00 sec)
	
调整锁超时时间重试：

	mysql> SHOW VARIABLES LIKE '%innodb_lock_wait%';
	+--------------------------+-------+
	| Variable_name            | Value |
	+--------------------------+-------+
	| innodb_lock_wait_timeout | 50    |
	+--------------------------+-------+
	1 row in set (0.01 sec)
	
	mysql> SET innodb_lock_wait_timeout=600;
	Query OK, 0 rows affected (0.00 sec)
	
	mysql> SET GLOBAL innodb_lock_wait_timeout=600;
	Query OK, 0 rows affected (0.00 sec)
	
	mysql> SHOW VARIABLES LIKE '%innodb_lock_wait%';
	+--------------------------+-------+
	| Variable_name            | Value |
	+--------------------------+-------+
	| innodb_lock_wait_timeout | 600   |
	+--------------------------+-------+
	
	1 row in set (0.01 sec)
	
重新启动死锁测试：
	
	mysql> UPDATE user SET name='AAA' WHERE id = 1;
	
	mysql> show full processlist;
	+----+------+-----------+------+---------+------+----------+-----------------------------------------+
	| Id | User | Host      | db   | Command | Time | State    | Info                                    |
	+----+------+-----------+------+---------+------+----------+-----------------------------------------+
	|  2 | root | localhost | test | Sleep   | 1079 |          | NULL                                    |
	|  3 | root | localhost | test | Query   |    8 | updating | UPDATE user SET name='AAA' WHERE id = 1 |
	|  4 | root | localhost | test | Query   |    0 | starting | show full processlist                   |
	+----+------+-----------+------+---------+------+----------+-----------------------------------------+
	3 rows in set (0.00 sec)

	mysql> select * from information_schema.innodb_trx\G
	*************************** 1. row ***************************
	                    trx_id: 266785
	                 trx_state: LOCK WAIT
	               trx_started: 2016-11-23 15:04:15
	     trx_requested_lock_id: 266785:172:3:5
	          trx_wait_started: 2016-11-23 15:04:15
	                trx_weight: 2
	       trx_mysql_thread_id: 4
	                 trx_query: UPDATE user SET name='AAA' WHERE id = 1
	       trx_operation_state: starting index read
	         trx_tables_in_use: 1
	         trx_tables_locked: 1
	          trx_lock_structs: 2
	     trx_lock_memory_bytes: 1136
	           trx_rows_locked: 1
	         trx_rows_modified: 0
	   trx_concurrency_tickets: 0
	       trx_isolation_level: REPEATABLE READ
	         trx_unique_checks: 1
	    trx_foreign_key_checks: 1
	trx_last_foreign_key_error: NULL
	 trx_adaptive_hash_latched: 0
	 trx_adaptive_hash_timeout: 0
	          trx_is_read_only: 0
	trx_autocommit_non_locking: 0
	*************************** 2. row ***************************
	                    trx_id: 266782
	                 trx_state: RUNNING
	               trx_started: 2016-11-23 14:44:53
	     trx_requested_lock_id: NULL
	          trx_wait_started: NULL
	                trx_weight: 3
	       trx_mysql_thread_id: 2
	                 trx_query: NULL
	       trx_operation_state: NULL
	         trx_tables_in_use: 0
	         trx_tables_locked: 1
	          trx_lock_structs: 2
	     trx_lock_memory_bytes: 1136
	           trx_rows_locked: 1
	         trx_rows_modified: 1
	   trx_concurrency_tickets: 0
	       trx_isolation_level: REPEATABLE READ
	         trx_unique_checks: 1
	    trx_foreign_key_checks: 1
	trx_last_foreign_key_error: NULL
	 trx_adaptive_hash_latched: 0
	 trx_adaptive_hash_timeout: 0
	          trx_is_read_only: 0
	trx_autocommit_non_locking: 0
	2 rows in set (0.00 sec)


	mysql> select * from information_schema.innodb_locks;
	+----------------+-------------+-----------+-----------+---------------+------------+------------+-----------+----------+-----------+
	| lock_id        | lock_trx_id | lock_mode | lock_type | lock_table    | lock_index | lock_space | lock_page | lock_rec | lock_data |
	+----------------+-------------+-----------+-----------+---------------+------------+------------+-----------+----------+-----------+
	| 266785:172:3:5 | 266785      | X         | RECORD    | `test`.`user` | PRIMARY    |        172 |         3 |        5 | 1         |
	| 266782:172:3:5 | 266782      | X         | RECORD    | `test`.`user` | PRIMARY    |        172 |         3 |        5 | 1         |
	+----------------+-------------+-----------+-----------+---------------+------------+------------+-----------+----------+-----------+
	2 rows in set (0.00 sec)

	mysql> select * from information_schema.innodb_lock_waits;
	+-------------------+-------------------+-----------------+------------------+
	| requesting_trx_id | requested_lock_id | blocking_trx_id | blocking_lock_id |
	+-------------------+-------------------+-----------------+------------------+
	| 266785            | 266785:172:3:5    | 266782          | 266782:172:3:5   |
	+-------------------+-------------------+-----------------+------------------+
	1 row in set (0.00 sec)
	
	通过 information_schema.innodb_trx 表我们得知 266785执行的是 “UPDATE user SET name='AAA' WHERE id = 1”，上述表表明事务id 266785被事务id 266782 blocked，而事务id 266782这是我们的没有conmmit的事务1。
	
	
跨表联合进行查询：

		mysql> SELECT * FROM innodb_locks WHERE lock_trx_id IN (SELECT blocking_trx_id FROM innodb_lock_waits) \G
	*************************** 1. row ***************************
	    lock_id: 266782:172:3:5
	lock_trx_id: 266782
	  lock_mode: X
	  lock_type: RECORD
	 lock_table: `test`.`user`
	 lock_index: PRIMARY
	 lock_space: 172
	  lock_page: 3
	   lock_rec: 5
	  lock_data: 1
	1 row in set (0.00 sec)
	
	
	mysql> SELECT innodb_locks.* FROM innodb_locks JOIN innodb_lock_waits ON (innodb_locks.lock_trx_id = innodb_lock_waits.blocking_trx_id) \G
	*************************** 1. row ***************************
	    lock_id: 266782:172:3:5
	lock_trx_id: 266782
	  lock_mode: X
	  lock_type: RECORD
	 lock_table: `test`.`user`
	 lock_index: PRIMARY
	 lock_space: 172
	  lock_page: 3
	   lock_rec: 5
	  lock_data: 1
	1 row in set (0.01 sec)


查看 ENGINE INNODB STATUS 表状态：

	mysql> SHOW ENGINE INNODB STATUS\G
	*************************** 1. row ***************************
	  Type: InnoDB
	  Name:
	Status:
	=====================================
	2016-11-23 15:11:05 0x14e9dc000 INNODB MONITOR OUTPUT
	=====================================
	Per second averages calculated from the last 1 seconds
	-----------------
	BACKGROUND THREAD
	-----------------
	srv_master_thread loops: 10 srv_active, 0 srv_shutdown, 1876 srv_idle
	srv_master_thread log flush and writes: 1886
	----------
	SEMAPHORES
	----------
	OS WAIT ARRAY INFO: reservation count 94
	OS WAIT ARRAY INFO: signal count 118
	RW-shared spins 0, rounds 611, OS waits 40
	RW-excl spins 0, rounds 952, OS waits 2
	RW-sx spins 0, rounds 0, OS waits 0
	Spin rounds per wait: 611.00 RW-shared, 952.00 RW-excl, 0.00 RW-sx
	------------
	TRANSACTIONS
	------------
	Trx id counter 266787
	Purge done for trx's n:o < 266781 undo n:o < 0 state: running but idle
	History list length 1219
	LIST OF TRANSACTIONS FOR EACH SESSION:
	---TRANSACTION 281479649524408, not started
	0 lock struct(s), heap size 1136, 0 row lock(s)
	
	--------------------------------------------------------------
	---TRANSACTION 266785, ACTIVE 410 sec starting index read
	mysql tables in use 1, locked 1
	LOCK WAIT 2 lock struct(s), heap size 1136, 1 row lock(s)
	MySQL thread id 4, OS thread handle 5614206976, query id 86 localhost root updating
	UPDATE user SET name='AAA' WHERE id = 1
	------- TRX HAS BEEN WAITING 410 SEC FOR THIS LOCK TO BE GRANTED:
	RECORD LOCKS space id 172 page no 3 n bits 72 index PRIMARY of table `test`.`user` trx id 266785 lock_mode X locks rec but not gap waiting
	Record lock, heap no 5 PHYSICAL RECORD: n_fields 4; compact format; info bits 0
	 0: len 4; hex 80000001; asc     ;;
	 1: len 6; hex 00000004121e; asc       ;;
	 2: len 7; hex 390000025105b6; asc 9   Q  ;;
	 3: len 2; hex 4141; asc AA;;
	
	-------------------------------------------------------------
	---TRANSACTION 266782, ACTIVE 1572 sec
	2 lock struct(s), heap size 1136, 1 row lock(s), undo log entries 1
	MySQL thread id 2, OS thread handle 5604212736, query id 48 localhost root cleaning up
	
	--------
	FILE I/O
	--------
	I/O thread 0 state: waiting for i/o request (insert buffer thread)
	I/O thread 1 state: waiting for i/o request (log thread)
	I/O thread 2 state: waiting for i/o request (read thread)
	I/O thread 3 state: waiting for i/o request (read thread)
	I/O thread 4 state: waiting for i/o request (read thread)
	I/O thread 5 state: waiting for i/o request (read thread)
	I/O thread 6 state: waiting for i/o request (write thread)
	I/O thread 7 state: waiting for i/o request (write thread)
	I/O thread 8 state: waiting for i/o request (write thread)
	I/O thread 9 state: waiting for i/o request (write thread)
	Pending normal aio reads: [0, 0, 0, 0] , aio writes: [0, 0, 0, 0] ,
	 ibuf aio reads:, log i/o's:, sync i/o's:
	Pending flushes (fsync) log: 0; buffer pool: 0
	545 OS file reads, 150 OS file writes, 72 OS fsyncs
	0.00 reads/s, 0 avg bytes/read, 0.00 writes/s, 0.00 fsyncs/s
	-------------------------------------
	INSERT BUFFER AND ADAPTIVE HASH INDEX
	-------------------------------------
	Ibuf: size 1, free list len 0, seg size 2, 0 merges
	merged operations:
	 insert 0, delete mark 0, delete 0
	discarded operations:
	 insert 0, delete mark 0, delete 0
	Hash table size 34673, node heap has 0 buffer(s)
	Hash table size 34673, node heap has 0 buffer(s)
	Hash table size 34673, node heap has 0 buffer(s)
	Hash table size 34673, node heap has 0 buffer(s)
	Hash table size 34673, node heap has 1 buffer(s)
	Hash table size 34673, node heap has 0 buffer(s)
	Hash table size 34673, node heap has 0 buffer(s)
	Hash table size 34673, node heap has 0 buffer(s)
	0.00 hash searches/s, 0.00 non-hash searches/s
	---
	LOG
	---
	Log sequence number 933573420
	Log flushed up to   933573420
	Pages flushed up to 933573420
	Last checkpoint at  933573411
	0 pending log flushes, 0 pending chkp writes
	49 log i/o's done, 0.00 log i/o's/second
	----------------------
	BUFFER POOL AND MEMORY
	----------------------
	Total large memory allocated 137428992
	Dictionary memory allocated 594239
	Buffer pool size   8191
	Free buffers       7654
	Database pages     536
	Old database pages 216
	Modified db pages  0
	Pending reads 0
	Pending writes: LRU 0, flush list 0, single page 0
	Pages made young 0, not young 0
	0.00 youngs/s, 0.00 non-youngs/s
	Pages read 497, created 39, written 85
	0.00 reads/s, 0.00 creates/s, 0.00 writes/s
	Buffer pool hit rate 1000 / 1000, young-making rate 0 / 1000 not 0 / 1000
	Pages read ahead 0.00/s, evicted without access 0.00/s, Random read ahead 0.00/s
	LRU len: 536, unzip_LRU len: 0
	I/O sum[0]:cur[0], unzip sum[0]:cur[0]
	--------------
	ROW OPERATIONS
	--------------
	0 queries inside InnoDB, 0 queries in queue
	0 read views open inside InnoDB
	Process ID=88056, Main thread ID=5577498624, state: sleeping
	Number of rows inserted 3, updated 1, deleted 0, read 13
	0.00 inserts/s, 0.00 updates/s, 0.00 deletes/s, 0.00 reads/s
	----------------------------
	END OF INNODB MONITOR OUTPUT
	============================
	
	1 row in set (0.00 sec)	
	
## 5 解决死锁

	mysql > kill 4   #show processlist输出的id编号
	
## 参考
1. [高级InnoDB死锁故障排除,SHOW INNODB STATUS可没有告诉你这些](http://www.javacoder.cn/?p=61)
2. [[MySQL FAQ]系列 — 如何查看当前最新事务ID](http://imysql.com/2015/03/25/mysql-faq-how-to-fetch-latest-trxid.shtml)
3. [【MySQL】死锁问题分析](http://blog.csdn.net/lapiggy/article/details/6715641)
4. [InnoDB’s gap locks](https://www.percona.com/blog/2012/03/27/innodbs-gap-locks/)
5. [巧用MySQL InnoDB引擎锁机制解决死锁问题](http://www.cnblogs.com/Arlen/articles/1756915.html)
6. [mysql InnoDB锁等待的查看及分析](http://blog.itpub.net/12679300/viewspace-1420031/)
7. [MySQL 5.5 InnoDB 锁等待](https://dbarobin.com/2015/01/27/innodb-lock-wait-under-mysql-5.5/)

	
<table class="table table-bordered table-striped table-condensed">   
</table> 